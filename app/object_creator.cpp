#include "object_creator.hpp"

#include "state.hpp"
#include "renderpass.hpp"
#include "model.hpp"
#include "scene.hpp"
#include "object3d.hpp"
#include "shader/program.hpp"

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
}

Eigen::Vector3f project3(const Eigen::Projective3f& projection,
                         const Eigen::Vector3f& vector);

ObjectCreator::ObjectCreator(ScenePtr scene, const QString& name)
  : m_state(0),
    m_scene(scene),
    m_name(name),
    m_hover(0, 0, 0),
    m_prog(new GLProgram("object-creator")),
    m_window_to_obj(Eigen::Projective3f::Identity()) {
  for (int i = 0; i < 5; ++i) m_points[i] = Eigen::Vector3f(0, 0, 0);
  m_prog->addShaderSrc(vertex_shader, Shader::Vertex);
  m_prog->addShaderSrc(fragment_shader, Shader::Fragment);
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
    if (m_state == 1) ++m_state;
    else if (m_state == 3) {
      Eigen::AlignedBox<float, 3> box;
      for (int i = 0; i < 3; ++i) box.extend(m_points[i]);
      // we are not re-using models, since we don't want to change the scale here
      ModelPtr model = Model::createBuiltin(m_name, m_name, box.sizes());
      m_scene->add(model);
      ObjectPtr obj(new Object3D(model->name(), model));
      obj->transform() = Eigen::Translation3f(box.center());

      m_scene->add(obj);
      RenderPassPtr rp = m_scene->selectedRenderPass(RenderPass::Normal);

      if (rp) rp->add(obj);
    }
  }
  m_points[m_state] = hit(Eigen::Vector2f(ev->x(), ev->y()));
  return true;
}

void ObjectCreator::render(State& state, const RenderOptions& render_opts) {
  m_window_to_obj = state.transform(true).inverse();

  m_hover = hit(Eigen::Vector2f(render_opts.hover.x(), render_opts.hover.y()));
  float s = 100.0f;

  float hover[] = {
    m_hover[0]-s, m_hover[1],   m_hover[2],   m_hover[0]+s, m_hover[1], m_hover[2],
    m_hover[0]  , m_hover[1], m_hover[2]-s,     m_hover[0], m_hover[1], m_hover[2]+s,
  };

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
  int vertex_attrib = glGetAttribLocation(m_prog->id(), "vertex");
  glVertexAttribPointer(vertex_attrib, 3, GL_FLOAT, false, 0, hover);
  glEnableVertexAttribArray(vertex_attrib);

  if (m_state < 2) {
    glColor4f(1,1,1,0.15);
    glDepthFunc(GL_GEQUAL);
    glDrawArrays(GL_LINES, 0, 4);

    glColor4f(1,1,1,0.5);
    glDepthFunc(GL_LESS);
    glDrawArrays(GL_LINES, 0, 4);
  }

  if (m_state > 0) renderBox(state, render_opts);

  m_prog->unbind();
  state.pop();
}

void ObjectCreator::renderBox(State& state, const RenderOptions& render_opts) {
  const Eigen::Vector3f p = m_points[0];
  Eigen::Vector3f p2 = m_state == 1 ? m_hover : m_points[1];
  std::vector<Eigen::Vector3f> points;
  points.push_back(p);
  points.push_back(v3(p2[0], p[1], p[2]));
  points.push_back(p2);
  points.push_back(v3(p[0], p[1], p2[2]));
  points.push_back(p);

  glVertexAttribPointer(0, 3, GL_FLOAT, false, (char*)&points[1] - (char*)&points[0], &points[0]);

  glDepthFunc(GL_GEQUAL);

  glColor4f(1,1,1,0.25);
  glDrawArrays(GL_LINE_STRIP, 0, 5);
  glColor4f(1,1,1,0.1);
  glDrawArrays(GL_QUADS, 0, 4);

  glDepthFunc(GL_LESS);

  glColor4f(1,1,1,0.8);
  glDrawArrays(GL_LINE_STRIP, 0, 5);
  glColor4f(1,1,1,0.2);
  glDrawArrays(GL_QUADS, 0, 4);

  if (m_state > 1) {
    points.clear();
    Eigen::Vector3f p3 = m_state == 2 ? m_hover : m_points[2];

    points.push_back(p);
    points.push_back(v3(p[0], p3[1], p[2]));

    points.push_back(v3(p2[0], p[1], p[2]));
    points.push_back(v3(p2[0], p3[1], p[2]));

    points.push_back(p2);
    points.push_back(v3(p2[0], p3[1], p2[2]));

    points.push_back(v3(p[0], p[1], p2[2]));
    points.push_back(v3(p[0], p3[1], p2[2]));

    points.push_back(p);
    points.push_back(v3(p[0], p3[1], p[2]));

    points.push_back(v3(p[0], p3[1], p[2]));
    points.push_back(v3(p2[0], p3[1], p[2]));
    points.push_back(v3(p2[0], p3[1], p2[2]));
    points.push_back(v3(p[0], p3[1], p2[2]));
    points.push_back(v3(p[0], p3[1], p[2]));

    glVertexAttribPointer(0, 3, GL_FLOAT, false, (char*)&points[1] - (char*)&points[0], &points[0]);

    glDepthFunc(GL_GEQUAL);

    glColor4f(1,1,1,0.25);
    glDrawArrays(GL_LINES, 0, 8);
    glDrawArrays(GL_LINE_STRIP, 10, 5);

    glColor4f(1,1,1,0.1);
    glDrawArrays(GL_QUAD_STRIP, 0, 10);
    glDrawArrays(GL_QUADS, 10, 4);

    glDepthFunc(GL_LESS);

    glColor4f(1,1,1,0.8);
    glDrawArrays(GL_LINES, 0, 8);
    glDrawArrays(GL_LINE_STRIP, 10, 5);

    glColor4f(1,1,1,0.2);
    glDrawArrays(GL_QUAD_STRIP, 0, 10);
    glDrawArrays(GL_QUADS, 10, 4);
  }
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