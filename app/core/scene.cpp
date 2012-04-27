/**
 * Copyright 2010-2012 Riku Palomäki.
 * This file is part of Shaderkit, http://www.shaderkit.org/.
 *
 * Shaderkit is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License Version 3 as
 * published by the Free Software Foundation.
 *
 * Shaderkit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Shaderkit.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "core/scene.hpp"
#include "core/renderpass.hpp"
#include "core/object3d.hpp"
#include "core/material.hpp"
#include "core/obj_importer.hpp"
#include "core/model.hpp"
#include "core/utils.hpp"
#include "core/resource_locator.hpp"

#include "gl/state.hpp"
#include "gl/light.hpp"
#include "gl/camera.hpp"
#include "gl/program.hpp"
#include "gl/shader.hpp"
#include "gl/texture.hpp"
#include "gl/opengl.hpp"

/// @todo these shouldn't be here
#include "gui/properties.hpp"
#include "gui/render_pass_properties.hpp"

#include "shaderdb/shaderdb.hpp"

// qjson
#include <parser.h>
#include <serializer.h>

#include <QVariantMap>
#include <QFileInfo>
#include <QDir>

#include <cassert>

namespace {
using namespace Shaderkit;

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

template <typename T>
class AutoVar {
public:
  AutoVar(T & var, T val) : m_var(var), m_orig(var) {
    m_var = val;
  }
  ~AutoVar() {
    m_var = m_orig;
  }

private:
  T & m_var;
  T m_orig;
};


} // namespace

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

namespace Shaderkit {

Scene::Scene(/*QString filename*/)
  : m_width(-1), m_height(-1),
    /*m_filename(filename), */m_node(new Node), m_picking(-1, -1),
    m_saver(*this),
    m_automaticSaving(false),
    //m_history(*this, filename),
    m_state(New),
    m_changed(false), m_loading(false), m_lastTime(0) {
  /// @todo remove this, this non-gui class shouldn't call gui stuff
  connect(this, SIGNAL(materialListUpdated(ScenePtr)),
          &MaterialProperties::instance(), SLOT(updateMaterialList(ScenePtr)));
  m_time.start();

  connect(this, SIGNAL(changed(bool)), &m_saver, SLOT(sceneChanged()));
  connect(this, SIGNAL(stateChanged()), &m_saver, SLOT(stateChanged()));
}

void Scene::resize(int width, int height) {
  if (width == m_width && height == m_height) return;

  m_width = width;
  m_height = height;
}

