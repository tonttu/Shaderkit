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

#include "gui/gizmos.hpp"
#include "gl/state.hpp"
#include "core/object3d.hpp"
#include "core/model.hpp"
#include "gl/camera.hpp"
#include "gl/program.hpp"
#include "gui/glwidget.hpp"

#include "glm/gtx/norm.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtx/rotate_normalized_axis.hpp"

namespace
{
  using namespace Shaderkit;

  const float ll = 0.2f, mm = 0.3f, hh = 1.0f;
  const float translate_gizmo_vertices[] = {
    ll,  0,  0,   hh,  0,  0,
    0, mm,  0,    0, mm, mm,
    0,  0, mm,    0, mm, mm,

    0, ll,  0,    0, hh,  0,
    0,  0, mm,   mm,  0, mm,
    mm,  0,  0,   mm,  0, mm,

    0,  0, ll,    0,  0, hh,
    0, mm,  0,   mm, mm,  0,
    mm,  0,  0,   mm, mm,  0,

    mm,  0,  0,   mm, mm,  0,    0, mm,  0,    0,  0,  0,
    0,  0,  0,    0,  0, mm,   mm,  0, mm,   mm,  0,  0,
    0,  0,  0,    0, mm,  0,    0, mm, mm,    0,  0, mm
  };

  const Gizmo::Constraint translate_gizmo_colors[] = {
    Gizmo::X, Gizmo::Y, Gizmo::Z,
    Gizmo::Y, Gizmo::Z, Gizmo::X,
    Gizmo::Z, Gizmo::Y, Gizmo::X
  };

  const Gizmo::Constraint translate_gizmo_groups[] = {
    Gizmo::X, Gizmo::YZ, Gizmo::YZ,
    Gizmo::Y, Gizmo::XZ, Gizmo::XZ,
    Gizmo::Z, Gizmo::XY, Gizmo::XY
  };
}

namespace Shaderkit
{

  glm::vec2 project2(const glm::mat4& projection,
                           const glm::vec3& vector)
  {
    glm::vec4 tmp = projection * glm::vec4(vector[0], vector[1], vector[2], 1.0f);
    return glm::vec2(tmp[0]/tmp[3], tmp[1]/tmp[3]);
  }

  glm::vec3 project3(const glm::mat4& projection,
                           const glm::vec3& vector)
  {
    glm::vec4 tmp = projection * glm::vec4(vector[0], vector[1], vector[2], 1.0f);
    return glm::vec3(tmp[0]/tmp[3], tmp[1]/tmp[3], tmp[2]/tmp[3]);
  }

/// p.origin() + p.direction() * closestLineParam(p, q) == closest point of line q on line p
/// @see http://softsurfer.com/Archive/algorithm_0106/algorithm_0106.htm#Distance%20between%20Lines
  float closestLineParam(const Line& p,
                         const Line& q)
  {
    const glm::vec3 w0 = p.origin() - q.origin();
    float a = glm::dot(p.direction(), p.direction());
    float b = glm::dot(p.direction(), q.direction());
    float c = glm::dot(q.direction(), q.direction());
    float d = glm::dot(p.direction(), w0);
    float e = glm::dot(q.direction(), w0);
    float denom = a*c - b*b; // always nonnegative

    if (denom < 1.0e-6f) return 0;
    return (b*e - c*d) / denom; // s
  }

  const char* vertex_shader_translate =
    "#version 150 compatibility\n"
    "\n"
    "precision highp float;\n"
    "\n"
    "uniform float grayness;\n"
    "\n"
    "void main() {\n"
    "  gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;\n"
    "  gl_FrontColor = mix(gl_Color, vec4(1.0, 1.0, 1.0, 1.0), grayness);\n"
    "}\n";

  const char* fragment_shader_translate =
    "#version 150 compatibility\n"
    "\n"
    "precision highp float;\n"
    "\n"
    "void main() {\n"
    "  gl_FragColor = gl_Color;\n"
    "}\n";

