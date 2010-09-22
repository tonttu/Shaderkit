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

#ifndef RENDERPASS_HPP
#define RENDERPASS_HPP

#include "forward.hpp"
#include "opengl.hpp"

#include <boost/enable_shared_from_this.hpp>

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
class RenderPass : public QObject, public boost::enable_shared_from_this<RenderPass> {
  Q_OBJECT

public:
  /// @todo separate Object from Model. Object is an instance of Model, including
  ///       the transformation matrix etc.
  typedef QSet<ObjectPtr> Objects;
  typedef QSet<LightPtr> Lights;

  RenderPass(ScenePtr scene);

  /// Render the pass
  void render(State& state);

  /// Load the render pass from map
  void load(QVariantMap map);

  int height() const;
  int width() const;

  QStringList in() const {
    return m_in.keys();
  }

  TexturePtr in(const QString& name) const {
    return m_in[name];
  }

  QStringList out() const;
  FBOImagePtr out(const QString& name) const;

  QString name() const;

signals:
  void changed(RenderPassPtr);

protected:
  void beginFBO();
  void endFBO();

  enum Type {
    Normal,
    PostProc
  } m_type;

  /// All objects that are rendered in this pass
  Objects m_objects;
  /// All enabled lights
  Lights m_lights;
  CameraPtr m_viewport;
  ProgramPtr m_shader;

  ScenePtr m_scene;

  /// Bitwise OR of GL_{COLOR,DEPTH,STENCIL}_BUFFER_BIT, or zero if we don't
  /// want to clear the buffer before rendering.
  GLbitfield m_clear;

  int m_width, m_height; /// Output size, if zero, use scene size.
  FBOPtr m_fbo;
  FBOImagePtr m_depth, m_color;

  QMap<QString, TexturePtr> m_in;
};

#endif // RENDERPASS_HPP
