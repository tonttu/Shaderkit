#include "gizmos.hpp"
#include "state.hpp"
#include "object3d.hpp"
#include "model.hpp"
#include "camera.hpp"
#include "shader/program.hpp"

#include "Eigen/OpenGLSupport"

namespace {
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


Eigen::Vector2f project2(const Eigen::Projective3f& projection,
                         const Eigen::Vector3f& vector) {
  Eigen::Vector4f tmp = projection * Eigen::Vector4f(vector[0], vector[1], vector[2], 1.0f);
  return Eigen::Vector2f(tmp[0]/tmp[3], tmp[1]/tmp[3]);
}

const char* vertex_shader =
    "#version 150 compatibility\n"
    "\n"
    "precision highp float;\n"
    "\n"
    "/*out vec2 point;*/\n"
    "\n"
    "uniform float grayness;\n"
    "\n"
    "void main() {\n"
    "  gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;\n"
    "  gl_FrontColor = mix(gl_Color, vec4(1.0, 1.0, 1.0, 1.0), grayness);\n"
    "  /*point = -gl_Position.xy / gl_Position.w;*/\n"
    "}\n";

const char* fragment_shader =
    "#version 150 compatibility\n"
    "\n"
    "precision highp float;\n"
    "\n"
    "void main() {\n"
    "  gl_FragColor = gl_Color;"
    "}\n";

LineSegment::LineSegment(const Eigen::Vector2f& a, const Eigen::Vector2f& b)
  : m_point(a),
    m_point2(b) {
  Eigen::Vector2f tmp = m_point2 - m_point;
  m_length = tmp.norm();
  m_unit = tmp / m_length;
}

float LineSegment::hit(const Eigen::Vector2f& p, float threshold2) const {
  float project = m_unit.dot(p - m_point);
  if (project < 0.0f) {
    if (project * project > threshold2) return -1.0f;
    float tmp = (p - m_point).squaredNorm();
    return tmp < threshold2 ? tmp : -1.0f;
  } else if(project > m_length) {
    if ((project - m_length) * (project - m_length) > threshold2) return -1.0f;
    float tmp = (p - m_point2).squaredNorm();
    return tmp < threshold2 ? tmp : -1.0f;
  } else {
    float tmp = Eigen::Vector2f(m_unit[1], -m_unit[0]).dot(p - m_point);
    tmp *= tmp;
    return tmp < threshold2 ? tmp : -1.0f;
  }
}


Gizmo::Gizmo() : m_hover(NONE), m_selected(NONE), m_active(false), m_scale(-1.0f) {
  m_prog.reset(new GLProgram("gizmo"));
  m_prog->addShaderSrc(vertex_shader, Shader::Vertex);
  m_prog->addShaderSrc(fragment_shader, Shader::Fragment);
}
Gizmo::~Gizmo() {}

void Gizmo::setObject(ObjectPtr obj) {
  if(m_active)
    buttonUp();
  m_object = obj;
}

void Gizmo::hover(const Eigen::Vector2f& point) {
  const HitShape* hit = pick(point);
  m_hover = hit ? hit->group : m_selected;
}

bool Gizmo::buttonDown(const Eigen::Vector2f& point) {
  const HitShape* hit = pick(point);
  if (hit) {
    m_selected = hit->group;
    m_active = true;
  }
  return hit != 0;
}

void Gizmo::input(const Eigen::Vector2f& diff) {
  /// @todo do something
}

void Gizmo::buttonUp() {
  m_active = false;
}

float Gizmo::size() const {
  return 100.0f;
}

const Gizmo::HitShape* Gizmo::pick(const Eigen::Vector2f& point) const {
  const HitShape* hit = 0;
  float hit_dist2 = 9.0f * 9.0f;
  foreach (const HitShape& shape, m_hit_shapes) {
    if (!shape.bbox.contains(point)) continue;
    foreach (const LineSegment& s, shape.transformed) {
      float test2 = s.hit(point, hit_dist2);
      if (test2 >= 0.0f) {
        hit_dist2 = test2;
        hit = &shape;
      }
    }
  }

  return hit;
}

void TranslateGizmo::render(QSize size, State& state, const RenderOptions& render_opts) {
  state.push();

  glDisable(GL_TEXTURE_2D);
  glDisable(GL_LIGHTING);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glDisable(GL_CULL_FACE);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_LINE_SMOOTH);
  glLineWidth(1.7f);

  Eigen::Vector3f center = m_object->model()->bbox().center();
  auto model = Eigen::Affine3f(Eigen::Translation3f(center))
      * m_object->transform();

