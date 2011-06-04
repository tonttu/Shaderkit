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
#include "render_pass_properties.hpp"

// qjson
#include <parser.h>
#include <serializer.h>

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
  for (auto it = tmp.begin(); it != tmp.end(); ++it) {
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
  for (auto it = tmp.begin(); it != tmp.end(); ++it) {
    P p;
    p.name = it.key();
    p.map = it.value().toMap();
    out << p;
  }
  return out;
}

template <typename T>
std::shared_ptr<T> clone(QMap<QString, ObjImporter::Scene>& imported, const P& p,
                         QMap<QString, std::shared_ptr<T>> ObjImporter::Scene::*src,
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

} // namespace

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

Scene::Scene(QString filename)
  : m_width(-1), m_height(-1),
    m_filename(filename), m_node(new Node), m_picking(-1, -1),
    m_renderPassesChanged(false) {
  /// @todo remove this, this non-gui class shouldn't call gui stuff
  connect(this, SIGNAL(materialListUpdated(ScenePtr)),
          &MaterialProperties::instance(), SLOT(updateMaterialList(ScenePtr)));
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

  if (m_renderPassesChanged) {
    m_render_passes = RenderPassProperties::instance().list();
    m_renderPassesChanged = false;
    emit renderPassesListUpdated();
  }

  bool ui = false;
  foreach (RenderPassPtr p, m_render_passes) {
    if (!p->viewport()) continue;

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

  if (m_pickFunc) {
    if (m_picked.first && m_picked.second) {
      m_pickFunc(m_picked.first, m_picked.second);
    }
    if (m_pickOnce) {
      pick(-1.0f, -1.0f);
      m_picked = QPair<ObjectPtr, MeshPtr>();
    }
  }

  if (m_picked.first && m_picking.x() < 0) {
    m_picked = QPair<ObjectPtr, MeshPtr>();
  }

  MaterialProperties::instance().setActiveMaterials(state.usedMaterials());
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

void Scene::remove(TexturePtr t) {
  foreach (auto m, m_materials)
    m->removeTexture(t);

  QStringList tmp = m_textures.keys(t);
  foreach (QString name, tmp)
    m_textures.remove(name);
  if (!tmp.isEmpty()) emit textureListUpdated();
}

QList<ShaderPtr> Scene::shaders(const QString& res) {
  QList<ShaderPtr> ret;
  foreach (ProgramPtr p, materialPrograms())
    foreach (ShaderPtr s, p->shaders())
      if (s->res() == res) ret << s;
  return ret;
}

QList<ProgramPtr> Scene::materialPrograms() const {
  QList<ProgramPtr> programs;
  foreach (MaterialPtr m, m_materials)
    if (m->prog()) programs << m->prog();
  return programs;
}

QSet<QString> Scene::filenames() const {
  QSet<QString> names;
  QFileInfo fi;
  fi.setFile(m_filename);
  names << fi.absoluteFilePath();
  foreach (ProgramPtr p, materialPrograms()) {
    foreach (ShaderPtr s, p->shaders()) {
      fi.setFile(s->res());
      names << fi.absoluteFilePath();
    }
  }
  return names;
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
  foreach (RenderPassPtr p, m_render_passes)
    render_passes << p->save();
  if (!render_passes.isEmpty()) map["render passes"] = render_passes;

  return map;
}

RenderPassPtr Scene::findRenderer(TexturePtr tex) {
  foreach (RenderPassPtr rp, m_render_passes) {
    FBOPtr fbo = rp->fbo();
    if (fbo) {
      foreach (FBOImagePtr img, fbo->buffers())
        if (img == tex) return rp;
    }
  }
  return RenderPassPtr();
}

void Scene::load(QVariantMap map) {
  ObjImporter importer;
  QMap<QString, ObjImporter::Scene> imported;

  /// @todo some kind of incremental loading would be cool
  m_render_passes.clear();
  m_objects.clear();
  m_lights.clear();
  m_cameras.clear();
  m_textures.clear();
  m_materials.clear();
  m_models.clear();
  m_imports.clear();

  m_metainfo.load(map["lab"].toMap());

  QVariantMap tmp = map["import"].toMap();
  for (auto it = tmp.begin(); it != tmp.end(); ++it) {
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

  for (auto it = m_imports.begin(); it != m_imports.end(); ++it) {
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
    setMaterial(p.name, m);

    foreach (QString filename, p.map["fragment"].toStringList()) {
      m->prog(true)->addShader(search(filename), Shader::Fragment);
    }
    foreach (QString filename, p.map["vertex"].toStringList()) {
      m->prog(true)->addShader(search(filename), Shader::Vertex);
    }
    foreach (QString filename, p.map["geometry"].toStringList()) {
      m->prog(true)->addShader(search(filename), Shader::Geometry);
    }

    Log::info("Shading model: %s", m->style.shading_model.toUtf8().data());
    /// @todo add a default shader if the material has shader hint
    ///       and prog is null

    QVariantMap tmp = p.map["textures"].toMap();
    /// @todo is the texture name always unique and correct?
    for (auto it = tmp.begin(); it != tmp.end(); ++it)
      m->addTexture(it.key(), genTexture(it.value().toString()));
  }

  foreach (P p, iterate(map["objects"])) {
    ObjectPtr o = clone(imported, p, &ObjImporter::Scene::objects);
    o->load(p.map);
    m_objects[p.name] = o;

    QString model = p.map["model"].toString();
    if (m_models.contains(model)) o->setModel(m_models[model]);

    QVariantMap tmp = p.map["materials"].toMap();
    for (auto it = tmp.begin(); it != tmp.end(); ++it) {
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
    m_render_passes << pass;
  }

}

void Scene::merge(const ObjImporter::Scene& s) {
  m_node->children << s.node;
  if (!s.objects.isEmpty()) {
    m_objects.unite(s.objects);
    emit objectListUpdated();
  }
  if (!s.lights.isEmpty()) {
    m_lights.unite(s.lights);
    emit lightListUpdated();
  }
  if (!s.cameras.isEmpty()) {
    m_cameras.unite(s.cameras);
    emit cameraListUpdated();
  }
  if (!s.textures.isEmpty()) {
    m_textures.unite(s.textures);
    emit textureListUpdated();
  }
  if (!s.materials.isEmpty()) {
    foreach (MaterialPtr m, s.materials)
      m->setScene(shared_from_this());
    m_materials.unite(s.materials);
    emit materialListUpdated(shared_from_this());
  }
  m_models.unite(s.models);
  /// @todo animations
  /// @todo add a default shader if the material has shader hint
}

void Scene::renderPassesChanged() {
  m_renderPassesChanged = true;
}

void Scene::remove(MaterialPtr m) {
  for (auto it = m_materials.begin(); it != m_materials.end();) {
    if (*it == m) it = m_materials.erase(it);
    else ++it;
  }

  foreach (RenderPassPtr rp, m_render_passes)
    if (rp->defaultMaterial() == m) rp->setDefaultMaterial(MaterialPtr());

  foreach (ObjectPtr o, m_objects)
    o->remove(m);

  emit materialListUpdated(shared_from_this());
}

ScenePtr Scene::load(const QString& filename) {
  ScenePtr scene;

  QDir dir(filename);
  QString root;
  if (dir.cdUp())
    root = dir.canonicalPath();
  if (root.isEmpty())
    return scene;

  QJson::Parser parser;
  bool ok;
  QFile file(filename);
  QVariant data = parser.parse(&file, &ok);
  if (ok) {
    scene.reset(new Scene(filename));
    scene->setRoot(root);
    scene->load(data.toMap());
  }
  return scene;
}

void Scene::renameFile(QString from, QString to) {
  /// @todo Watcher::rename(from, to);
  ///       -> will put the renaming to queue and update stuff on next Watcher::update
  if (QFile::exists(from)) {
    Log::info("Renaming %s -> %s", from.toUtf8().data(), to.toUtf8().data());
    QFile::rename(from, to);
  }

  /// @todo handle project files, images and maybe some other assets
  foreach (ProgramPtr p, materialPrograms())
    foreach (ShaderPtr s, p->shaders())
      if (s->res() == from)
        s->setFilename(to);
}

bool Scene::save(const QString& filename) {
  QJson::Serializer serializer;
  QFile file(filename);
  // serializer.serialize(QVariant, QIODevice* io, bool* ok ) uses QDataStream
  // that isn't what we want.
  const QByteArray str = serializer.serialize(save());
  if (!str.isNull() && file.open(QIODevice::WriteOnly)) {
    file.write(str);
    m_filename = filename;
    return true;
  }
  return false;
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

void Scene::pick(float x, float y, bool once, PickFunc func) {
  m_picking = QPointF(x, y);
  m_pickOnce = once;
  m_pickFunc = func;
}

void Scene::setMaterial(QString name, MaterialPtr material) {
  m_materials[name] = material;
  material->setScene(shared_from_this());
  emit materialListUpdated(shared_from_this());
}

void Scene::addTexture(TexturePtr t) {
  QString name = Utils::uniqueName(t->name(), m_textures.keys());
  t->setName(name);
  m_textures[name] = t;
  emit textureListUpdated();
}
