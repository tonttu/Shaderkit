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

#ifndef RENDERPASS_HPP
#define RENDERPASS_HPP

#include "forward.hpp"
#include "opengl.hpp"
#include "shader/uniform.hpp"

/**
 * Render pass represents one renderable image that might only be a small part
 * of the whole composition. Different kinds of render passes could be for
 * example shadow map rendering, gaussian blur post-processing and normal phong
 * shading.
 *
 * Render pass might use outputs (textures) of other render passes as inputs
 * to shaders, and the output generated from this render pass might be either
 * the final output to screen or an intermediate result, rendered to texture
 * with Frame Buffer Object.
 *
 * Currently one render pass can have exactly one shader program and one Camera
 * (or more generally "viewport"). It also has list of objects, lights and
 * in/out textures.
 *
 * Render passes are controlled by Scene, the Pass itself doesn't know of any
 * other passes.
 *
 * @todo implement that stuff ^
 */
class RenderPass : public QObject, public std::enable_shared_from_this<RenderPass> {
  Q_OBJECT

public:
  enum Type {
    Normal,
    PostProc
  } m_type;

  /// @todo separate Object from Model. Object is an instance of Model, including
  ///       the transformation matrix etc.
  typedef QSet<ObjectPtr> Objects;
  typedef QSet<LightPtr> Lights;

  RenderPass(QString name, ScenePtr scene);

  /// Render the pass
  void render(State& state);

  QVariantMap save() const;

  /// Load the render pass from map
  void load(QVariantMap map);

  int height() const;
  int width() const;
  bool autosize() const;

  void setAutosize(bool v);
  void resize(int w, int h);

  GLbitfield clearBits() const { return m_clear; }
  void setClearBits(GLbitfield bits);

  QStringList out() const;
  FBOImagePtr out(const QString& name) const;

  QString name() const;

  Type type() const { return m_type; }
  void setType(Type type);

  ScenePtr scene() { return m_scene; }

  MaterialPtr defaultMaterial() { return m_defaultMaterial; }
  void setDefaultMaterial(MaterialPtr defaultMaterial);

  Objects objects() { return m_objects; }
  void setObjects(Objects objs);

  Lights lights() { return m_lights; }
  void setLights(Lights lights);

  CameraPtr viewport() { return m_viewport; }
  void setViewport(CameraPtr camera);

  QIcon icon();

signals:
  void changed(RenderPassPtr);

protected:
  void beginFBO();
  void endFBO();

  QString m_name;

  /// All objects that are rendered in this pass
  Objects m_objects;
  /// All enabled lights
  Lights m_lights;
  MaterialPtr m_defaultMaterial;
  CameraPtr m_viewport;

  ScenePtr m_scene;

  /// Bitwise OR of GL_{COLOR,DEPTH,STENCIL}_BUFFER_BIT, or zero if we don't
  /// want to clear the buffer before rendering.
  GLbitfield m_clear;

  int m_width, m_height; /// Output size, if zero, use scene size.
  bool m_autosize;
  FBOPtr m_fbo;
  FBOImagePtr m_depth, m_color;
};

#endif // RENDERPASS_HPP
