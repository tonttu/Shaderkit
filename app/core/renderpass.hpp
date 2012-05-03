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

#ifndef RENDERPASS_HPP
#define RENDERPASS_HPP

#include "forward.hpp"

#include "core/color.hpp"
#include "core/attribute.hpp"

#include "gl/scene_object.hpp"
#include "gl/opengl.hpp"
#include "gl/uniform.hpp"
#include "gl/buffer_object.hpp"

#include <QColor>
#include <QPointF>

namespace Shaderkit
{

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
  class RenderPass : public QObject, public std::enable_shared_from_this<RenderPass>,
      public SceneObject
  {
    Q_OBJECT

  public:
    enum Type {
      Normal,
      PostProc,
      Disabled
    };

    /// @todo separate Object from Model. Object is an instance of Model, including
    ///       the transformation matrix etc.
    typedef QSet<ObjectPtr> Objects;
    typedef QSet<LightPtr> Lights;

    RenderPass(const QString& name, ScenePtr scene);

    /// Render the pass
    void render(State& state, const RenderOptions& render_opts);

    QVariantMap toMap() const;

    /// Load the render pass from map
    void load(QVariantMap map);

    int height() const;
    int width() const;
    bool autosize() const;

    void setAutosize();
    void setManualSize(int w, int h);

    GLbitfield clearBits() const { return m_clear; }
    void setClearBits(GLbitfield bits);

    const Color& clearColor() const { return m_clearColor; }
    void setClearColor(const Color& color);

    QStringList out() const;
    FBOImagePtr out(const QString& name) const;

    /// @todo this should be removed, CameraEditor & Camera have the same functionality
    Type type() const { return m_type; }
    void setType(Type type);

    ScenePtr scene() const { return m_scene; }

    MaterialPtr defaultMaterial() const { return m_defaultMaterial; }
    void setDefaultMaterial(const MaterialPtr& defaultMaterial);

    Objects objects() const { return m_objects; }
    void setObjects(const Objects& objs);
    void add(const ObjectPtr& obj);
    void remove(const ObjectPtr& obj);

    Lights lights() { return m_lights; }
    void setLights(const Lights& lights);
    void add(const LightPtr& light);
    void remove(const LightPtr& light);

    CameraPtr view() const { return m_view; }
    void setView(const CameraPtr& camera);

    QIcon icon() const;

    RenderPassPtr clone() const;

    FBOPtr fbo() const { return m_fbo; }

    void remove(FBOImagePtr buffer);
    void set(int target, FBOImagePtr buffer);

    virtual void attributeChanged();

  signals:
    void changed(RenderPassPtr);

  private:
    RenderPass(const RenderPass& r);

    void beginFBO();
    void endFBO();
    void renderUI(State& state, const RenderOptions& render_opts);

  private:
    Attribute<Type> m_type;

    /// All objects that are rendered in this pass
    Objects m_objects;
    /// All enabled lights
    Lights m_lights;
    MaterialPtr m_defaultMaterial;
    CameraPtr m_view;

    ScenePtr m_scene;

    /// Bitwise OR of GL_{COLOR,DEPTH,STENCIL}_BUFFER_BIT, or zero if we don't
    /// want to clear the buffer before rendering.
    Attribute<GLbitfield> m_clear;
    Attribute<Color> m_clearColor;

    Attribute<int> m_width, m_height; /// Output size, if zero, use scene size.
    Attribute<bool> m_autosize;
    FBOPtr m_fbo;

    ProgramPtr m_gridProg;
  };

} // namespace Shaderkit

#endif // RENDERPASS_HPP
