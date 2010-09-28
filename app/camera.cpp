/**
 * Copyright 2010 Riku Palomäki.
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

#include "camera.hpp"
#include "scene.hpp"
#include "opengl.hpp"

#include <cassert>

Camera::Camera(const QString &name)
  : m_name(name), m_type(Perspective),
    m_position(3, 1, 2), m_target(0, 0, 0), m_up(0, 1, 0),
    m_fov(45), m_near(0.1f), m_far(1000.0f) {}

void Camera::prepare(int width, int height) {
  glCheck("Camera::prepare");
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
  } else if (m_type == Rect) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, 0, height, m_near, m_far);

    glMatrixMode(GL_MODELVIEW);

    glLoadIdentity();
  } else {
    /// @todo implement ortho camera
    assert(false && "Ortho Camera not implemented");
  }
}

void Camera::setRect(float near_, float far_) {
  m_type = Rect;
  m_near = near_;
  m_far = far_;
}

QVariantMap Camera::save() const {
  QVariantMap map;
  if (m_type == Perspective)
    map["type"] = "perspective";
  else if (m_type == Ortho)
    map["type"] = "ortho";
  else if (m_type == Rect)
    map["type"] = "rect";

  map["position"] = toList(m_position);
  map["target"] = toList(m_target);
  map["up"] = toList(m_up);
  map["fov"] = m_fov;
  map["near"] = m_near;
  map["far"] = m_far;

  return map;
}

void Camera::load(QVariantMap map) {
  if (map["type"] == "perspective") m_type = Perspective;
  if (map["type"] == "ortho") m_type = Ortho;

  /// At least Qt 4.7 doesn't convert anything to QVector3D
  m_position = toVector(map["position"]);
  m_target = toVector(map["target"]);
  m_up = toVector(map["up"]);
  m_fov = map["fov"].toFloat();
  m_near = map["near"].toFloat();
  m_far = map["far"].toFloat();
}
