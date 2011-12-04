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

#ifndef RENDERPASS_HPP
#define RENDERPASS_HPP

#include "forward.hpp"
#include "opengl.hpp"
#include "shader/uniform.hpp"
#include "buffer_object.hpp"

struct RenderOptions {
  RenderOptions() : grid(false), ui(false), blueprint(false), grid_animation(2.0f), gizmo_type(NONE) {}
  bool grid;
  bool ui;
  bool blueprint;
  float grid_animation;
  // QSize size; /// @todo
  enum {
    NONE,
    TRANSLATE,
    ROTATE,
    SCALE
  } gizmo_type;
  GizmoPtr gizmo;
  FocusGrabberPtr focus_grabber;
  QPointF hover;
};

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
 * (or more generally "view"). It also has list of objects, lights and
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
    PostProc,
    Disabled
  } m_type;

  /// @todo separate Object from Model. Object is an instance of Model, including
  ///       the transformation matrix etc.
  typedef QSet<ObjectPtr> Objects;
  typedef QSet<LightPtr> Lights;

  RenderPass(QString name, ScenePtr scene);

  /// Render the pass
  void render(State& state, const RenderOptions& render_opts);

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
  void setName(const QString& name);

  Type type() const { return m_type; }
  void setType(Type type);

  ScenePtr scene() { return m_scene; }

  MaterialPtr defaultMaterial() { return m_defaultMaterial; }
  void setDefaultMaterial(MaterialPtr defaultMaterial);

  Objects objects() const { return m_objects; }
  void setObjects(Objects objs);
  void add(ObjectPtr obj);
  void remove(ObjectPtr obj);

  Lights lights() { return m_lights; }
  void setLights(Lights lights);
  void add(LightPtr light);

  CameraPtr view() { return m_view; }
  void setView(CameraPtr camera);

  QIcon icon();

  RenderPassPtr clone() const;

  FBOPtr fbo() const { return m_fbo; }

  void remove(FBOImagePtr buffer);
  void set(int target, FBOImagePtr buffer);

signals:
  void changed(RenderPassPtr);

protected:
  void beginFBO();
  void endFBO();
  void renderUI(State& state, const RenderOptions& render_opts);

  QString m_name;

  /// All objects that are rendered in this pass
  Objects m_objects;
  /// All enabled lights
  Lights m_lights;
  MaterialPtr m_defaultMaterial;
  CameraPtr m_view;

  ScenePtr m_scene;

  /// Bitwise OR of GL_{COLOR,DEPTH,STENCIL}_BUFFER_BIT, or zero if we don't
  /// want to clear the buffer before rendering.
  GLbitfield m_clear;

  int m_width, m_height; /// Output size, if zero, use scene size.
  bool m_autosize;
  FBOPtr m_fbo;

  BufferObject m_gridVertices, m_gridColors;
  ProgramPtr m_gridProg;
};

#endif // RENDERPASS_HPP