  const char* vertex_shader_rotate =
    "#version 150 compatibility\n"
    "\n"
    "precision highp float;\n"
    "\n"
    "out vec3 point;\n"
    "\n"
    "uniform float grayness;\n"
    "\n"
    "void main() {\n"
    "  vec4 p = gl_ModelViewMatrix * gl_Vertex;"
    "  gl_Position = gl_ProjectionMatrix * p;\n"
    "  point = p.xyz / p.w;\n"
    "  gl_FrontColor = mix(gl_Color, vec4(1.0, 1.0, 1.0, 1.0), grayness);\n"
    "}\n";

  const char* fragment_shader_rotate =
    "#version 150 compatibility\n"
    "\n"
    "precision highp float;\n"
    "\n"
    "in vec3 point;\n"
    "\n"
    "uniform vec3 center;\n"
    "uniform vec3 cam;\n"
    "uniform vec3 dir;\n"
    "uniform float mix_amount;\n"
    "\n"
    "void main() {\n"
    "  if (dot(cam - point, center - point) <= 0.0) {\n"
    "    gl_FragColor = gl_Color;\n"
    "  } else {\n"
    "    gl_FragColor = mix(gl_Color, vec4(0.5, 0.5, 0.5, 0.125), mix_amount);\n"
    "  }\n"
    "}\n";

  GizmoLineSegment::GizmoLineSegment(const glm::vec3& a, const glm::vec3& b)
    : m_point(a[0], a[1]), m_point2(b[0], b[1]),
      m_depth(a[2]), m_depth2(b[2])
  {
    glm::vec2 tmp = m_point2 - m_point;
    m_length = glm::length(tmp);
    m_unit = tmp / m_length;
  }

  float GizmoLineSegment::hit(const glm::vec2& p, float threshold2) const
  {
    float project = glm::dot(m_unit, p - m_point);
    if (project < 0.0f) {
      if (project * project > threshold2) return -1.0f;
      float tmp = glm::length2(p - m_point);
      return tmp < threshold2 ? tmp : -1.0f;
    } else if (project > m_length) {
      if ((project - m_length) * (project - m_length) > threshold2) return -1.0f;
      float tmp = glm::length2(p - m_point2);
      return tmp < threshold2 ? tmp : -1.0f;
    } else {
      float tmp = glm::dot(glm::vec2(m_unit[1], -m_unit[0]), p - m_point);
      tmp *= tmp;
      return tmp < threshold2 ? tmp : -1.0f;
    }
  }

  bool GizmoLineSegment::hit(const glm::vec2& p, float threshold2, float& depth) const
  {
    float project = glm::dot(m_unit, p - m_point);
    if (project < 0.0f) {
      if (depth < m_depth || project * project > threshold2) return false;
      float tmp = glm::length2(p - m_point);
      if (tmp > threshold2) return false;
      depth = m_depth;
    } else if (project > m_length) {
      if (depth < m_depth2 || (project - m_length) * (project - m_length) > threshold2) return false;
      float tmp = glm::length2(p - m_point2);
      if (tmp > threshold2) return false;
      depth = m_depth2;
    } else {
      float tmp = glm::dot(glm::vec2(m_unit[1], -m_unit[0]), p - m_point);
      tmp *= tmp;
      if (tmp > threshold2) return false;
      float factor = project / m_length;
      tmp = m_depth * (1.0f - factor) + m_depth2 * factor;
      if (depth < tmp) return false;
      depth = tmp;
    }
    return true;
  }

  Gizmo::Gizmo() : m_hover(NONE), m_selected(NONE), m_active(false), m_hit_mode(NEAREST),
    m_scale(-1.0f), m_scale_factor(1),
    m_start_cursor(0, 0), m_current_cursor(0, 0),
    m_update_inv_projection(false)
  {
    m_prog.reset(new GLProgram("gizmo"));
  }
  Gizmo::~Gizmo() {}

