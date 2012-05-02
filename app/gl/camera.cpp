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

#include "gl/opengl.hpp"

#include "core/utils.hpp"

#include "Eigen/OpenGLSupport"

#include <cassert>

#include <QIcon>

// this include last because of near/far undefs
#include "gl/camera.hpp"

namespace Shaderkit
{
  Camera::Camera(const QString& name)
    : SceneObject(name),
      m_type(*this, Perspective),
      m_target(*this, Eigen::Vector3f(0, 0, 0)),
      m_up(0, 1, 0),
      m_dx(*this, 0),
      m_dy(*this, 0),
      m_dist(*this, 0.0f),
      m_projection(*this, Eigen::Projective3f::Identity()),
      m_view(*this, Eigen::Affine3f::Identity()),
      m_width(*this, -1),
      m_height(*this, -1),
      m_fov(*this, 45),
      m_near(*this, 0.1f),
      m_far(*this, 1000.0f)
  {
    updateVectors();
  }

  Camera::Camera(const Camera& c)
    : QObject(),
      std::enable_shared_from_this<Camera>(),
      SceneObject(c),
      m_type(*this, c.m_type),
      m_target(*this, c.m_target),
      m_up(c.m_up),
      m_right(c.m_right),
      m_front(c.m_front),
      m_dx(*this, c.m_dx),
      m_dy(*this, c.m_dy),
      m_dist(*this, c.m_dist),
      m_projection(*this, c.m_projection),
      m_view(*this, c.m_view),
      m_width(*this, c.m_width),
      m_height(*this, c.m_height),
      m_fov(*this, c.m_fov),
      m_near(*this, c.m_near),
      m_far(*this, c.m_far)
  {
  }

  void Camera::prepare(int width, int height)
  {
    glCheck("Camera::prepare");
    m_width = width;
    m_height = height;
    glViewport(0, 0, width, height);
    if (m_type == Perspective) {
      float f = 1.0f / tanf(m_fov*0.5f);
      Eigen::Projective3f p;
      p.matrix() <<
          f* height/width, 0.0f,                          0.0f,                              0.0f,
                     0.0f,    f,                          0.0f,                              0.0f,
                     0.0f, 0.0f, (m_far+m_near)/(m_near-m_far), 2.0f*m_far* m_near/(m_near-m_far),
                     0.0f, 0.0f,                         -1.0f,                              0.0f;

      Eigen::Affine3f v;
      v.matrix() <<
           float(m_right[0]),  float(m_right[1]),  float(m_right[2]), 0.0f,
              float(m_up[0]),     float(m_up[1]),     float(m_up[2]), 0.0f,
          float(-m_front[0]), float(-m_front[1]), float(-m_front[2]), 0.0f,
                        0.0f,               0.0f,               0.0f, 1.0f;

      m_projection = p;
      Eigen::Vector3f eye = m_target.value() - m_front*m_dist.value();
      m_view = v * Eigen::Translation3f(-eye);
    } else if (m_type == Rect) {
      float tz = -(m_far+m_near)/(m_far-m_near);
      Eigen::Projective3f p;
      p.matrix() <<
          2.0f/width,           0,                    0, -1.0f,
                   0, 2.0f/height,                    0, -1.0f,
                   0,           0, -2.0f/(m_far-m_near),    tz,
                   0,           0,                    0,     1;

      m_projection = p;
      m_view = Eigen::Affine3f::Identity();
    } else {
      /// @todo implement ortho camera
      m_projection = Eigen::Projective3f::Identity();
      m_view = Eigen::Affine3f::Identity();
      assert(false && "Ortho Camera not implemented");
    }

    glMatrixMode(GL_PROJECTION);
    glLoadMatrix(m_projection.value());

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrix(m_view.value());
  }

  void Camera::setRect(float near_, float far_)
  {
    m_type = Rect;
    m_near = near_;
    m_far = far_;
  }

  void Camera::setNear(float near)
  {
    m_near = near;
  }

  void Camera::setFar(float far)
  {
    m_far = far;
  }

  void Camera::setFov(float fov)
  {
    m_fov = fov;
  }

