/**
 * Copyright 2010 Riku Palomäki.
 * This file is part of GLSL Lab.
 *
 * GLSL Lab is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
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

#include <QtOpenGL>
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
    (*it)->render(state);
  }
}

TexturePtr Scene::texture(const QString& name) {
  if (m_textures.contains(name)) return m_textures[name];

  TexturePtr tex(new Texture(name));
  m_textures[name] = tex;
  return tex;
}

QList<ShaderPtr> Scene::shadersByFilename(const QString& filename) {
  QList<ShaderPtr> res;
  for (std::map<QString, ProgramPtr>::iterator it = m_shaders.begin(); it != m_shaders.end(); ++it) {
    GLProgram::Shaders shaders = it->second->shaders();
    for (GLProgram::Shaders::iterator it2 = shaders.begin(); it2 != shaders.end(); ++it2) {
      if ((*it2)->filename() == filename) res << *it2;
    }
  }
  return res;
}

void Scene::load(QVariantMap map) {
  QVariantMap tmp = map["objects"].toMap();
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
    if (object) m_objects[it.key()] = object;
  }

  tmp = map["lights"].toMap();
  for (QVariantMap::iterator it = tmp.begin(); it != tmp.end(); ++it) {
    LightPtr light(new Light(it.key()));
    light->load(it->toMap());
    m_lights[it.key()] = light;
  }

  tmp = map["cameras"].toMap();
  for (QVariantMap::iterator it = tmp.begin(); it != tmp.end(); ++it) {
    CameraPtr camera(new Camera(it.key()));
    camera->load(it->toMap());
    m_cameras[it.key()] = camera;
  }

  tmp = map["shaders"].toMap();
  for (QVariantMap::iterator it = tmp.begin(); it != tmp.end(); ++it) {
    ProgramPtr shader(new GLProgram(it.key()));
    foreach (QString name, it->toMap()["fragment"].toStringList())
      shader->addShader(name, Shader::Fragment);
    foreach (QString name, it->toMap()["vertex"].toStringList())
      shader->addShader(name, Shader::Vertex);
    foreach (QString name, it->toMap()["geometry"].toStringList())
      shader->addShader(name, Shader::Geometry);
    m_shaders[it.key()] = shader;
  }

  foreach (QVariant item, map["render passes"].toList()) {
    RenderPassPtr pass(new RenderPass(shared_from_this()));
    pass->load(item.toMap());
    m_render_passes.push_back(pass);
  }

  m_metainfo.load(map["lab"].toMap());
}
