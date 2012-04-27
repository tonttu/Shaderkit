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

#include "core/object_creator.hpp"
#include "core/renderpass.hpp"
#include "core/model.hpp"
#include "core/scene.hpp"
#include "core/object3d.hpp"
#include "core/mesh_manager.hpp"

#include "gl/state.hpp"
#include "gl/program.hpp"

#include <Eigen/OpenGLSupport>

#include <QMouseEvent>

inline Eigen::Vector3f v3(float x, float y, float z) { return Eigen::Vector3f(x, y, z); }

#define SHADER(x) #x

namespace {
const char* vertex_shader =
  "#version 150 compatibility\n"
  SHADER(
    precision highp float;
    in vec3 vertex;

    void main() {
      gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * vec4(vertex, 1.0);
      gl_FrontColor = gl_Color;
    }
  );

const char* fragment_shader =
  "#version 150 compatibility\n"
  SHADER(
    precision highp float;

    void main() {
      gl_FragColor = gl_Color;
    }
  );

const char* vertex_shader_norm =
  "#version 150 compatibility\n"
  SHADER(
    precision highp float;
    in vec3 vertex;
    in vec3 normal;

    out vec3 surface_normal;

    void main() {
      gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * vec4(vertex, 1.0);
      surface_normal = normal;
      gl_FrontColor = gl_Color;
    }
  );

const char* fragment_shader_norm =
  "#version 150 compatibility\n"
  SHADER(
    precision highp float;
    in vec3 surface_normal;

    void main() {
      vec3 surface_unit = normalize(surface_normal);
      float dotv = 1.0f;
      //float dotv = abs(surface_unit.z);
      gl_FragColor = vec4(gl_Color.rgb * dotv, gl_Color.a);
    }
  );
}