  QVariantMap Camera::toMap() const
  {
    QVariantMap map = SceneObject::toMap();
    if (m_type == Perspective)
      map["type"] = "perspective";
    else if (m_type == Ortho)
      map["type"] = "ortho";
    else if (m_type == Rect)
      map["type"] = "rect";

    Eigen::Vector3f eye = m_target.value() - m_front*m_dist.value();
    map["location"] = Utils::toList(eye);
    map["target"] = Utils::toList(m_target);
    map["fov"] = m_fov.value();
    map["near"] = m_near.value();
    map["far"] = m_far.value();

    return map;
  }

  void Camera::load(QVariantMap map)
  {
    SceneObject::load(map);

    if (map["type"] == "perspective") m_type = Perspective;
    if (map["type"] == "ortho") m_type = Ortho;

    m_target = Utils::toVector3(map["target"]);
    m_fov = map["fov"].toFloat();
    m_near = map["near"].toFloat();
    m_far = map["far"].toFloat();

    setLocation(Utils::toVector3(map["location"]));
  }

  void Camera::updateVectors()
  {
    float x = cosf(m_dx), z = -sinf(m_dx);
    float c = cosf(m_dy);
    m_front[0] =-c*x;
    m_front[1] = -sin(m_dy);
    m_front[2] = -c*z;
    m_front.normalize();
    m_right = m_front.cross(Eigen::Vector3f(0, 1, 0)).normalized();
    m_up = m_right.cross(m_front).normalized();
  }

  CameraPtr Camera::clone() const
  {
    return CameraPtr(new Camera(*this));
  }

  QIcon Camera::icon()
  {
    if (m_type == Perspective)
      return QIcon(":/icons/3dpass.png");
    else if (m_type == Rect)
      return QIcon(":/icons/texture.png");
    else
      return QIcon(":/icons/2dpass.png");
  }

  void Camera::rotate(QPointF diff)
  {
    diff *= 5.0f;
    m_dx = m_dx.value() - diff.x();
    m_dy = m_dy.value() + diff.y();
    if (m_dy < -M_PI*0.499f) m_dy = -M_PI*0.499f;
    if (m_dy > M_PI*0.499f) m_dy = M_PI*0.499f;
    updateVectors();
  }

  void Camera::translate(QPointF diff)
  {
    diff *= m_dist;
    m_target = m_target.value() + m_up*diff.y() - m_right*diff.x();
  }

  void Camera::zoom(float diff)
  {
    m_dist = m_dist.value() + diff*m_dist*0.01f;
  }

  void Camera::setTarget(const Eigen::Vector3f& target)
  {
    m_target = target;
  }

  const Eigen::Vector3f& Camera::target() const
  {
    return m_target;
  }

  void Camera::setLocation(const Eigen::Vector3f& location)
  {
    Eigen::Vector3f to = m_target.value() - location;
    m_dist = to.norm();
    m_dx = atan2f(to[2], -to[0]);
    m_dy = asinf(-to[1] / m_dist);

    updateVectors();
  }

  const Eigen::Vector3f Camera::location() const
  {
    return m_target.value() - m_front*m_dist.value();
  }

  const Eigen::Vector3f Camera::up() const
  {
    return m_up;
  }

  const Eigen::Vector3f Camera::right() const
  {
    return m_right;
  }

  const Eigen::Vector3f Camera::front() const
  {
    return m_front;
  }

  void Camera::setType(Type type)
  {
    m_type = type;
    updateVectors();
  }

  float Camera::dist() const
  {
    return m_dist;
  }

  Eigen::Projective3f Camera::normToWindow(bool swap_y) const
  {
    float f = swap_y ? -1.0f : 1.0f;

    Eigen::Projective3f window_scale;
    window_scale.matrix() << m_width * 0.5f,                   0, 0, 0,
                        0, m_height * 0.5f * f, 0, 0,
                        0,                   0, 1, 0,
                        0,                   0, 0, 1;

    // -1..1 to window coordinates
    return window_scale * Eigen::Translation3f(1, f, 0);
  }

  Eigen::Projective3f Camera::transform(bool swap_y) const
  {
    return normToWindow(swap_y) * projection() * view();
  }

  void Camera::attributeChanged()
  {
    emit changed(shared_from_this());
  }

} // namespace Shaderkit
