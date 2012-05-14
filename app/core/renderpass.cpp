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

#include "core/renderpass.hpp"
#include "core/scene.hpp"
#include "core/object3d.hpp"
#include "core/object_creator.hpp"
#include "core/material.hpp"
#include "core/utils.hpp"
#include "core/mesh_manager.hpp"

#include "gl/camera.hpp"
#include "gl/light.hpp"
#include "gl/program.hpp"
#include "gl/fbo.hpp"
#include "gl/texture.hpp"
#include "gl/state.hpp"

#include "gui/gizmos.hpp"
#include "gui/properties.hpp"
#include "gui/render_pass_properties.hpp"
#include "gui/mainwindow.hpp"

#define SHADER(x) #x

namespace
{
  const char* vertex_shader =
    "#version 150 compatibility\n"
    SHADER(
      precision highp float;

      in vec3 vertex;
      in vec4 color;
      out vec2 v;

      void main()
      {
        v = vertex.xz;
        gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * vec4(vertex, 1);
        gl_FrontColor = color;
      }
    );

  const char* fragment_shader =
    "#version 150 compatibility\n"
    SHADER(
      precision highp float;

      uniform float anim;
      uniform float active;
      uniform float time;
      in vec2 v;

      void main()
      {
        vec2 d = vec2(0.70710678, 0.70710678);
        float dist = abs(dot(d, v) - anim);
        float m = clamp(-2.0 / (10*5) * dist + 1.0, 0.0, 1.0);
        m *= m;
        gl_FragColor = mix(gl_Color, vec4(1, 1, 0, 1), mix(m, 1.0f, active*(1.0 + sin(time*6.456f)*0.3)));
      }
    );
}

namespace Shaderkit
{

  Eigen::Vector3f project3(const Eigen::Projective3f& projection,
                           const Eigen::Vector3f& vector);


  RenderPass::RenderPass(const QString& name, ScenePtr scene)
    : SceneObject(name),
      m_type(*this, Disabled),
      m_scene(scene),
      m_clear(*this, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT),
      m_clearColor(*this, Color::fromBytes(50, 50, 50)),
      m_width(*this, 0),
      m_height(*this, 0),
      m_autosize(*this, true),
      m_fbo(new FrameBufferObject),
      m_gridProg(new GLProgram("grid"))
  {
    m_gridProg->addShaderSrc(vertex_shader, Shader::Vertex);
    m_gridProg->addShaderSrc(fragment_shader, Shader::Fragment);
  }

  RenderPass::RenderPass(const RenderPass& r)
    : QObject(),
      std::enable_shared_from_this<RenderPass>(),
      SceneObject(r),
      m_type(*this, r.m_type),
      m_objects(r.m_objects),
      m_lights(r.m_lights),
      m_defaultMaterial(r.m_defaultMaterial),
      m_view(r.m_view),
      m_scene(r.m_scene),
      m_clear(*this, r.m_clear),
      m_clearColor(*this, r.m_clearColor),
      m_width(*this, r.m_width),
      m_height(*this, r.m_height),
      m_autosize(*this, r.m_autosize),
      m_fbo(new FrameBufferObject),
      m_gridProg(new GLProgram("grid"))
  {
    m_gridProg->addShaderSrc(vertex_shader, Shader::Vertex);
    m_gridProg->addShaderSrc(fragment_shader, Shader::Fragment);
  }

  int RenderPass::width() const
  {
    return m_width > 0 && !m_autosize ? m_width : m_scene->width();
  }

  int RenderPass::height() const
  {
    return m_height > 0 && !m_autosize ? m_height : m_scene->height();
  }

  bool RenderPass::autosize() const
  {
    return m_autosize;
  }

  void RenderPass::setAutosize()
  {
    m_width = 0;
    m_height = 0;
    m_autosize = true;
  }

  void RenderPass::setManualSize(int w, int h)
  {
    m_width = w;
    m_height = h;
    m_autosize = false;
  }

  void RenderPass::setClearBits(GLbitfield bits)
  {
    m_clear = bits;
  }

  void RenderPass::setClearColor(const Color& color)
  {
    m_clearColor = color;
  }

/// @todo delete these two functions
  QStringList RenderPass::out() const
  {
    QStringList lst;
    foreach (FBOImagePtr img, m_fbo->buffers())
      lst << img->name();
    return lst;
  }