namespace Shaderkit {

Eigen::Vector3f project3(const Eigen::Projective3f& projection,
                         const Eigen::Vector3f& vector);

ObjectCreator::ObjectCreator(ScenePtr scene, const QString& name)
  : m_state(0),
    m_scene(scene),
    m_name(name),
    m_hover(0, 0, 0),
    m_prog(new GLProgram("object-creator")),
    m_prog2(new GLProgram("object-creator2")),
    m_window_to_obj(Eigen::Projective3f::Identity()) {
  for (int i = 0; i < 5; ++i) m_points[i] = Eigen::Vector3f(0, 0, 0);
  m_prog->addShaderSrc(vertex_shader, Shader::Vertex);
  m_prog->addShaderSrc(fragment_shader, Shader::Fragment);
  m_prog2->addShaderSrc(vertex_shader_norm, Shader::Vertex);
  m_prog2->addShaderSrc(fragment_shader_norm, Shader::Fragment);
}

bool ObjectCreator::move(QMouseEvent* ev) {
  m_points[m_state] = hit(Eigen::Vector2f(ev->x(), ev->y()));
  return true;
}

bool ObjectCreator::btn(QMouseEvent* ev) {
  if (ev->button() != Qt::LeftButton) return false;
  m_points[m_state] = hit(Eigen::Vector2f(ev->x(), ev->y()));
  if (ev->type() == QEvent::MouseButtonPress) {
    if(m_state == 0 || m_state == 2) ++m_state;
  } else {
    if (m_name == "sphere") {
      float r = (m_points[1] - m_points[0]).norm();
      QVariantMap map;
      map["size"] = QVariantList() << 2*r << 2*r << 2*r;
      ModelPtr model = Model::createBuiltin(m_name, m_name, map);
      m_scene->add(model);
      ObjectPtr obj(new Object3D(model->name(), model));
      Eigen::Vector3f c = m_points[0];
      c[1] += r;
      obj->transform() = Eigen::Translation3f(c);

      m_scene->add(obj);
      RenderPassPtr rp = m_scene->selectedRenderPass(RenderPass::Normal);

      if (rp) rp->add(obj);
      m_state = -1;
    } else {
      if (m_state == 1) ++m_state;
      else if (m_name == "box" && m_state == 3) {
        Eigen::AlignedBox<float, 3> box;
        for (int i = 0; i < 3; ++i) box.extend(m_points[i]);
        // we are not re-using models, since we don't want to change the scale here
        auto vec3 = box.sizes();
        QVariantMap map;
        map["size"] = QVariantList() << vec3[0] << vec3[1] << vec3[2];
        ModelPtr model = Model::createBuiltin(m_name, m_name, map);
        m_scene->add(model);
        ObjectPtr obj(new Object3D(model->name(), model));
        obj->transform() = Eigen::Translation3f(box.center());

        m_scene->add(obj);
        RenderPassPtr rp = m_scene->selectedRenderPass(RenderPass::Normal);

        if (rp) rp->add(obj);
        m_state = -1;
      }
    }
  }
  if (m_state >= 0 && m_state < 5)
    m_points[m_state] = hit(Eigen::Vector2f(ev->x(), ev->y()));
  return true;
}

bool ObjectCreator::done() const {
  return m_state < 0;
}

void ObjectCreator::render(State& state, const RenderOptions& render_opts) {
  m_window_to_obj = state.transform(true).inverse();

  m_hover = hit(Eigen::Vector2f(render_opts.hover.x(), render_opts.hover.y()));
  float s = 100.0f;

  BufferObject2 & hover_bo = MeshManager::fetch("ObjectCreator::hover");
  hover_bo.setUsage(GL_DYNAMIC_DRAW);
  hover_bo.set<GL_FLOAT>(VertexAttrib::Vertex0, 3);

  float hover[] = {
    m_hover[0]-s, m_hover[1],   m_hover[2],   m_hover[0]+s, m_hover[1], m_hover[2],
    m_hover[0]  , m_hover[1], m_hover[2]-s,     m_hover[0], m_hover[1], m_hover[2]+s,
  };

  hover_bo.upload(hover, 0, 4*3);

  state.push();
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_LIGHTING);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_FALSE);

  glDisable(GL_CULL_FACE);
  glLineWidth(1.2f);

  m_prog->bind(&state);
  state.attr()[VertexAttrib::Vertex0] = "vertex";

  if (m_state < 2) {
    BufferObject2::BindHolder b = hover_bo.bind(state);
    glColor4f(1,1,1,0.15f);
    glDepthFunc(GL_GEQUAL);
    glDrawArrays(GL_LINES, 0, 4);

    glColor4f(1,1,1,0.5f);
    glDepthFunc(GL_LESS);
    glDrawArrays(GL_LINES, 0, 4);
  }

  if (m_name == "box" && m_state > 0) renderBox(state, render_opts);
  if (m_name == "sphere" && m_state > 0) renderSphere(state, render_opts);

  m_prog->unbind();
  state.pop();
}