  void Gizmo::setObject(ObjectPtr obj)
  {
    if (m_object == obj) return;
    if (m_active)
      buttonUp();
    m_object = obj;
  }

  void Gizmo::render(QSize size, State& state, const RenderOptions&)
  {
    state.push();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glDisable(GL_CULL_FACE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    glDepthMask(GL_FALSE);
    glLineWidth(1.7f);

    glm::vec3 center = m_object->model()->bbox().center();
    m_gizmo_to_obj = glm::translate(center);

    // -1..1 to window coordinates
    glm::mat4 normalized_to_window = state.camera()->normToWindow();

    state.pushModel(m_object->transform());

    // projection x view x model. Transforms model coordinates to normalized frustum
    auto transform = state.camera()->projection() * state.camera()->view() *
                     state.model();

    if (m_scale < 0.0f || !m_active) {
      // gizmo center in global coordinates
      const glm::vec3 gizmo_center = project3(state.model() * m_gizmo_to_obj,
                                           glm::vec3(0, 0, 0));

      // gizmo distance from camera
      float dist = glm::length(gizmo_center - state.camera()->location());
      dist *= m_scale_factor * 75.0f / size.height() * std::tan(state.camera()->fov());

      m_scale = dist;
    }

    m_gizmo_to_obj = m_gizmo_to_obj * glm::scale(glm::vec3(m_scale));

    // Gizmo coordinates to window coordinates
    m_window_projection = normalized_to_window * transform * m_gizmo_to_obj;


    if (m_update_inv_projection) {
      // window to gizmo coordinates
      m_window_to_gizmo = glm::inverse(m_window_projection);

      m_update_inv_projection = false;
    }

    state.pushModel(m_gizmo_to_obj);
    glPushMatrix();
    glLoadMatrixf(glm::value_ptr(state.camera()->view() * state.model()));

    m_prog->bind(&state);
    renderImpl(state);
    m_prog->unbind();

    glRun(glPopMatrix());
    state.popModel();
    state.popModel();
    state.pop();
  }

  void Gizmo::hover(const glm::vec2& point)
  {
    const HitShape* hit = pick(point);
    m_hover = hit ? hit->group : m_selected;
  }

  bool Gizmo::buttonDown(const glm::vec2& point)
  {
    m_start_cursor = m_current_cursor = point;
    const HitShape* hit = pick(point);
    return hit && makeActive(hit->group);
  }

  void Gizmo::input(const glm::vec2&)
  {
  }

  void Gizmo::buttonUp()
  {
    m_active = false;
    m_selected = NONE;
  }

  float Gizmo::size() const
  {
    return 100.0f;
  }

  const Gizmo::HitShape* Gizmo::pick(const glm::vec2& point) const
  {
    int hit = -1;
    float hit_dist2 = 9.0f * 9.0f;
    float depth = 10.0f;
    for (int i = 0, m = m_hit_shapes.size(); i < m; ++i) {
      if (!m_hit_shapes[i].bbox.contains(point)) continue;
      foreach (const GizmoLineSegment& s, m_hit_shapes[i].transformed) {
        if (m_hit_mode == FRONT) {
          if (s.hit(point, hit_dist2, depth)) {
            hit = i;
          }

        } else {
          float test2 = s.hit(point, hit_dist2);
          if (test2 >= 0.0f) {
            hit_dist2 = test2;
            hit = i;
          }
        }
      }
    }

    return hit >= 0 ? &m_hit_shapes[hit] : 0;
  }

  bool Gizmo::makeActive(Constraint type)
  {
    m_selected = type;
    m_active = true;
    m_update_inv_projection = true;
    m_object_orig_transform = m_object->transform();

    return true;
  }

  TranslateGizmo::TranslateGizmo() : m_update_line(false)
  {
    m_prog->addShaderSrc(vertex_shader_translate, Shader::Vertex);
    m_prog->addShaderSrc(fragment_shader_translate, Shader::Fragment);
  }


  void TranslateGizmo::renderImpl(State& state)
  {
    const int segments = 8;

    if (m_verts.size() == 0) {
      std::vector<float> vv(9*2*3 + 3*3*segments + 3*4*3), cv(3*3*2*4 + 4*3*segments + 4*4*3);
      const float* src = translate_gizmo_vertices;
      float* v = &vv[0];
      float* c = &cv[0];

      m_hit_shapes.clear();
      QVector<glm::vec3> points[6];

      // X/Y/Z and XY/XZ/YZ
      for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 6; ++j) *v++ = src[j];
        points[translate_gizmo_groups[i]] << glm::vec3(src[0], src[1], src[2]);
        points[translate_gizmo_groups[i]] << glm::vec3(src[3], src[4], src[5]);
        src += 6;
      }
      c += 9*8;

      // three arrow heads
      for (int i = 0; i < 3; ++i) {
        *v++ = i == 0 ? 1 : 0;
        *v++ = i == 1 ? 1 : 0;
        *v++ = i == 2 ? 1 : 0;

        for (int j = 1; j < segments; ++j) {
          float a = M_PI*2.0f / (segments-2.0f) * j;
          float s = std::sin(a)*0.04f, c = std::cos(a)*0.04f;
          *v++ = i == 0 ? 0.85f : s;
          *v++ = i == 1 ? 0.85f : (i == 2 ? c : s);
          *v++ = i == 2 ? 0.85f : c;
        }
      }
      c += 4*3*segments;

      // plane quads
      for (int j = 0; j < 3*4; ++j) {
        *v++ = *src++;
        *v++ = *src++;
        *v++ = *src++;
        *c++ = 1.0f;
        *c++ = 1.0f;
        *c++ = 0.0f;
        *c++ = 0.5f;
      }

      m_verts.enableArray(state, GL_VERTEX_ARRAY, 3, vv);
      m_colors.enableArray(state, GL_COLOR_ARRAY, 4, cv);

      for (int c = X; c <= YZ; ++c)
        m_hit_shapes.push_back(HitShape(points[c], Constraint(c)));
    } else {
      m_verts.enableArray(state, GL_VERTEX_ARRAY, 3);
      m_colors.enableArray(state, GL_COLOR_ARRAY, 4);
    }

