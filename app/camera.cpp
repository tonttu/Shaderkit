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

#include "camera.hpp"
#include "scene.hpp"
#include "opengl.hpp"

#include "Eigen/OpenGLSupport"

#include <cassert>

Camera::Camera(const QString &name)
  : SceneObject(name), m_type(Perspective),
    m_target(0, 0, 0), m_up(0, 1, 0),
    m_dx(0), m_dy(0),
    m_projectionMatrix(Eigen::Matrix4f::Identity()),
    m_viewMatrix(Eigen::Matrix4f::Identity()),
    m_fov(45), m_near(0.1f), m_far(1000.0f) {}

void Camera::prepare(int width, int height) {
  glCheck("Camera::prepare");
  glViewport(0, 0, width, height);
  if (m_type == Perspective) {
    float f = 1.0f / tanf(m_fov*0.5f);
    m_projectionMatrix <<
        f*height/width, 0.0f, 0.0f, 0.0f,
        0.0f, f, 0.0f, 0.0f,
        0.0f, 0.0f, (m_far+m_near)/(m_near-m_far), 2.0f*m_far*m_near/(m_near-m_far),
        0.0f, 0.0f, -1.0f, 0.0f;

    m_viewMatrix <<
        float(m_right.x()),  float(m_right.y()),  float(m_right.z()), 0.0f,
        float(m_up.x()),     float(m_up.y()),     float(m_up.z()), 0.0f,
        float(-m_front.x()), float(-m_front.y()), float(-m_front.z()), 0.0f,
        0.0f,                0.0f,                0.0f, 1.0f;

    QVector3D eye = m_target - m_front*m_dist;
    m_viewMatrix = (Eigen::Affine3f(m_viewMatrix) *
                    Eigen::Translation3f(-eye.x(), -eye.y(), -eye.z())).matrix();
  } else if (m_type == Rect) {
    float tz = -(m_far+m_near)/(m_far-m_near);
    m_projectionMatrix <<
        2.0f/width,           0,                    0, -1.0f,
                 0, 2.0f/height,                    0, -1.0f,
                 0,           0, -2.0f/(m_far-m_near),    tz,
                 0,           0,                    0,     1;

    m_viewMatrix = Eigen::Matrix4f::Identity();
  } else {
    /// @todo implement ortho camera
    m_projectionMatrix = Eigen::Matrix4f::Identity();
    m_viewMatrix = Eigen::Matrix4f::Identity();
    assert(false && "Ortho Camera not implemented");
  }

  glMatrixMode(GL_PROJECTION);
  glLoadMatrix(m_projectionMatrix);

  glMatrixMode(GL_MODELVIEW);
  glLoadMatrix(m_viewMatrix);
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

  setPosition(toVector(map["position"]));
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

void Camera::setTarget(const QVector3D& target) {
  m_target = target;
}

void Camera::setPosition(const QVector3D& position) {
  QVector3D to = m_target - position;
  m_dist = to.length();
  m_dx = atan2f(to.z(), -to.x());
  m_dy = asinf(-to.y() / m_dist);

  updateVectors();
}

const Eigen::Vector3f Camera::location() const {
  QVector3D tmp = m_target - m_front*m_dist;
  return Eigen::Vector3f(tmp.x(), tmp.y(), tmp.z());
}