void ObjectCreator::renderBox(State& state, const RenderOptions& render_opts) {
  BufferObject2 & box_bo = MeshManager::fetch("ObjectCreator::renderBox");
  box_bo.setUsage(GL_DYNAMIC_DRAW);
  box_bo.set<GL_FLOAT>(VertexAttrib::Vertex0, 3);

  const Eigen::Vector3f p = m_points[0];
  Eigen::Vector3f p2 = m_state == 1 ? m_hover : m_points[1];

  {
    BufferObject2::Array<Eigen::Vector3f> points = box_bo.mapWrite<Eigen::Vector3f>(0, 5);
    points << p << v3(p2[0], p[1], p[2]) << p2 << v3(p[0], p[1], p2[2]) << p;
  }

  BufferObject2::BindHolder b = box_bo.bind(state);

  glDepthFunc(GL_GEQUAL);

  glColor4f(1,1,1,0.25f);
  glDrawArrays(GL_LINE_STRIP, 0, 5);
  glColor4f(1,1,1,0.1f);
  glDrawArrays(GL_QUADS, 0, 4);

  glDepthFunc(GL_LESS);

  glColor4f(1,1,1,0.8f);
  glDrawArrays(GL_LINE_STRIP, 0, 5);
  glColor4f(1,1,1,0.2f);
  glDrawArrays(GL_QUADS, 0, 4);

  if (m_state > 1) {
    {
      BufferObject2::Array<Eigen::Vector3f> points = box_bo.mapWrite<Eigen::Vector3f>(0, 15);

      Eigen::Vector3f p3 = m_state == 2 ? m_hover : m_points[2];

      points << p << v3(p[0], p3[1], p[2]);

      points << v3(p2[0], p[1], p[2]) << v3(p2[0], p3[1], p[2]);

      points << p2 << v3(p2[0], p3[1], p2[2]);

      points << v3(p[0], p[1], p2[2]) << v3(p[0], p3[1], p2[2]);

      points << p << v3(p[0], p3[1], p[2]);

      points << v3(p[0], p3[1], p[2]) << v3(p2[0], p3[1], p[2]) << v3(p2[0], p3[1], p2[2]) <<
                v3(p[0], p3[1], p2[2]) << v3(p[0], p3[1], p[2]);
    }

    glDepthFunc(GL_GEQUAL);

    glColor4f(1,1,1,0.25f);
    glDrawArrays(GL_LINES, 0, 8);
    glDrawArrays(GL_LINE_STRIP, 10, 5);

    glColor4f(1,1,1,0.1f);
    glDrawArrays(GL_QUAD_STRIP, 0, 10);
    glDrawArrays(GL_QUADS, 10, 4);

    glDepthFunc(GL_LESS);

    glColor4f(1,1,1,0.8f);
    glDrawArrays(GL_LINES, 0, 8);
    glDrawArrays(GL_LINE_STRIP, 10, 5);

    glColor4f(1,1,1,0.2f);
    glDrawArrays(GL_QUAD_STRIP, 0, 10);
    glDrawArrays(GL_QUADS, 10, 4);
  }
}

void ObjectCreator::renderSphere(State& state, const RenderOptions& render_opts) {
  Eigen::Vector3f p = m_points[0];
  const float r = (m_hover - p).norm();

  m_prog2->bind(&state);
  state.attr()[VertexAttrib::Vertex0] = "vertex";
  state.attr()[VertexAttrib::Normal] = "normal";

  p[1] += r;

  glPushMatrix();
  glMultMatrix(Eigen::Projective3f(Eigen::Translation3f(p)));

  if (r > 0.05f) {
    ObjectRenderer::drawSphere(r, 32, 32, state);
    glColor4f(0.2f, 0.4f, 0.8f, 0.1f);
    ObjectRenderer::drawBox(r, r, r, state);
  }

  glPopMatrix();
  m_prog2->unbind();
}


float closestLineParam(const Eigen::ParametrizedLine<float, 3>& p,
                       const Eigen::ParametrizedLine<float, 3>& q);

Eigen::Vector3f ObjectCreator::hit(const Eigen::Vector2f& c) {
  Eigen::Vector3f p = project3(m_window_to_obj, Eigen::Vector3f(c[0], c[1], 1.0f));
  Eigen::Vector3f p2 = project3(m_window_to_obj, Eigen::Vector3f(c[0], c[1], 0.0f));

  Eigen::ParametrizedLine<float, 3> ray = Eigen::ParametrizedLine<float, 3>::Through(p, p2);

  if (m_state > 1) {
    Eigen::ParametrizedLine<float, 3> up(m_points[1], Eigen::Vector3f(0, 1, 0));
    float height = closestLineParam(up, ray);
    return up.origin() + up.direction() * height;
  } else {
    Eigen::Hyperplane<float, 3> plane = Eigen::Hyperplane<float, 3>(
          Eigen::Vector3f(0, 1, 0), Eigen::Vector3f(0, 0, 0));

    return ray.origin() + ray.direction() * ray.intersection(plane);
  }
}

} // namespace Shaderkit