void Scene::render(RenderOptions& opts_in) {
  float t = m_time.elapsed()/1000.0f;
  float dt = t - m_lastTime;
  State state(*this, t, dt);
  state.setSelection(m_selection);
  RenderOptions opts = opts_in;
  opts_in.grid_animation += dt * 1.0f;

  opts.grid = true;

  CameraPtr defcam = camera();

  /// @todo how stupid name is "Normal" for render pass. Extremely stupid. Fix it.
  RenderPassPtr ui_pass = selectedRenderPass(RenderPass::Normal);

  bool ui = false;
  foreach (RenderPassPtr p, m_render_passes) {
    if (!p->view()) continue;

    opts.ui = ui_pass == p;

    bool pick = false;
    if (m_picking.x() >= 0 && p->view() == defcam) {
      pick = true;
      state.setPicking(QPoint(m_picking.x()*p->width(), m_picking.y()*p->height()));
    }

    p->render(state, opts);

    if (pick) {
      m_picked = state.picked();
      state.disablePicking();
    }
  }

  if (m_pickFunc) {
    if ((m_picked.first && m_picked.second) || m_pickEmpty) {
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
  m_lastTime = t;
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

QList<ShaderPtr> Scene::shaders(const QString& filename) {
  QSet<ShaderPtr> ret;
  foreach (ProgramPtr p, materialPrograms())
    foreach (ShaderPtr s, p->shaders())
      if (s->filename() == filename) ret << s;
  return ret.toList();
}

QSet<ShaderPtr> Scene::shaders() {
  QSet<ShaderPtr> ret;
  foreach (ProgramPtr p, materialPrograms())
    ret += p->shaders();
  return ret;
}

QList<ProgramPtr> Scene::materialPrograms() const {
  QList<ProgramPtr> programs;
  foreach (MaterialPtr m, m_materials)
    if (m->prog()) programs << m->prog();
  return programs;
}

QList<Scene::FileInfo> Scene::files() const {
  QList<FileInfo> ret;
  ret << FileInfo(filename(), icon(), "project");
  foreach (ProgramPtr p, materialPrograms())
    foreach (ShaderPtr s, p->shaders())
      ret << FileInfo(s->filename(), s->icon(), "shader");

  foreach (auto i, m_imports)
    ret << FileInfo(i.filename(), i.icon(), "import");

/*  foreach (auto m, m_models) {
    fi.setFile(m->res());
    if(!fi.absoluteFilePath().isEmpty())
      ret << FileInfo(fi.absoluteFilePath(), m->icon(), "model");
  }*/

  foreach (auto t, m_textures) {
    TextureFile* tf = dynamic_cast<TextureFile*>(t.get());
    if (!tf) continue;
    if(!tf->filename().isEmpty())
      ret << FileInfo(tf->filename(), tf->icon(), "texture");
  }

  return ret;
}

QSet<QString> Scene::filenames() const {
  QSet<QString> set;
  foreach (auto tmp, files())
    set << tmp.name;
  return set;
}

QVariantMap Scene::toMap() const {
  QVariantMap map, tmp;

  foreach (QString name, m_models.keys())
    tmp[name] = m_models[name]->toMap();
  if (!tmp.isEmpty()) map["models"] = tmp, tmp.clear();

  foreach (QString name, m_textures.keys())
    tmp[name] = m_textures[name]->toMap();
  if (!tmp.isEmpty()) map["textures"] = tmp, tmp.clear();

  foreach (QString name, m_materials.keys())
    tmp[name] = m_materials[name]->toMap();
  if (!tmp.isEmpty()) map["materials"] = tmp, tmp.clear();

  foreach (QString name, m_objects.keys())
    tmp[name] = m_objects[name]->toMap();
  if (!tmp.isEmpty()) map["objects"] = tmp, tmp.clear();

  foreach (QString name, m_lights.keys())
    tmp[name] = m_lights[name]->toMap();
  if (!tmp.isEmpty()) map["lights"] = tmp, tmp.clear();

  foreach (QString name, m_cameras.keys())
    tmp[name] = m_cameras[name]->toMap();
  if (!tmp.isEmpty()) map["cameras"] = tmp, tmp.clear();

  /*foreach (QString name, m_animations.keys())
    tmp[name] = m_animations[name]->toMap();
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
    i["file"] = m_imports[name].rawFilename();

    QStringList flags;
    foreach (QString flag, m_imports[name].options.keys())
      if (m_imports[name].options[flag]) flags << flag;

    if (!flags.isEmpty()) i["flags"] = flags;
    tmp[name] = i;
  }
  if (!tmp.isEmpty()) map["import"] = tmp, tmp.clear();

  tmp = m_metainfo.toMap();
  if (!tmp.isEmpty()) map["shaderkit"] = tmp, tmp.clear();

  QVariantList render_passes;
  foreach (RenderPassPtr p, m_render_passes)
    render_passes << p->toMap();
  if (!render_passes.isEmpty()) map["render passes"] = render_passes;

  return map;
}

struct FileInfo {
  QString name;
  QIcon* icon;
  QString type;
  QString status;
};

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

void Scene::load(const QString& filename, SceneState state, QVariantMap map) {
  AutoVar<bool> loading(m_loading, true);
  setFilename(filename);
  ResourceLocator::Path path("scene", root());
  m_state = state;
  ObjImporter importer;
  QMap<QString, ObjImporter::Scene> imported;

  /** @todo node graph
  "scene" : [
   ["object", "box.1",
     ["object", "box.2"]
   ],
   ["object", "dwarf",
     ["camera", "cam.1", "HAND.L"]
   ],
   ["light", "light1"]
  ]*/

  /// @todo some kind of incremental loading would be cool
  m_selection.clear();
  m_render_passes.clear();
  m_objects.clear();
  m_lights.clear();
  m_cameras.clear();
  m_textures.clear();
  m_materials.clear();
  m_models.clear();
  m_imports.clear();

  m_metainfo.load(map["shaderkit"].toMap());

  QVariantMap tmp = map["import"].toMap();
  for (auto it = tmp.begin(); it != tmp.end(); ++it) {
    Import& im = m_imports[it.key()];
    QVariantMap item = it->toMap();
    im.setFilename(item["file"].toString());
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
    if (importer.readFile(it->filename(), it->options)) {
      imported[it.key()] = importer.load(it->filter);
      m_node->children << imported[it.key()].node;
    }
  }

  foreach (P p, iterate(map["models"])) {
    ModelPtr model;
    if (!p.map["built-in"].toString().isEmpty()) {
      model = Model::createBuiltin(p.name, p.map["built-in"].toString(), p.map);
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
    m->load(*this, p.map);
    setMaterial(p.name, m);
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
        o->setMaterialForMesh(it.key(), m_materials[n]);
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

  emit stateChanged();
}

void Scene::merge(const Import& import, const ObjImporter::Scene& s) {
  QFileInfo fi(import.filename());
  QString name = Utils::uniqueName(fi.baseName(), m_imports.keys(), "import");

  m_imports[name] = import;

  m_node->children << s.node;

  /// @todo name conflicts?
  /// set the ref thingy

  foreach (auto o, s.objects) o->setRef(name, o->name()), add(o);

  foreach (auto l, s.lights) l->setRef(name, l->name()), add(l);

  foreach (auto c, s.cameras) c->setRef(name, c->name()), add(c);

  foreach (auto t, s.textures) t->setRef(name, t->name()), add(t);

  foreach (auto m, s.materials) m->setRef(name, m->name()), add(m);

  foreach (auto m, s.models) m->setRef(name, m->name()), add(m);

  /// @todo animations
  /// @todo add a default shader if the material has shader hint

  RenderPassPtr pass;
  foreach (RenderPassPtr p, m_render_passes)
    if (p->view() && p->type() == RenderPass::Normal)
      pass = p;

  if (!m_objects.isEmpty()) {
    if (!pass) {
      pass.reset(new RenderPass("Default", shared_from_this()));
      add(pass);
    }

    if (!pass->view() && s.cameras.isEmpty()) {
      CameraPtr camera(new Camera("Default"));

      /// @todo use bounding box to calculate optimal location for the camera
      camera->setTarget(Eigen::Vector3f(0, 30, 0));
      camera->setLocation(Eigen::Vector3f(60, 45, -60));

      add(camera);
      pass->setView(camera);
    }

    if (pass->lights().isEmpty() && s.lights.isEmpty()) {
      LightPtr light(new Light("back"));
      light->setAmbient(Color(0, 0, 0));
      light->setDiffuse(Color(0.8f, 0.8f, 1.0f));
      light->setSpecular(Color(0.8f, 0.8f, 1.0f));
      light->setDirection(Eigen::Vector3f(0, 0.5f, 1.0f));

      LightPtr light1(new Light("light 1"));
      light1->setAmbient(Color(0.2f, 0.2f, 0.2f));
      light1->setDiffuse(Color(1.0f, 0.8f, 1.0f));
      light1->setSpecular(Color(1.0f, 0.8f, 1.0f));
      light1->setDirection(Eigen::Vector3f(0.3f, 0.4f, -1.0f));

      LightPtr light2(new Light("light 2"));
      light2->setAmbient(Color(0, 0, 0));
      light2->setDiffuse(Color(1.0f, 1.0f, 0.8f));
      light2->setSpecular(Color(1.0f, 1.0f, 0.8f));
      light2->setDirection(Eigen::Vector3f(-0.2f, 0.2f, -1.0f));

      add(light);
      add(light1);
      add(light2);

      pass->add(light);
      pass->add(light1);
      pass->add(light2);
    }
  }

  if (pass) {
    if (!s.cameras.isEmpty())
      pass->setView(s.cameras.values().first());

    foreach (LightPtr light, s.lights)
      pass->add(light);

    foreach (ObjectPtr obj, s.objects)
      pass->add(obj);
  }

  /// @todo add all stuff to default render pass
}

void Scene::remove(MaterialPtr m) {
  for (auto it = m_materials.begin(); it != m_materials.end();) {
    if (*it == m) it = m_materials.erase(it);
    else ++it;
  }

  foreach (RenderPassPtr rp, m_render_passes)
    if (rp->defaultMaterial() == m) rp->setDefaultMaterial(MaterialPtr());

  foreach (ObjectPtr o, m_objects) {
    o->remove(m);
    m_selection.removeAll(o);
  }

  emit materialListUpdated(shared_from_this());
}

ScenePtr Scene::load(const QString& filename, SceneState state) {
  ScenePtr scene;

  QJson::Parser parser;
  bool ok;
  QFile file(filename);
  QVariant data = parser.parse(&file, &ok);
  if (ok) {
    scene.reset(new Scene());
    scene->load(filename, state, data.toMap());
  }
  return scene;
}

/*void Scene::renameFile(QString from, QString to) {
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
}*/

bool Scene::fileRename(const QString& from, const QString& to, bool keep_old_file) {
  /// @todo Watcher::rename(from, to);
  ///       -> will put the renaming to queue and update stuff on next Watcher::update
  if (!QFile::exists(from)) {
    Log::warn("fileRename: %s -> %s, the src file doesn't exist", from.toUtf8().data(), to.toUtf8().data());
    // there wasn't exactly an error
    return true;
  }

  if (QFile::exists(to)) {
    Log::error("fileRename: %s -> %s, the target file already exists", from.toUtf8().data(), to.toUtf8().data());
    return false;
  }

  Log::info("Renaming %s -> %s", from.toUtf8().data(), to.toUtf8().data());
  if (keep_old_file) {
    return QFile::copy(from, to);
  } else {
    /// @todo this (probably?) doesn't work across filesystems!
    if (QFile::rename(from, to)) return true;
    if (QFile::copy(from, to)) {
      if (QFile::remove(from)) return true;
      Log::warn("fileRename: %s -> %s, move failed, copied instead and failed to remove the src file",
                from.toUtf8().data(), to.toUtf8().data());
    }
    return false;
  }
}

bool Scene::renameShaderFile(const QString& from, const QString& to, bool keep_old_file) {
  if (!fileRename(from, to, keep_old_file))
    return false;

  foreach (ShaderPtr shader, shaders(from))
    shader->setFilename(to);

  return true;
}

bool Scene::renameImportFile(const QString& from, const QString& to, bool keep_old_file) {
  if (!fileRename(from, to, keep_old_file))
    return false;

  for (auto it = m_imports.begin(); it != m_imports.end(); ++it) {
    Import& i = *it;
    if (i.filename() == from)
      i.setFilename(to);
  }
  return true;
}

void Scene::setAutomaticSaving(bool state) {
  m_automaticSaving = state;
  emit stateChanged();
}

bool Scene::renameTextureFile(const QString& from, const QString& to, bool keep_old_file) {
  if (!fileRename(from, to, keep_old_file))
    return false;

  for (auto it = m_textures.begin(); it != m_textures.end(); ++it) {
    TextureFile* tf = dynamic_cast<TextureFile*>(it->get());
    if (tf && tf->filename() == from)
      tf->setFilename(to);
  }
  return true;
}

void Scene::setState(SceneState state) {
  if (m_state == state) return;
  m_state = state;
  emit stateChanged();
}

void Scene::syncHistory() {
  //m_history.sync();
}

void Scene::add(RenderPassPtr pass) {
  QList<QString> names;
  foreach (RenderPassPtr rp, m_render_passes) names << rp->name();

  pass->setName(Utils::uniqueName(pass->name(), names, "Untitled"));

  m_render_passes << pass;
  emit renderPassesListUpdated(m_render_passes);
}

void Scene::remove(RenderPassPtr pass) {
  m_render_passes.removeAll(pass);
  emit renderPassesListUpdated(m_render_passes);
}

void Scene::setRenderPasses(RenderPasses passes) {
  m_render_passes = passes;
  emit renderPassesListUpdated(m_render_passes);
}

void Scene::add(CameraPtr camera) {
  camera->setName(Utils::uniqueName(camera->name(), m_cameras.keys(), "Camera"));
  m_cameras[camera->name()] = camera;
  emit cameraListUpdated();
}

void Scene::remove(CameraPtr camera) {
  foreach (const QString& name, m_cameras.keys(camera))
    m_cameras.remove(name);

  foreach (RenderPassPtr rp, m_render_passes)
    if (rp->view() == camera)
      rp->setView(CameraPtr());

  emit cameraListUpdated();
}

void Scene::add(LightPtr light) {
  light->setName(Utils::uniqueName(light->name(), m_lights.keys(), "Light"));
  m_lights[light->name()] = light;

  emit lightListUpdated();
}

void Scene::remove(LightPtr light) {
  foreach (const QString& name, m_lights.keys(light))
    m_lights.remove(name);

  foreach (RenderPassPtr rp, m_render_passes) {
    RenderPass::Lights lights = rp->lights();
    if (!lights.contains(light)) continue;
    lights.remove(light);
    rp->setLights(lights);
  }
  emit lightListUpdated();
}

void Scene::add(MaterialPtr mat) {
  mat->setName(Utils::uniqueName(mat->name(), m_materials.keys(), "Material"));
  m_materials[mat->name()] = mat;
  mat->setScene(shared_from_this());

  emit materialListUpdated(shared_from_this());
}

void Scene::add(ObjectPtr obj) {
  obj->setName(Utils::uniqueName(obj->name(), m_objects.keys(), "Object"));
  m_objects[obj->name()] = obj;

  emit objectListUpdated();
}

void Scene::remove(ObjectPtr obj) {
  foreach (const QString& name, m_objects.keys(obj))
    m_selection.removeAll(m_objects.take(name));

  foreach (RenderPassPtr rp, m_render_passes)
    rp->remove(obj);

  ModelPtr model = obj->model();

  bool used = false;
  foreach (ObjectPtr o, m_objects)
    if (o->model() == model) {
      used = true;
      break;
    }

  if (!used)
    remove(model);

  /// @todo gizmo?

  emit objectListUpdated();
}

void Scene::add(ModelPtr model) {
  model->setName(Utils::uniqueName(model->name(), m_models.keys(), "Model"));
  m_models[model->name()] = model;
}

void Scene::remove(ModelPtr model) {
  foreach (const QString& name, m_models.keys(model))
    m_models.remove(name);

  foreach (ObjectPtr o, m_objects)
    if (o->model() == model)
      remove(o);
}

RenderPassPtr Scene::selectedRenderPass(RenderPass::Type filter) const {
  /// @todo implement render pass selection
  foreach (RenderPassPtr p, m_render_passes) {
    if (!p->view() || p->type() != filter) continue;

    return p;
  }
  return RenderPassPtr();
}

QIcon Scene::icon() const {
  /// @todo implement something fun
  return QIcon();
}

void Scene::changedSlot() {
  if (m_loading) return;
  bool prev = m_changed;
  m_changed = true;
  //m_history.changed();
  emit changed(prev);
}

/// @todo what state use after this?
bool Scene::save(const QString& filename) {
  QJson::Serializer serializer;
  serializer.allowSpecialNumbers(true);
  serializer.setIndentMode(QJson::IndentFull);
  QFile file(filename);
  // serializer.serialize(QVariant, QIODevice* io, bool* ok ) uses QDataStream
  // that isn't what we want.
  const QByteArray str = serializer.serialize(toMap());
  if (!str.isNull() && file.open(QIODevice::WriteOnly)) {
    file.write(str);
    setFilename(filename);
    m_changed = false;
    emit saved();
    return true;
  }
  return false;
}

bool Scene::save(const QVariantMap& map) {
  QFile file(filename());

  // We should use save(string) -version with read-only files!
  assert(m_state != ReadOnly);

  if (m_state == New) {
    assert(file.fileName().isEmpty());
    if (ShaderDB::openNewLimbo(file, metainfo().name)) {
      setFilename(file.fileName());
      m_state = Limbo;
    } else {
      return false;
    }
  } else {
    assert(!file.fileName().isEmpty());
    if (!file.open(QIODevice::WriteOnly))
      return false;
  }

  QJson::Serializer serializer;
  serializer.allowSpecialNumbers(true);
  serializer.setIndentMode(QJson::IndentFull);
  const QByteArray str = serializer.serialize(map);
  if (!str.isNull()) {
    file.write(str);
    m_changed = false;
    emit saved();
    return true;
  }
  return false;
}

QString Scene::root() const {
  QFileInfo fi(filename());
  return fi.absolutePath();
}

void Scene::setFilename(const QString& filename) {
  FileResource::setFilename(filename);
  //m_history.setSceneFilename(filename);
}

CameraPtr Scene::camera() {
  foreach (CameraPtr c, m_cameras)
    if (c->type() == Camera::Perspective) return c;
  return CameraPtr();
}

void Scene::pick(float x, float y, bool once, bool pick_empty, PickFunc func) {
  m_picking = QPointF(x, y);
  m_pickOnce = once;
  m_pickEmpty = pick_empty;
  m_pickFunc = func;
}

void Scene::setMaterial(QString name, MaterialPtr material) {
  m_materials[name] = material;
  material->setScene(shared_from_this());
  emit materialListUpdated(shared_from_this());
}

void Scene::add(TexturePtr t) {
  t->setName(Utils::uniqueName(t->name(), m_textures.keys(), "Texture"));
  m_textures[t->name()] = t;
  emit textureListUpdated();
}

} // namespace Shaderkit