    {
      float* c = m_colors.mapRW();

      for (int i = 0; i < 9; ++i) {
        Constraint color = translate_gizmo_colors[i];
        for (int p = 0; p < 2; ++p) {
          if (translate_gizmo_groups[i] == m_hover) {
            *c++ = 1.0f;
            *c++ = 1.0f;
            *c++ = 0.0f;
          } else {
            *c++ = color == X ? 1.0f : 0.0f;
            *c++ = color == Y ? 1.0f : 0.0f;
            *c++ = color == Z ? 1.0f : 0.0f;
          }
          *c++ = 1.0f;
        }
      }

      for (int i = X; i <= Z; ++i) {
        for (int j = 0; j < segments; ++j) {
          if (i == m_hover) {
            *c++ = 1.0f;
            *c++ = 1.0f;
            *c++ = 0.0f;
          } else {
            *c++ = i == 0 ? 1.0f : 0.0f;
            *c++ = i == 1 ? 1.0f : 0.0f;
            *c++ = i == 2 ? 1.0f : 0.0f;
          }
          *c++ = 1.0f;
        }
      }

      m_colors.unmap();
    }

    for (auto it = m_hit_shapes.begin(); it != m_hit_shapes.end(); ++it)
      it->transform(m_window_projection);

    glEnable(GL_DEPTH_TEST);