  FBOImagePtr RenderPass::out(const QString& name) const
  {
    foreach (FBOImagePtr img, m_fbo->buffers())
      if (img->name() == name) return img;
    return FBOImagePtr();
  }

  void RenderPass::setType(Type type)
  {
    if (type == m_type) return;
    if (type == PostProc) {
      m_view.reset(new Camera("post"));
      m_view->setRect();
    } else if (type == Disabled) {
      m_view.reset();
    }
    m_type = type;
    emit changed(shared_from_this());
  }

  void RenderPass::setDefaultMaterial(const MaterialPtr& mat)
  {
    if (mat == m_defaultMaterial) return;
    m_defaultMaterial = mat;
    emit changed(shared_from_this());
  }

  void RenderPass::setObjects(const Objects& objs)
  {
    if (objs == m_objects) return;
    m_objects = objs;
    emit changed(shared_from_this());
  }

  void RenderPass::add(const ObjectPtr& obj)
  {
    if (m_objects.contains(obj)) return;
    m_objects << obj;
    emit changed(shared_from_this());
  }

  void RenderPass::remove(const ObjectPtr& obj)
  {
    if (!m_objects.contains(obj)) return;
    m_objects.remove(obj);
    emit changed(shared_from_this());
  }

  void RenderPass::setLights(const Lights& lights)
  {
    if (lights == m_lights) return;
    m_lights = lights;
    emit changed(shared_from_this());
  }

  void RenderPass::add(const LightPtr& light)
  {
    if (m_lights.contains(light)) return;
    m_lights << light;
    emit changed(shared_from_this());
  }

  void RenderPass::remove(const LightPtr& light)
  {
    if (!m_lights.contains(light)) return;
    m_lights.remove(light);
    emit changed(shared_from_this());
  }

  void RenderPass::setView(const CameraPtr& camera)
  {
    if (m_view == camera) return;
    m_view = camera;
    if (camera) {
      m_type = camera->type() == Camera::Perspective ? Normal : PostProc;
    } else {
      m_type = Disabled;
    }
    emit changed(shared_from_this());
  }

  QIcon RenderPass::icon() const
  {
    const char* icon = 0;
    if (m_type == PostProc)
      icon = ":/icons/2dpass.png";
    else if (m_type == Normal)
      icon = ":/icons/3dpass.png";
    else
      icon = ":/icons/disabled_pass.png";

    return QIcon(icon);
  }

  RenderPassPtr RenderPass::clone() const
  {
    return RenderPassPtr(new RenderPass(*this));
  }

  void RenderPass::render(State& state, const RenderOptions& render_opts)
  {
    state.push();
    beginFBO();

    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();

    state.setCamera(m_view);
    m_view->prepare(width(), height());

    glClearColor(m_clearColor.value()[0], m_clearColor.value()[1],
                 m_clearColor.value()[2], m_clearColor.value()[3]);
    if (m_clear) glClear(m_clear);
    if (m_defaultMaterial) state.pushMaterial(m_defaultMaterial);
    state.applyUniformsMappings();

    if (m_type == PostProc) {
      glDisable(GL_DEPTH_TEST);
      glDisable(GL_LIGHTING);
      glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
      glEnable(GL_TEXTURE_2D);

      glBegin(GL_QUADS);

      glTexCoord2f(0.0f, 0.0f);
      glVertex2f(0.0f, 0.0f);

      glTexCoord2f(1.0f, 0.0f);
      glVertex2f(width(), 0.0f);

      glTexCoord2f(1.0f, 1.0f);
      glVertex2f(width(), height());

      glTexCoord2f(0.0f, 1.0f);
      glVertex2f(0.0f, height());

      glEnd();
    } else {
      glEnable(GL_DEPTH_TEST);
      glEnable(GL_CULL_FACE);
      glCullFace(GL_BACK);
      glFrontFace(GL_CCW);
      glEnable(GL_NORMALIZE);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      glShadeModel(GL_SMOOTH);
      glEnable(GL_MULTISAMPLE);
      //glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST);

      foreach (LightPtr light, m_lights)
        light->activate(state);

      foreach (ObjectPtr obj, m_objects) {
        glPushMatrix();
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        obj->render(state);
        glRun(glPopAttrib());
        glRun(glPopMatrix());
      }

      foreach (LightPtr light, m_lights)
        light->deactivate(state);

      if (render_opts.ui) renderUI(state, render_opts);
    }

    if (m_defaultMaterial) state.popMaterial();
    endFBO();
    state.pop();
  }

