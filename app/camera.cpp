/**
 * Copyright 2010,2011 Riku Palomäki.
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
  : SceneObject(name), m_type(Perspective),
    m_target(0, 0, 0), m_up(0, 1, 0),
    m_dx(0), m_dy(0),
    m_fov(45), m_near(0.1f), m_far(1000.0f) {}

void Camera::prepare(int width, int height) {
  glCheck("Camera::prepare");
  glViewport(0, 0, width, height);
  if (m_type == Perspective) {
    float f = 1.0f / tanf(m_fov*0.5f);
    float p[] = { f*height/width, 0.0f, 0.0f, 0.0f,
                  0.0f, f, 0.0f, 0.0f,
                  0.0f, 0.0f, (m_far+m_near)/(m_near-m_far), 2.0f*m_far*m_near/(m_near-m_far),
                  0.0f, 0.0f, -1.0f, 0.0f };

    glMatrixMode(GL_PROJECTION);
    glLoadTransposeMatrixf(p);

    float m[] = { float(m_right.x()),  float(m_right.y()),  float(m_right.z()), 0.0f,
                     float(m_up.x()),     float(m_up.y()),     float(m_up.z()), 0.0f,
                 float(-m_front.x()), float(-m_front.y()), float(-m_front.z()), 0.0f,
                                0.0f,                0.0f,                0.0f, 1.0f };
    glMatrixMode(GL_MODELVIEW);
    glLoadTransposeMatrixf(m);

    QVector3D eye = m_target - m_front*m_dist;
    glTranslatef(-eye.x(), -eye.y(), -eye.z());
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
  QVariantMap map = SceneObject::save();
  if (m_type == Perspective)
    map["type"] = "perspective";
  else if (m_type == Ortho)
    map["type"] = "ortho";
  else if (m_type == Rect)
    map["type"] = "rect";

  QVector3D eye = m_target - m_front*m_dist;
  map["position"] = toList(eye);
  map["target"] = toList(m_target);
  map["fov"] = m_fov;
  map["near"] = m_near;
  map["far"] = m_far;

  return map;
}

void Camera::load(QVariantMap map) {
  SceneObject::load(map);

  if (map["type"] == "perspective") m_type = Perspective;
  if (map["type"] == "ortho") m_type = Ortho;

  /// At least Qt 4.7 doesn't convert anything to QVector3D
  m_target = toVector(map["target"]);
  m_fov = map["fov"].toFloat();
  m_near = map["near"].toFloat();
  m_far = map["far"].toFloat();

  QVector3D to = m_target - toVector(map["position"]);
  m_dist = to.length();
  m_dx = atan2f(to.z(), -to.x());
  m_dy = asinf(-to.y() / m_dist);

  updateVectors();
}

void Camera::updateVectors() {
  float x = cosf(m_dx), z = -sinf(m_dx);
  float c = cosf(m_dy);
  m_front.setX(-c*x);
  m_front.setY(-sin(m_dy));
  m_front.setZ(-c*z);
  m_front.normalize();
  m_right = QVector3D::crossProduct(m_front, QVector3D(0, 1, 0)).normalized();
  m_up = QVector3D::crossProduct(m_right, m_front);
}

CameraPtr Camera::clone() const {
  return CameraPtr(new Camera(*this));
}

void Camera::rotate(QPointF diff) {
  diff *= 5.0f;
  m_dx -= diff.x();
  m_dy += diff.y();
  if (m_dy < -M_PI*0.499f) m_dy = -M_PI*0.499f;
  if (m_dy > M_PI*0.499f) m_dy = M_PI*0.499f;
  updateVectors();
}

void Camera::translate(QPointF diff) {
  diff *= m_dist;
  m_target += m_up*diff.y() - m_right*diff.x();
}

void Camera::zoom(float diff) {
  m_dist += diff*m_dist*0.01f;
}