  auto all = state.camera()->projection() * state.camera()->view() * model;
  if(m_scale < 0.0f || m_active) {
    auto inv = all.inverse(Eigen::Projective);

    Eigen::Vector4f qwe = all * Eigen::Vector4f(0, 0, 0, 1);
    Eigen::Vector4f a = inv * Eigen::Vector4f(0, 0, qwe[2], 1);
    Eigen::Vector4f b = inv * Eigen::Vector4f(size.height()*0.005f, 0, qwe[2], 1);
    Eigen::Vector3f aa(a[0]/a[3], a[1]/a[3], a[2]/a[3]);
    Eigen::Vector3f bb(b[0]/b[3], b[1]/b[3], b[2]/b[3]);
    float diff = (aa - bb).norm();

    m_scale = 0.05/diff;
  }

  all = all * Eigen::Scaling(m_scale);
  state.pushTransform(model * Eigen::Scaling(m_scale));
  glPushMatrix();
  glLoadMatrix(state.camera()->view() * state.transform());

  const int segments = 8;

  if (m_verts.size() == 0) {
    std::vector<float> vv(9*2*3 + 3*3*segments + 3*4*3), cv(3*3*2*4 + 4*3*segments + 4*4*3);
    const float* src = translate_gizmo_vertices;
    float* v = &vv[0];
    float* c = &cv[0];

    m_hit_shapes.clear();
    QVector<Eigen::Vector3f> points[6];

    // X/Y/Z and XY/XZ/YZ
    for (int i = 0; i < 9; ++i) {
      for (int j = 0; j < 6; ++j) *v++ = src[j];
      points[translate_gizmo_groups[i]] << Eigen::Vector3f(src[0], src[1], src[2]);
      points[translate_gizmo_groups[i]] << Eigen::Vector3f(src[3], src[4], src[5]);
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
      *v++ = *src++; *v++ = *src++; *v++ = *src++;
      *c++ = 1.0f; *c++ = 1.0f; *c++ = 0.0f; *c++ = 0.5f;
    }

    m_verts.enableArray(state, GL_VERTEX_ARRAY, 3, vv);
    m_colors.enableArray(state, GL_COLOR_ARRAY, 4, cv);

    for (int c = X; c <= YZ; ++c)
      m_hit_shapes.push_back(HitShape(points[c], Constraint(c)));
  } else {
    m_verts.enableArray(state, GL_VERTEX_ARRAY, 3);
    m_colors.enableArray(state, GL_COLOR_ARRAY, 4);
  }

  float* c = m_colors.mapRW();

  for (int i = 0; i < 9; ++i) {
    Constraint color = translate_gizmo_colors[i];
    for (int p = 0; p < 2; ++p) {
      if (translate_gizmo_groups[i] == m_hover) {
        *c++ = 1.0f; *c++ = 1.0f; *c++ = 0.0f;
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
  c = 0;

  for (auto it = m_hit_shapes.begin(); it != m_hit_shapes.end(); ++it)
    it->transform(all, size);

  m_prog->bind(&state);
  glDepthMask(GL_FALSE);

  for (int i = 0; i < 2; ++i) {
    glDepthFunc(i == 0 ? GL_GEQUAL : GL_LESS);
    m_prog->setUniform(&state, "grayness", 0.5f - i*0.5f);
    glRun(glDrawArrays(GL_LINES, 0, 3*2*3));
    glRun(glDrawArrays(GL_TRIANGLE_FAN, 3*2*3, segments));
    glRun(glDrawArrays(GL_TRIANGLE_FAN, 3*2*3+segments, segments));
    glRun(glDrawArrays(GL_TRIANGLE_FAN, 3*2*3+2*segments, segments));
  }

  if (m_hover >= XY && m_hover <= YZ) {
    glDisable(GL_DEPTH_TEST);
    m_prog->setUniform(&state, "grayness", 0.0f);
    glRun(glDrawArrays(GL_QUADS, 3*2*3+3*segments + (m_hover-XY)*4, 4));
  }

  m_prog->unbind();
  glRun(glPopMatrix());
  state.popTransform();
  state.pop();
}

void RotateGizmo::render(QSize size, State& state, const RenderOptions& render_opts) {

}

void ScaleGizmo::render(QSize size, State& state, const RenderOptions& render_opts) {

}

void Gizmo::HitShape::transform(const Eigen::Projective3f& projection, QSize size) {
  bbox.setEmpty();

  /// @todo why this fails without parentheses?
  Eigen::Projective3f window_projection =
      Eigen::AlignedScaling3f(size.width() * 0.5f, size.height() * 0.5f, 1.0f) *
      (Eigen::Translation3f(1, 1, 0) *
      projection);

  transformed.resize(points.size() / 2);
  for (int i = 0, s = points.size(); i < s; i += 2) {
    // project 3D coordinates to 2D window coordinates
    transformed[i/2] = LineSegment(project2(window_projection, points[i]),
                                   project2(window_projection, points[i+1]));

    bbox.extend(transformed[i/2].point());
    bbox.extend(transformed[i/2].point2());
  }
  bbox.extend(bbox.min() - Eigen::Vector2f(9.0f, 9.0f));
  bbox.extend(bbox.max() + Eigen::Vector2f(9.0f, 9.0f));
}