  void RenderPass::beginFBO()
  {
    if (m_fbo->isEmpty()) return;

    m_fbo->resize(width(), height());
    m_fbo->bind();
  }

  void RenderPass::endFBO()
  {
    m_fbo->unbind();
    foreach (FBOImagePtr img, m_fbo->buffers())
      img->dataUpdated();
  }

  void RenderPass::renderUI(State& state, const RenderOptions& render_opts)
  {
    state.push();
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    glLineWidth(1.7f);

    const float step = 10.0f;
    const int dist = 5;

    if (render_opts.grid) {
      BufferObject2& gridVertices = MeshManager::fetch("gridVertices");
      BufferObject2& gridColors = MeshManager::fetch("gridColors");
      if (!gridVertices.size()) {
        std::vector<float> v;
        std::vector<float> c;
#define V(a,b,c) v.push_back(a), v.push_back(b), v.push_back(c)
#define C(r,g,b,a) c.push_back(r), c.push_back(g), c.push_back(b), c.push_back(a)
        for (int i = -dist; i <= dist; ++i) {
          V(-dist*step, 0, i*step);
          V(dist*step, 0, i*step);
          V(i*step, 0, -dist*step);
          V(i*step, 0, dist*step);

          if (i == 0) {
            V((dist-0.2f)*step, 0, -step*0.2f);
            V(dist*step, 0, 0);
            V((dist-0.2f)*step, 0, step*0.2f);
            V(dist*step, 0, 0);

            V(-step*0.2f, 0, (dist-0.2f)*step);
            V(0, 0, dist*step);
            V(step*0.2f, 0, (dist-0.2f)*step);
            V(0, 0, dist*step);

            for (int j = 0; j < 2; ++j) C(1.0f, 0.5, 0.5f, 0.5f);
            for (int j = 0; j < 2; ++j) C(0.5f, 1.0, 0.5f, 0.5f);
            for (int j = 0; j < 4; ++j) C(1.0f, 0.5, 0.5f, 0.5f);
            for (int j = 0; j < 4; ++j) C(0.5f, 1.0, 0.5f, 0.5f);
          } else {
            for (int j = 0; j < 4; ++j) {
              C(0.5f, 0.5f, 0.5f, 0.3f);
            }
          }
        }
#undef V
#undef C
        gridVertices.set<GL_FLOAT>(VertexAttrib::Vertex0, 3);
        gridVertices.upload(v);

        gridColors.set<GL_FLOAT>(VertexAttrib::Color0, 4, 0, 0);
        gridColors.upload(c);
      }
      state.attr()[VertexAttrib::Vertex0] = "vertex";
      state.attr()[VertexAttrib::Color0] = "color";

      m_gridProg->bind(&state);
      auto b1 = gridVertices.bind(state);
      auto b2 = gridColors.bind(state);

      if (render_opts.grid_animation <= 1.0f) {
        const float drop = step * dist * 0.5f;
        const float r = 1.4142135623731f * step * dist + drop;
        m_gridProg->setUniform(&state, "anim", (render_opts.grid_animation * 2.0f - 1.0f) * r);
      } else {
        m_gridProg->setUniform(&state, "anim", step * dist * 10.0f);
      }
      m_gridProg->setUniform(&state, "time", state.time());
      m_gridProg->setUniform(&state, "active", render_opts.focus_grabber ? 0.2f : 0);
      glRun(glDrawArrays(GL_LINES, 0, gridVertices.size()/sizeof(float)/3));
      m_gridProg->unbind();
    }

    state.pop();

    if (render_opts.focus_grabber) {
      render_opts.focus_grabber->render(state, render_opts);
    }

    if (render_opts.gizmo)
      render_opts.gizmo->render(QSize(width(), height()), state, render_opts);
  }

