/**
 * Copyright 2010,2011 Riku Palomäki.
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
#include "material.hpp"
#include "obj_importer.hpp"
#include "model.hpp"
#include "utils.hpp"

#include <QVariantMap>

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

namespace {
struct Import {
  ObjImporter::Filter filter;
  ObjImporter::Options options;
  QString file;
};

void loadRefs(QMap<QString, Import>& imports, QVariant src,
              QSet<QString> ObjImporter::Filter::*target) {
  QVariantMap tmp = src.toMap();
  for (QVariantMap::iterator it = tmp.begin(); it != tmp.end(); ++it) {
    QVariantMap item = it->toMap();
    if (item.contains("ref")) {
      QStringList ref = item["ref"].toStringList();
      imports[ref[0]].filter.*target << QString(ref.size() > 1 ? ref[1] : "");
    }
  }
}

struct P {
  QString name;
  QVariantMap map;
};

QList<P> iterate(QVariant map) {
  QList<P> out;
  QVariantMap tmp = map.toMap();
  for (QVariantMap::iterator it = tmp.begin(); it != tmp.end(); ++it) {
    P p;
    p.name = it.key();
    p.map = it.value().toMap();
    out << p;
  }
  return out;
}

template <typename T>
std::shared_ptr<T> clone(QMap<QString, ObjImporter::Scene>& imported, const P& p,
                         QMap<QString, std::shared_ptr<T> > ObjImporter::Scene::*src,
                         bool create = true) {
  typedef std::shared_ptr<T> Ptr;
  QStringList ref = p.map["ref"].toStringList();
  if (!ref.isEmpty()) {
    if (ref.size() == 1 || ref[1].isEmpty()) {
      foreach (Ptr t, imported[ref[0]].*src)
        if (t) {
          Log::debug("Importing first object (%s)", t->name().toUtf8().data());
          return t->clone();
        }
      Log::error("Couldn't find any objects for ref %s", p.name.toUtf8().data());
    } else {
      Ptr t = (imported[ref[0]].*src).value(ref[1]);
      if (t) {
        Log::debug("Importing %s", t->name().toUtf8().data());
        return t->clone();
      } else {
        Log::error("Couldn't find ref %s for %s", ref[1].toUtf8().data(), p.name.toUtf8().data());
        Log::info("Choices are: %s", QStringList((imported[ref[0]].*src).keys()).join(", ").toUtf8().data());
      }
    }
  }
  return Ptr(create ? new T(p.name) : 0);
}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

Scene::Scene(QString filename) : m_width(-1), m_height(-1), m_filename(filename), m_node(new Node) {}

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
  Log::debug("genTexture(%s): %s", name.toUtf8().data(),
             QStringList(m_textures.keys()).join(", ").toUtf8().data());
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
  QVariantMap objects, lights, cameras, materials;
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

/*  foreach (QString name, m_materials.keys())
    materials[name] = m_materials[name]->save(m_root);
  if (!materials.isEmpty()) map["materials"] = materials;*/

  foreach (RenderPasses::value_type p, m_render_passes)
    render_passes << p.second->save();
  if (!render_passes.isEmpty()) map["render passes"] = render_passes;

  return map;
}

