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

#ifndef SCENE_HPP
#define SCENE_HPP

#include "forward.hpp"
#include "shaderdb/metainfo.hpp"

#include <QString>
#include <QVariantMap>

/**
 * Scene is a data structure that stores all objects by name, and controls
 * the rendering. Scene also knows the actual viewport size and handles
 * the JSON loading. Currently the whole JSON project-file only contains
 * stuff that are actually only in Scene.
 */
class Scene : public QObject, public std::enable_shared_from_this<Scene> {
  Q_OBJECT

public:
  typedef QList<RenderPassPtr> RenderPasses;

  Scene();

  /// Set the viewport size
  void resize(int width, int height);

  /// Renders everything in this scene
  void render();

  /// Current viewport width
  /// @todo QRect?
  int width() const { return m_width; }
  /// Current viewport height
  int height() const { return m_height; }

  ProgramPtr shader(const QString& name) { return m_shaders[name]; }
  ObjectPtr object(const QString& name) { return m_objects[name]; }
  LightPtr light(const QString& name) { return m_lights[name]; }
  CameraPtr camera(const QString& name) { return m_cameras[name]; }
  TexturePtr texture(const QString& name);

  /// Finds all the shaders with given file name.
  QList<ShaderPtr> shadersByFilename(const QString& filename);

  /// Name -> shader mapping
  QMap<QString, ProgramPtr> shaders() { return m_shaders; }

  /// Name -> object mapping
  QMap<QString, ObjectPtr> objects() { return m_objects; }

  /// Name -> light mapping
  QMap<QString, LightPtr> lights() { return m_lights; }

  /// Name -> camera mapping
  QMap<QString, CameraPtr> cameras() { return m_cameras; }

  /// Ordered list of all render passes
  RenderPasses renderPasses() { return m_render_passes; }

  QVariantMap save() const;

  /// Load the scene from map
  void load(QVariantMap map);

  /// Returns the metainfo loaded from the project file
  MetaInfo & metainfo() { return m_metainfo; }

  /// @see root()
  void setRoot(QString root) { m_root = root; }
  /// The root directory for the scene, all relative filenames
  /// are relative to this directory
  QString root() const { return m_root; }

  /// Returns absolute file path for filename, or empty string if not found
  QString search(QString filename) const;

signals:
  void shaderListUpdated();
  void objectListUpdated();
  void lightListUpdated();
  void cameraListUpdated();

protected:
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
  QMap<QString, ProgramPtr> m_shaders;
  QMap<QString, TexturePtr> m_textures;

  MetaInfo m_metainfo;
  
  int m_width, m_height;

  QString m_root;
};

/// Convert array of three doubles to vector
QVector3D toVector(QVariant in);
/// Convert array of four doubles to color
QColor toColor(QVariant in);
QVariantList toList(QVector3D in);
QVariantList toList(QColor in);

#endif // SCENE_HPP