  QVariantMap RenderPass::toMap() const
  {
    QVariantMap map;

    QVariantList tmp;
    if (m_clear & GL_COLOR_BUFFER_BIT) tmp << "color";
    if (m_clear & GL_DEPTH_BUFFER_BIT) tmp << "depth";
    if (m_clear & GL_STENCIL_BUFFER_BIT) tmp << "stencil";

    if (!tmp.isEmpty()) map["clear"] = tmp;

    map["name"] = name();
    map["background"] = m_clearColor->toQVariant();
    if (m_defaultMaterial)
      map["material"] = m_defaultMaterial->name();

    tmp.clear();
    foreach (ObjectPtr obj, m_objects)
      tmp << obj->name();
    if (!tmp.isEmpty()) map["objects"] = tmp;

    tmp.clear();
    foreach (LightPtr obj, m_lights)
      tmp << obj->name();
    if (!tmp.isEmpty()) map["lights"] = tmp;

    if (m_type == Normal) {
      tmp.clear();
      tmp << "camera" << m_view->name();
      map["view"] = tmp;
    } else if (m_type == PostProc) {
      map["view"] = "post";
    }

    QVariantMap in, out;

    if (m_width > 0 && !m_autosize) out["width"] = m_width.value();
    if (m_height > 0 && !m_autosize) out["height"] = m_height.value();

    auto buffers = m_fbo->buffers();
    for (auto it = buffers.begin(); it != buffers.end(); ++it) {
      QString bufname;
      if (it.key() == GL_DEPTH_ATTACHMENT) {
        bufname = "depth";
      } else if (it.key() >= GL_COLOR_ATTACHMENT0 && it.key() < GL_COLOR_ATTACHMENT0 + 8) {
        bufname = QString("color%1").arg(it.key() - GL_COLOR_ATTACHMENT0);
      } else {
        Log::error("Corrupted fbo buffers, buffer name %d doesn't make sense", it.key());
        continue;
      }
      tmp.clear();
      tmp << (*it)->imageClass();
      if (!(*it)->name().isEmpty())
        tmp << (*it)->name();
      out[bufname] = tmp;
    }

    /** @todo to material
    foreach (QString name, m_in.keys()) {
      tmp.clear();
      tmp << "texture" << m_in[name]->name();
      in[name] = tmp;
    }

    if (!in.isEmpty()) map["in"] = in;*/
    if (!out.isEmpty()) map["out"] = out;

    return map;
  }

  void RenderPass::load(QVariantMap map)
  {
    if (map.contains("material"))
      m_defaultMaterial = m_scene->material(map["material"].toString());

    foreach (QString name, map["objects"].toStringList()) {
      ObjectPtr o = m_scene->object(name);
      if (o) m_objects.insert(o);
    }

    foreach (QString name, map["lights"].toStringList()) {
      LightPtr l = m_scene->light(name);
      if (l) m_lights.insert(l);
    }

    QStringList tmp = map["view"].toStringList();
    if (tmp.size() == 2 && tmp[0] == "camera") {
      m_view = m_scene->camera(tmp[1]);
      m_type = Normal;
    }
    if (tmp.size() == 1 && tmp[0] == "post") {
      setType(PostProc);
    }

    foreach (QString name, map["clear"].toStringList()) {
      if (name == "color") m_clear |= GL_COLOR_BUFFER_BIT;
      else if (name == "depth") m_clear |= GL_DEPTH_BUFFER_BIT;
      else if (name == "stencil") m_clear |= GL_STENCIL_BUFFER_BIT;
    }

    m_clearColor = Color::fromQVariant(map["background"]);

    QVariantMap out = map["out"].toMap();

    m_width = out["width"].toInt();
    m_height = out["height"].toInt();

    m_autosize = m_width <= 0 || m_height <= 0;

    for (int i = -1; i < 16; ++i) {
      int target = i == -1 ? GL_DEPTH_ATTACHMENT : GL_COLOR_ATTACHMENT0 + i;
      QString bufname = i == -1 ? "depth" : QString("color%1").arg(i);
      tmp = out[bufname].toStringList();
      if (tmp.size() == 2 && tmp[0] == "texture")
        m_fbo->set(target, m_scene->genTexture(tmp[1]));

      if (tmp.size() == 1 && tmp[0] == "renderbuffer")
        m_fbo->set(target, FBOImagePtr(new RenderBuffer));

      if (tmp.size() == 2 && tmp[0] == "renderbuffer")
        m_fbo->set(target, FBOImagePtr(new RenderBuffer(tmp[1])));
    }

    emit changed(shared_from_this());
  }

  void RenderPass::attributeChanged()
  {
    emit changed(shared_from_this());
  }

} // namespace Shaderkit
