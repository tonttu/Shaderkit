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
#include "properties.hpp"

#include <QVariantMap>

#include <cassert>

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

void loadRefs(QMap<QString, Scene::Import>& imports, QVariant src,
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
          Log::info("Importing first object (%s)", t->name().toUtf8().data());
          Ptr t2 = t->clone();
          t2->setName(p.name);
          return t2;
        }
      Log::error("Couldn't find any objects for ref %s", p.name.toUtf8().data());
    } else {
      Ptr t = (imported[ref[0]].*src).value(ref[1]);
      if (t) {
        Log::info("Importing %s", t->name().toUtf8().data());
        Ptr t2 = t->clone();
        t2->setName(p.name);
        return t2;
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

Scene::Scene(QString filename)
  : m_width(-1), m_height(-1),
    m_filename(filename), m_node(new Node), m_picking(-1, -1) {
  m_time.start();
}

void Scene::resize(int width, int height) {
  if (width == m_width && height == m_height) return;

  m_width = width;
  m_height = height;
}

void Scene::render() {
  State state(m_time.elapsed()/1000.0f);
  RenderOptions opts;
  opts.grid = true;
  opts.ui = false;

  CameraPtr defcam = camera();

  bool ui = false;
  for (RenderPasses::iterator it = m_render_passes.begin(); it != m_render_passes.end(); ++it) {
    RenderPassPtr p = it->second;
    opts.ui = !ui && p->type() == RenderPass::Normal;

    bool pick = false;
    if (m_picking.x() >= 0 && p->viewport() == defcam) {
      pick = true;
      state.setPicking(QPoint(m_picking.x()*p->width(), m_picking.y()*p->height()));
    }

    p->render(state, opts);

    if (pick) {
      m_picked = state.picked();
      state.disablePicking();
    }

    ui = opts.ui || ui;
  }

  if (m_material_assign) {
    if (m_picked.first && m_picked.second) {
      m_picked.first->setMaterial(m_picked.second->name, m_material_assign);
    }
    setPickDisplay(-1.0f, -1.0f);
    m_picked = QPair<ObjectPtr, MeshPtr>();
    m_material_assign.reset();
  }

  if (m_picked.first && m_picking.x() < 0) {
    m_picked = QPair<ObjectPtr, MeshPtr>();
    m_material_assign.reset();
  }

  MaterialProperties::instance().setMaterials(state.usedMaterials());
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
  foreach (ProgramPtr p, m_programs)
    foreach (ShaderPtr s, p->shaders())
      if (s->filename() == filename) res << s;
  return res;
}

QVariantMap Scene::save() const {
  QVariantMap map, tmp;

  foreach (QString name, m_models.keys())
    tmp[name] = m_models[name]->save();
  if (!tmp.isEmpty()) map["models"] = tmp, tmp.clear();

  foreach (QString name, m_textures.keys())
    tmp[name] = m_textures[name]->save();
  if (!tmp.isEmpty()) map["textures"] = tmp, tmp.clear();

  foreach (QString name, m_materials.keys()) {
    MaterialPtr m = m_materials[name];
    QVariantMap t = m->save();
    if (m->prog()) m->prog()->save(t, m_root);
    tmp[name] = t;
  }
  if (!tmp.isEmpty()) map["materials"] = tmp, tmp.clear();

  foreach (QString name, m_objects.keys())
    tmp[name] = m_objects[name]->save();
  if (!tmp.isEmpty()) map["objects"] = tmp, tmp.clear();

  foreach (QString name, m_lights.keys())
    tmp[name] = m_lights[name]->save();
  if (!tmp.isEmpty()) map["lights"] = tmp, tmp.clear();

  foreach (QString name, m_cameras.keys())
    tmp[name] = m_cameras[name]->save();
  if (!tmp.isEmpty()) map["cameras"] = tmp, tmp.clear();

  /*foreach (QString name, m_animations.keys())
    tmp[name] = m_animations[name]->save();
  if (!tmp.isEmpty()) map["animations"] = tmp, tmp.clear();*/

  QSet<QString> imports;
  foreach (QVariant v, map) {
    foreach (QVariant v2, v.toMap()) {
      QStringList lst = v2.toMap().value("ref").toStringList();
      if (!lst.isEmpty()) imports << lst[0];
    }
  }

  foreach (QString name, imports) {
    assert(m_imports.contains(name));
    QVariantMap i;
    i["file"] = m_imports[name].file;

    QStringList flags;
    foreach (QString flag, m_imports[name].options.keys())
      if (m_imports[name].options[flag]) flags << flag;

    if (!flags.isEmpty()) i["flags"] = flags;
    tmp[name] = i;
  }
  if (!tmp.isEmpty()) map["import"] = tmp, tmp.clear();

  tmp = m_metainfo.save();
  if (!tmp.isEmpty()) map["lab"] = tmp, tmp.clear();

  QVariantList render_passes;
  foreach (RenderPasses::value_type p, m_render_passes)
    render_passes << p.second->save();
  if (!render_passes.isEmpty()) map["render passes"] = render_passes;

  return map;
}

void Scene::load(QVariantMap map) {
  ObjImporter importer;
  QMap<QString, ObjImporter::Scene> imported;

  /// @todo some kind of incremental loading would be cool
  m_render_passes.clear();
  m_objects.clear();
  m_lights.clear();
  m_cameras.clear();
  m_programs.clear();
  m_textures.clear();
  m_materials.clear();
  m_models.clear();
  m_imports.clear();

  m_metainfo.load(map["lab"].toMap());

  QVariantMap tmp = map["import"].toMap();
  for (QVariantMap::iterator it = tmp.begin(); it != tmp.end(); ++it) {
    Import& im = m_imports[it.key()];
    QVariantMap item = it->toMap();
    im.file = item["file"].toString();
    foreach (QString flag, item["flags"].toStringList())
      im.options[flag] = true;
    /// @todo m_imports = stuff
  }

  /// @todo should this be objects instead of models?
  loadRefs(m_imports, map["models"], &ObjImporter::Filter::models);
  loadRefs(m_imports, map["textures"], &ObjImporter::Filter::textures);
  loadRefs(m_imports, map["materials"], &ObjImporter::Filter::materials);
  loadRefs(m_imports, map["animations"], &ObjImporter::Filter::animations);
  loadRefs(m_imports, map["camera"], &ObjImporter::Filter::cameras);
  loadRefs(m_imports, map["lights"], &ObjImporter::Filter::lights);

  for (QMap<QString, Import>::iterator it = m_imports.begin(); it != m_imports.end(); ++it) {
    if (importer.readFile(search(it->file), it->options)) {
      imported[it.key()] = importer.load(it->filter);
      m_node->children << imported[it.key()].node;
    }
  }

  foreach (P p, iterate(map["models"])) {
    ModelPtr model;
    if (!p.map["built-in"].toString().isEmpty()) {
      model = Model::createBuiltin(p.name, p.map["built-in"].toString());
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

    Log::info("Shading model: %s", m->style.shading_model.toUtf8().data());
    /// @todo add a default shader if the material has shader hint
    ///       and prog is null

    if (prog) {
      m->setProg(prog);
      m_programs[p.name] = prog;
    }

    QVariantMap tmp = p.map["textures"].toMap();
    /// @todo is the texture name always unique and correct?
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

CameraPtr Scene::camera() {
  foreach (CameraPtr c, m_cameras)
    if (c->type() == Camera::Perspective) return c;
  return CameraPtr();
}

void Scene::setPickDisplay(float x, float y) {
  m_picking = QPointF(x, y);
}

void Scene::setMaterial(float x, float y, QString material) {
  if (m_materials.contains(material)) {
    setPickDisplay(x, y);
    m_material_assign = m_materials[material];
  } else setPickDisplay(-1.0f, -1.0f);
}
