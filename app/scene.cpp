/**
 * Copyright 2010 Riku Palomäki.
 * This file is part of GLSL Lab.
 *
 * GLSL Lab is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License Version 3 as
 * published by the Free Software Foundation.
 *
 * GLSL Lab is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GLSL Lab.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "scene.hpp"
#include "state.hpp"
#include "renderpass.hpp"
#include "object3d.hpp"
#include "light.hpp"
#include "camera.hpp"
#include "shader/program.hpp"
#include "shader/shader.hpp"
#include "texture.hpp"
#include "opengl.hpp"

#include <QVariantMap>

/// @todo move to Object3D
static ObjectPtr createObject(const QString& name) {
  if (name == "teapot") {
    return ObjectPtr(new Teapot);
  } else if (name == "box") {
    return ObjectPtr(new Box);
  }
  return ObjectPtr();
}

QVector3D toVector(QVariant in) {
  QVariantList lst = in.toList();
  if (lst.size() == 3) {
    return QVector3D(lst[0].toFloat(), lst[1].toFloat(), lst[2].toFloat());
  }
  return QVector3D();
}

QColor toColor(QVariant in) {
  QVariantList lst = in.toList();
  if (lst.size() == 4) {
    return QColor::fromRgbF(lst[0].toFloat(), lst[1].toFloat(), lst[2].toFloat(), lst[3].toFloat());
  }
  return QColor();
}

QVariantList toList(QVector3D in) {
  QVariantList ret;
  ret << in.x() << in.y() << in.z();
  return ret;
}

QVariantList toList(QColor in) {
  QVariantList ret;
  ret << in.redF() << in.greenF() << in.blueF() << in.alphaF();
  return ret;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

Scene::Scene() : m_width(-1), m_height(-1) {}

void Scene::resize(int width, int height) {
  if (width == m_width && height == m_height) return;

  m_width = width;
  m_height = height;
}

void Scene::render() {
  State state;
  for (RenderPasses::iterator it = m_render_passes.begin(); it != m_render_passes.end(); ++it) {
    RenderPassPtr p = it->second;
    p->render(state);
  }
}

TexturePtr Scene::genTexture(const QString& name) {
  if (m_textures.contains(name)) return m_textures[name];

  TexturePtr tex(new Texture(name));
  m_textures[name] = tex;
  emit textureListUpdated();
  return tex;
}

QList<ShaderPtr> Scene::shadersByFilename(const QString& filename) {
  QList<ShaderPtr> res;
  foreach (ProgramPtr p, m_shaders)
    foreach (ShaderPtr s, p->shaders())
      if (s->filename() == filename) res << s;
  return res;
}

QVariantMap Scene::save() const {
  QVariantMap map;
  QVariantMap tmp = m_metainfo.save();
  if (!tmp.isEmpty()) map["lab"] = tmp;
  QVariantMap objects, lights, cameras, shaders;
  QVariantList render_passes;

  foreach (QString name, m_objects.keys())
    objects[name] = m_objects[name]->save();
  if (!objects.isEmpty()) map["objects"] = objects;

  foreach (QString name, m_lights.keys())
    lights[name] = m_lights[name]->save();
  if (!lights.isEmpty()) map["lights"] = lights;

  foreach (QString name, m_cameras.keys())
    cameras[name] = m_cameras[name]->save();
  if (!cameras.isEmpty()) map["cameras"] = cameras;

  foreach (QString name, m_shaders.keys())
    shaders[name] = m_shaders[name]->save();
  if (!shaders.isEmpty()) map["shaders"] = shaders;

  foreach (RenderPasses::value_type p, m_render_passes)
    render_passes << p.second->save();
  if (!render_passes.isEmpty()) map["render passes"] = render_passes;

  return map;
}

void Scene::load(QVariantMap map) {
  QVariantMap tmp = map["objects"].toMap();
  bool changed = false;
  for (QVariantMap::iterator it = tmp.begin(); it != tmp.end(); ++it) {
    QVariantMap item = it->toMap();
    ObjectPtr object;
    QStringList model = item["model"].toStringList();
    if (model.size() == 2) {
      if (model[0] == "built-in") {
        object = createObject(model[1]);
      } else if (model[0] == "file") {
        // object = loadModel(model[1]);
      }
    }
    if (object) {
      m_objects[it.key()] = object;
      changed = true;
    }
  }
  if (changed)
    emit objectListUpdated();

  tmp = map["lights"].toMap();
  for (QVariantMap::iterator it = tmp.begin(); it != tmp.end(); ++it) {
    LightPtr light(new Light(it.key()));
    light->load(it->toMap());
    m_lights[it.key()] = light;
  }
  if (!tmp.isEmpty())
    emit lightListUpdated();

  tmp = map["cameras"].toMap();
  for (QVariantMap::iterator it = tmp.begin(); it != tmp.end(); ++it) {
    CameraPtr camera(new Camera(it.key()));
    camera->load(it->toMap());
    m_cameras[it.key()] = camera;
  }
  if (!tmp.isEmpty())
    emit cameraListUpdated();

  tmp = map["shaders"].toMap();
  for (QVariantMap::iterator it = tmp.begin(); it != tmp.end(); ++it) {
    ProgramPtr shader(new GLProgram(it.key()));
    foreach (QString filename, it->toMap()["fragment"].toStringList())
      shader->addShader(search(filename), Shader::Fragment);
    foreach (QString filename, it->toMap()["vertex"].toStringList())
      shader->addShader(search(filename), Shader::Vertex);
    foreach (QString filename, it->toMap()["geometry"].toStringList())
      shader->addShader(search(filename), Shader::Geometry);
    m_shaders[it.key()] = shader;
  }
  if (!tmp.isEmpty())
    emit shaderListUpdated();

  foreach (QVariant item, map["render passes"].toList()) {
    QVariantMap map = item.toMap();
    RenderPassPtr pass(new RenderPass(map["name"].toString(), shared_from_this()));
    pass->load(map);
    m_render_passes.push_back(qMakePair(pass->name(), pass));
  }

  m_metainfo.load(map["lab"].toMap());
}

QString Scene::search(QString filename) const {
  if (m_root.isEmpty())
    return QDir(filename).canonicalPath();
  QString cwd = QDir::currentPath();
  QDir::setCurrent(m_root);
  QString ret = QDir(filename).canonicalPath();
  QDir::setCurrent(cwd);
  return ret;
}
