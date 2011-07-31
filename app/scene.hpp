/**
 * Copyright 2010,2011 Riku Palomäki.
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

#ifndef SCENE_HPP
#define SCENE_HPP

#include "forward.hpp"
#include "shaderdb/metainfo.hpp"
#include "obj_importer.hpp"
#include "history.hpp"
#include "renderpass.hpp"

#include "Eigen/Geometry"

#include <QString>
#include <QVariantMap>
#include <QTime>
#include <QPoint>

#include <functional>

/**
 * Scene is a data structure that stores all objects by name, and controls
 * the rendering. Scene also knows the actual viewport size and handles
 * the JSON loading. Currently the whole JSON project-file only contains
 * stuff that are actually only in Scene.
 */
class Scene : public QObject, public std::enable_shared_from_this<Scene> {
  Q_OBJECT

public:
  typedef std::function<void(ObjectPtr, MeshPtr)> PickFunc;
  typedef QList<RenderPassPtr> RenderPasses;
  struct Import {
    ObjImporter::Filter filter;
    ObjImporter::Options options;
    QString file;
  };

  Scene(QString filename);

  /// Set the viewport size
  void resize(int width, int height);

  /// Renders everything in this scene
  void render(RenderOptions& opts);

  /// Current viewport width
  /// @todo QRect?
  int width() const { return m_width; }
  /// Current viewport height
  int height() const { return m_height; }

  //ProgramPtr shader(const QString& name) { return m_shaders[name]; }
  ObjectPtr object(const QString& name) const { return m_objects.value(name); }
  LightPtr light(const QString& name) const { return m_lights.value(name); }
  CameraPtr camera(const QString& name) const { return m_cameras.value(name); }
  MaterialPtr material(const QString& name) const { return m_materials.value(name); }
  TexturePtr genTexture(const QString& name);

  void remove(TexturePtr t);

  /// Finds all the shaders with given resource name
  QList<ShaderPtr> shaders(const QString& res);

  /// Name -> shader mapping
  QMap<QString, MaterialPtr> materials() { return m_materials; }

  /// Name -> object mapping
  QMap<QString, ObjectPtr> objects() { return m_objects; }

  /// Name -> light mapping
  QMap<QString, LightPtr> lights() { return m_lights; }

  /// Name -> camera mapping
  QMap<QString, CameraPtr> cameras() { return m_cameras; }

  /// Name -> texture mapping
  QMap<QString, TexturePtr> textures() { return m_textures; }

  /// Ordered list of all render passes
  RenderPasses renderPasses() { return m_render_passes; }

  QList<ProgramPtr> materialPrograms() const;

  QSet<QString> filenames() const;

  QVariantMap save() const;

  /// Finds the render pass that owns the fbo that uses the given texture as a render buffer
  RenderPassPtr findRenderer(TexturePtr tex);

  /// Load the scene from map
  void load(QVariantMap map);

  /// Saves the scene to JSON file.
  /// @returns true if saving succeeds
  bool save(const QString& filename);
  bool save(const QVariantMap& map);

  /// Returns the metainfo loaded from the project file
  MetaInfo & metainfo() { return m_metainfo; }

  /// @see root()
  void setRoot(QString root) { m_root = root; }
  /// The root directory for the scene, all relative filenames
  /// are relative to this directory
  QString root() const { return m_root; }

  NodePtr node() { return m_node; }

  /// Returns absolute file path for filename, or empty string if not found
  QString search(QString filename) const;

  void setFilename(QString filename);
  QString filename() const { return m_filename; }

  CameraPtr camera();

  void pick(float x, float y, bool once = true, PickFunc func = PickFunc());

  void setMaterial(QString name, MaterialPtr material);
  void add(TexturePtr t);

  void merge(const Import& import, const ObjImporter::Scene& s);

  void remove(MaterialPtr m);

  /// Loads a scene from JSON file.
  static ScenePtr load(const QString& filename);

  void renameFile(QString from, QString to);

  void setAutomaticSaving(bool state) { m_automaticSaving = state; }
  bool automaticSaving() const { return m_automaticSaving; }

  bool isChanged() const { return m_changed; }
  void syncHistory();

  void add(RenderPassPtr pass);
  void remove(RenderPassPtr pass);
  void setRenderPasses(RenderPasses passes);

  void add(CameraPtr camera);
  void remove(CameraPtr camera);

  void add(LightPtr light);
  void remove(LightPtr light);

  void add(MaterialPtr mat);

  void add(ObjectPtr obj);

  void add(ModelPtr model);

  void setSelection(QList<ObjectPtr> objects) { m_selection = objects; }
  QList<ObjectPtr> selection() const { return m_selection; }

  RenderPassPtr selectedRenderPass(RenderPass::Type filter) const;

signals:
  void shaderListUpdated();
  void objectListUpdated();
  void lightListUpdated();
  void cameraListUpdated();
  void textureListUpdated();
  void materialListUpdated(ScenePtr);
  void renderPassesListUpdated(QList<RenderPassPtr>);

  void changed();
  void saved();

protected slots:
  void changedSlot();

protected:
  void createDefaults();

  QMap<QString, Import> m_imports;

  /**
   * RenderPasses is defined as an ordered list of passes, while actually the
   * render passes form a directed dependency graph (without loops).
   * @todo The JSON structure should be treated as a set of Render Passes, and
   *       the actual order should be calculated when loading it.
   */
  RenderPasses m_render_passes;

  QMap<QString, ObjectPtr> m_objects;
  QMap<QString, LightPtr> m_lights;
  QMap<QString, CameraPtr> m_cameras;
  QMap<QString, TexturePtr> m_textures;
  QMap<QString, MaterialPtr> m_materials;
  QMap<QString, ModelPtr> m_models;

  MetaInfo m_metainfo;
  
  int m_width, m_height;

  QTime m_time;

  QString m_filename;
  QString m_root;

  NodePtr m_node;

  QPair<ObjectPtr, MeshPtr> m_picked;
  QPointF m_picking;
  bool m_pickOnce;
  PickFunc m_pickFunc;

  bool m_automaticSaving;
  History m_history;

  QList<ObjectPtr> m_selection;

  bool m_changed;

  float m_lastTime;
};

/// Convert array of three doubles to vector
QVector3D toVector(QVariant in);
Eigen::Vector3f toVector3(QVariant in);
/// Convert array of four doubles to color
QColor toColor(QVariant in);
QVariantList toList(QVector3D in);
QVariantList toList(Eigen::Vector3f in);
QVariantList toList(QColor in);

#endif // SCENE_HPP