    for (int i = 0; i < 2; ++i) {
      glDepthFunc(i == 0 ? GL_GEQUAL : GL_LESS);
      m_prog->setUniform(&state, "grayness", 0.5f - i*0.5f);
      glRun(glDrawArrays(GL_LINES, 0, 3*2*3));
      glRun(glDrawArrays(GL_TRIANGLE_FAN, 3*2*3, segments));
      glRun(glDrawArrays(GL_TRIANGLE_FAN, 3*2*3+segments, segments));
      glRun(glDrawArrays(GL_TRIANGLE_FAN, 3*2*3+2*segments, segments));
    }

    // render "planes"
    if (m_hover >= XY && m_hover <= YZ) {
      glDisable(GL_DEPTH_TEST);
      m_prog->setUniform(&state, "grayness", 0.0f);
      glRun(glDrawArrays(GL_QUADS, 3*2*3+3*segments + (m_hover-XY)*4, 4));
    }
  }

  void TranslateGizmo::input(const glm::vec2& diff)
  {
    m_current_cursor += diff;
    if (m_update_inv_projection) return;
    bool plane = m_selected == XY || m_selected == XZ || m_selected == YZ;

    if (m_update_line) {
      glm::vec3 p = project3(m_window_to_gizmo, glm::vec3(m_start_cursor[0], m_start_cursor[1], 1.0f));
      glm::vec3 p2 = project3(m_window_to_gizmo, glm::vec3(m_start_cursor[0], m_start_cursor[1], 0.0f));

      // in gizmo coordinates
      Line ray = Line::through(p, p2);

      if (plane) {
        glm::vec3 normal(m_selected == YZ ? 1.0f : 0.0f,
                               m_selected == XZ ? 1.0f : 0.0f,
                               m_selected == XY ? 1.0f : 0.0f);

        m_plane = Plane(normal, glm::vec3(0, 0, 0));
        m_line = Line::originAndDirection(ray.origin() + ray.direction() * ray.intersection(m_plane),
                                          m_plane.normal());
      } else {
        glm::vec3 dir(m_selected == X ? 1.0f : 0.0f,
                            m_selected == Y ? 1.0f : 0.0f,
                            m_selected == Z ? 1.0f : 0.0f);
        // in gizmo coordinates
        m_line = Line::originAndDirection(glm::vec3(0, 0, 0), dir);
        m_line.origin() = dir * closestLineParam(m_line, ray);
      }
      m_update_line = false;
    }

    auto tmp = m_window_to_gizmo;

    glm::vec3 p1 = project3(tmp, glm::vec3(m_current_cursor[0], m_current_cursor[1], 1.0f));
    glm::vec3 p2 = project3(tmp, glm::vec3(m_current_cursor[0], m_current_cursor[1], 0.0f));

    // in gizmo coordinates
    Line ray = Line::through(p1, p2);

    if (plane) {
      // the point we have moved to, in gizmo coordinates
      glm::vec3 point = ray.intersection(m_plane);

      // rotate/scale that vector to object coordinates and move the object
      m_object->setTransform(m_object_orig_transform *
                             glm::translate(glm::mat3(m_gizmo_to_obj) * (point - m_line.origin())));
    } else {
      // how much we have moved in gizmo coordinates
      glm::vec3 diff_gizmo = m_line.direction() * closestLineParam(m_line, ray);

      // rotate/scale that vector to object coordinates and move the object
      m_object->setTransform(m_object_orig_transform *
                             glm::translate(glm::mat3(m_gizmo_to_obj) * diff_gizmo));
    }
  }

  bool TranslateGizmo::makeActive(Constraint type)
  {
    m_update_line = true;
    return Gizmo::makeActive(type);
  }

  RotateGizmo::RotateGizmo() : m_update_center(false), m_reversed(false), m_angle(0), m_start_angle(0)
  {
    m_prog->addShaderSrc(vertex_shader_rotate, Shader::Vertex);
    m_prog->addShaderSrc(fragment_shader_rotate, Shader::Fragment);
    m_hit_mode = FRONT;
    m_scale_factor = 0.5f;
  }

  void RotateGizmo::renderImpl(State& state)
  {
    m_update_inv_projection = true;
    const int segments = 48;

    if (m_verts.size() == 0) {
      std::vector<float> vv(3*4*(segments+1) + 3*(segments+2)), cv(4*4*(segments+1) + 4*(segments+2));
      float* v = &vv[0];
      float* c = &cv[0];

      m_hit_shapes.clear();
      QVector<glm::vec3> points[7];

      // X/Y/Z
      for (int i = 0; i < 3; ++i) {
        for (int j = 0; j <= segments; ++j) {
          float a = M_PI*2.0f / segments * j;
          float s = std::sin(a), c = std::cos(a);
          *v++ = i == 0 ? 0 : s;
          *v++ = i == 1 ? 0 : (i == 2 ? c : s);
          *v++ = i == 2 ? 0 : c;

          if (j > 0 && j < segments) points[i] << glm::vec3(v[-3], v[-2], v[-1]);
          points[i] << glm::vec3(v[-3], v[-2], v[-1]);
        }
      }

      c += 4*3*(segments+1);
      for (int j = 0; j <= segments; ++j) {
        *c++ = 0.8f;
        *c++ = 0.8f;
        *c++ = 0.8f;
        *c++ = 1.0f;
      }

      for (int j = 0; j <= segments + 1; ++j) {
        *c++ = 0.2f;
        *c++ = 0.5f;
        *c++ = 1.0;
        *c++ = 0.2f;
      }

      m_verts.enableArray(state, GL_VERTEX_ARRAY, 3, vv);
      m_colors.enableArray(state, GL_COLOR_ARRAY, 4, cv);

      for (int c = X; c <= Z; ++c)
        m_hit_shapes.push_back(HitShape(points[c], Constraint(c)));
    } else {
      m_verts.enableArray(state, GL_VERTEX_ARRAY, 3);
      m_colors.enableArray(state, GL_COLOR_ARRAY, 4);
    }

    {
      float* c = m_colors.mapRW();

      for (int i = 0; i < 3; ++i) {
        for (int j = 0; j <= segments; ++j) {
          if (i == m_hover) {
            *c++ = 1.0f;
            *c++ = 1.0f;
            *c++ = 0.0f;
          } else {
            *c++ = i == X ? 1.0f : 0.0f;
            *c++ = i == Y ? 1.0f : 0.0f;
            *c++ = i == Z ? 1.0f : 0.0f;
          }
          *c++ = 1.0f;
        }
      }

      m_colors.unmap();
    }

    // gizmo center in camera coordinates
    const glm::vec3 gizmo_center = project3(
                                           state.camera()->view() * state.model(), glm::vec3(0, 0, 0));

    {
      glm::vec2 window_loc = project2(m_window_projection, glm::vec3(0, 0, 0));
      glm::vec3 a = project3(m_window_to_gizmo, glm::vec3(window_loc[0], window_loc[1], 0));
      glm::vec3 b = project3(m_window_to_gizmo, glm::vec3(window_loc[0], window_loc[1], 1));
      glm::vec3 dir = glm::normalize(b-a);
      glm::vec3 x = glm::cross(glm::vec3(std::sqrt(3.f)), dir);
      if (std::abs(glm::length2(x) - 1.0f) > 0.1) x = glm::cross(glm::vec3(1, 0, 0), dir);
      glm::vec3 y = glm::cross(dir, x);

      glm::vec3 camera_loc_in_gizmo_coords = project3(
            glm::inverse(state.model()), state.camera()->location());
      float d = glm::length(camera_loc_in_gizmo_coords);

      glm::vec3 front = glm::normalize(camera_loc_in_gizmo_coords);

      float radius = std::sqrt(1.0f - 1.0f/(d*d));
      glm::vec3 circle_center = front * (1.0f / d);

      float* v = m_verts.mapRW();
      v += 3*3*(segments+1);

      for (int j = 0; j <= segments; ++j) {
        float a = M_PI*2.0f / segments * j;
        float s = std::sin(a) * radius, c = std::cos(a) * radius;
        glm::vec3 tmp = s*x + c*y + circle_center;
        v[3*(segments+2)] = tmp[0];
        *v++ = tmp[0];
        v[3*(segments+2)] = tmp[1];
        *v++ = tmp[1];
        v[3*(segments+2)] = tmp[2];
        *v++ = tmp[2];
      }

      *v++ = circle_center[0];
      *v++ = circle_center[1];
      *v++ = circle_center[2];

      m_verts.unmap();
    }

    for (auto it = m_hit_shapes.begin(); it != m_hit_shapes.end(); ++it)
      it->transform(m_window_projection);

    m_prog->setUniform(&state, "mix_amount", 0.0f);
    m_prog->setUniform(&state, "center", gizmo_center);
    m_prog->setUniform(&state, "cam", project3(state.camera()->view(), state.camera()->location()));
    m_prog->setUniform(&state, "grayness", 0.0f);

    glDisable(GL_DEPTH_TEST);
    glRun(glDrawArrays(GL_TRIANGLE_FAN, 4*(segments+1), segments+2));
    glRun(glDrawArrays(GL_LINE_STRIP, 3*(segments+1), segments+1));

    m_prog->setUniform(&state, "mix_amount", 0.8f);

    glEnable(GL_DEPTH_TEST);
    glLineWidth(2.7f);
    for (int i = 0; i < 2; ++i) {
      glDepthFunc(i == 0 ? GL_GEQUAL : GL_LESS);
      m_prog->setUniform(&state, "grayness", 0.5f - i*0.5f);
      glRun(glDrawArrays(GL_LINE_STRIP, 0, segments+1));
      glRun(glDrawArrays(GL_LINE_STRIP, segments+1, segments+1));
      glRun(glDrawArrays(GL_LINE_STRIP, 2*(segments+1), segments+1));
    }
  }

  void RotateGizmo::input(const glm::vec2& diff)
  {
    m_current_cursor += diff;
    if (m_selected < X || m_selected > Z) return;

    glm::vec3 normal(m_selected == X ? 1.0f : 0.0f,
                           m_selected == Y ? 1.0f : 0.0f,
                           m_selected == Z ? 1.0f : 0.0f);

    if (m_update_center) {
      m_center = project2(m_window_projection, glm::vec3(0, 0, 0));
      glm::vec2 dir = m_current_cursor - m_center;
      m_start_angle = std::atan2(dir[1], dir[0]);

      auto tst = glm::mat3(m_window_projection) * normal;
      m_reversed = tst[2] > 0.0f;

      m_update_center = false;
    }

    glm::vec2 dir = m_current_cursor - m_center;
    float angle = std::atan2(dir[1], dir[0]);

    m_object->setTransform(glm::rotateNormalizedAxis(m_object_orig_transform * m_gizmo_to_obj,
                                                     (m_reversed ? -1.0f : 1.0f) * (angle - m_start_angle), normal) *
                           glm::inverse(m_gizmo_to_obj));
  }

  bool RotateGizmo::makeActive(Constraint type)
  {
    m_update_center = true;
    return Gizmo::makeActive(type);
  }

  void ScaleGizmo::renderImpl(State&)
  {

  }

  void Gizmo::HitShape::transform(const glm::mat4& window_projection)
  {
    bbox.clear();

    transformed.resize(points.size() / 2);
    for (int i = 0, s = points.size(); i < s; i += 2) {
      // project 3D coordinates to 3D window coordinates
      transformed[i/2] = GizmoLineSegment(project3(window_projection, points[i]),
                                          project3(window_projection, points[i+1]));

      bbox.extend(transformed[i/2].point());
      bbox.extend(transformed[i/2].point2());
    }
    bbox.extend(bbox.min() - glm::vec2(9.0f, 9.0f));
    bbox.extend(bbox.max() + glm::vec2(9.0f, 9.0f));
  }

} // namespace Shaderkit
