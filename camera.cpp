#include "camera.hpp"
#include "json_value.hpp"

#include <QtOpenGL>

Camera::Camera(const QString &name)
  : m_name(name), m_type(Perspective),
    m_position(3, 1, 2), m_target(0, 0, 0), m_up(0, 1, 0),
    m_fov(45), m_near(0.1), m_far(1000.0) {}

void Camera::prepare(int width, int height) {
  glViewport(0, 0, width, height);
  if (m_type == Perspective) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(m_fov, (GLfloat)width/height, m_near, m_far);

    glMatrixMode(GL_MODELVIEW);

    glLoadIdentity();
    // Should we not use glu? It's trivial to calculate this manually too
    gluLookAt(m_position.x(), m_position.y(), m_position.z(),
              m_target.x(), m_target.y(), m_target.z(),
              m_up.x(), m_up.y(), m_up.z());
  } else {
    /// @todo implement ortho camera
    assert(false && "Ortho Camera not implemented");
  }
}

void Camera::load(const Value& value) {
  if (value.str("type") == "perspective") m_type = Perspective;
  if (value.str("type") == "ortho") m_type = Ortho;

  value.to(m_position, "position");
  value.to(m_target, "target");
  value.to(m_up, "up");
  value.to(m_fov, "fov");
  value.to(m_near, "near");
  value.to(m_far, "far");
}