void Scene::load(QVariantMap map) {
  ObjImporter importer;
  QMap<QString, Import> imports;
  QMap<QString, ObjImporter::Scene> imported;

  m_metainfo.load(map["lab"].toMap());

  QVariantMap tmp = map["import"].toMap();
  for (QVariantMap::iterator it = tmp.begin(); it != tmp.end(); ++it) {
    Import& im = imports[it.key()];
    QVariantMap item = it->toMap();
    im.file = item["file"].toString();
    foreach (QString flag, item["flags"].toStringList())
      im.options[flag] = true;
    /// @todo m_imports = stuff
  }

  /// @todo should this be objects instead of models?
  loadRefs(imports, map["models"], &ObjImporter::Filter::models);
  loadRefs(imports, map["textures"], &ObjImporter::Filter::textures);
  loadRefs(imports, map["materials"], &ObjImporter::Filter::materials);
  loadRefs(imports, map["animations"], &ObjImporter::Filter::animations);
  loadRefs(imports, map["camera"], &ObjImporter::Filter::cameras);
  loadRefs(imports, map["lights"], &ObjImporter::Filter::lights);

  for (QMap<QString, Import>::iterator it = imports.begin(); it != imports.end(); ++it) {
    if (importer.readFile(search(it->file), it->options)) {
      imported[it.key()] = importer.load(it->filter);
      m_node->children << imported[it.key()].node;
    }
  }

  foreach (P p, iterate(map["models"])) {
    ModelPtr model;
    if (!p.map["built-in"].toString().isEmpty()) {
      model = Model::createBuiltin(p.map["built-in"].toString());
    } else {
      model = clone(imported, p, &ObjImporter::Scene::models);
    }
    model->load(p.map);
    m_models[p.name] = model;
  }

  foreach (P p, iterate(map["textures"])) {
    TexturePtr t = clone(imported, p, &ObjImporter::Scene::textures, false);
    if (!t) {
      if (p.map.contains("file")) t.reset(new TextureFile(p.name));
      else t.reset(new Texture(p.name));
    }
    t->load(p.map);
    m_textures[p.name] = t;
  }

  foreach (P p, iterate(map["materials"])) {
    MaterialPtr m = clone(imported, p, &ObjImporter::Scene::materials);
    m->load(p.map);
    m_materials[p.name] = m;

    ProgramPtr prog;
    foreach (QString filename, p.map["fragment"].toStringList()) {
      if (!prog) prog.reset(new GLProgram(p.name));
      prog->addShader(search(filename), Shader::Fragment);
    }
    foreach (QString filename, p.map["vertex"].toStringList()) {
      if (!prog) prog.reset(new GLProgram(p.name));
      prog->addShader(search(filename), Shader::Vertex);
    }
    foreach (QString filename, p.map["geometry"].toStringList()) {
      if (!prog) prog.reset(new GLProgram(p.name));
      prog->addShader(search(filename), Shader::Geometry);
    }

    Log::debug("Shading model: %s", m->style.shading_model.toUtf8().data());
    /// @todo add a default shader if the material has shader hint
    ///       and prog is null

    if (prog) m->setProg(prog);

    QVariantMap tmp = p.map["textures"].toMap();
    for (QVariantMap::iterator it = tmp.begin(); it != tmp.end(); ++it)
      m->addTexture(it.key(), genTexture(it.value().toString()));
  }

  foreach (P p, iterate(map["objects"])) {
    ObjectPtr o = clone(imported, p, &ObjImporter::Scene::objects);
    o->load(p.map);
    m_objects[p.name] = o;

    QString model = p.map["model"].toString();
    if (m_models.contains(model)) o->setModel(m_models[model]);

    QVariantMap tmp = p.map["materials"].toMap();
    for (QVariantMap::iterator it = tmp.begin(); it != tmp.end(); ++it) {
      QString n = it.value().toString();
      if (m_materials.contains(n))
        o->setMaterial(it.key(), m_materials[n]);
    }

    if (p.map.contains("material"))
      o->setDefaultMaterial(m_materials.value(p.map["material"].toString()));
  }

  foreach (P p, iterate(map["lights"])) {
    LightPtr l = clone(imported, p, &ObjImporter::Scene::lights);
    l->load(p.map);
    m_lights[p.name] = l;
  }

  foreach (P p, iterate(map["cameras"])) {
    CameraPtr c = clone(imported, p, &ObjImporter::Scene::cameras);
    c->load(p.map);
    m_cameras[p.name] = c;
  }

  /// @todo animations

  foreach (QVariant item, map["render passes"].toList()) {
    QVariantMap map = item.toMap();
    RenderPassPtr pass(new RenderPass(map["name"].toString(), shared_from_this()));
    pass->load(map);
    m_render_passes.push_back(qMakePair(pass->name(), pass));
  }

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
