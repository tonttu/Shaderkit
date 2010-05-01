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

#ifndef SCENE_HPP
#define SCENE_HPP

#include "forward.hpp"

#include <QString>
#include <QVariantMap>

#include <boost/enable_shared_from_this.hpp>

#include <map>
#include <vector>

/**
 * Scene is a data structure that stores all objects by name, and controls
 * the rendering. Scene also knows the actual viewport size and handles
 * the JSON loading. Currently the whole JSON project-file only contains
 * stuff that are actually only in Scene.
 */
class Scene : public boost::enable_shared_from_this<Scene> {
public:
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

  /// Finds the shader with given file name.
  ShaderPtr shaderByFilename(const QString& filename);

  /// Name -> shader mapping
  std::map<QString, ProgramPtr> shaders() { return m_shaders; }

  /// Load the scene from map
  void load(QVariantMap map);

protected:
  typedef std::vector<RenderPassPtr> RenderPasses;
  /**
   * RenderPasses is defined as an ordered list of passes, while actually the
   * render passes form a directed dependency graph (without loops).
   * @todo The JSON structure should be treated as a set of Render Passes, and
   *       the actual order should be calculated when loading it.
   */
  RenderPasses m_render_passes;

  std::map<QString, ObjectPtr> m_objects;
  std::map<QString, LightPtr> m_lights;
  std::map<QString, CameraPtr> m_cameras;
  std::map<QString, ProgramPtr> m_shaders;
  
  int m_width, m_height;
};

#endif // SCENE_HPP
