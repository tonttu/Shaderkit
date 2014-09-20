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

#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <cassert>

#include <QIcon>

// this include last because of near/far undefs
#include "gl/camera.hpp"

namespace Shaderkit
{
  Camera::Camera(const QString& name)
    : SceneObject(name),
      m_type(*this, Perspective),
      m_target(*this, glm::vec3(0, 0, 0)),
      m_up(0, 1, 0),
      m_dx(*this, 0),
      m_dy(*this, 0),
      m_dist(*this, 0.0f),
      m_width(-1),
      m_height(-1),
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
      m_projection(c.m_projection),
      m_view(c.m_view),
      m_width(c.m_width),
      m_height(c.m_height),
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
      m_projection = glm::mat4(
          f* height/width, 0.0f,                              0.0f, 0.0f,
                     0.0f,    f,                              0.0f, 0.0f,
                     0.0f, 0.0f,     (m_far+m_near)/(m_near-m_far), -1.f,
                     0.0f, 0.0f, 2.0f*m_far* m_near/(m_near-m_far), 0.0f);

      auto v = glm::mat4(
            m_right[0], m_up[0], -m_front[0], 0.0f,
            m_right[1], m_up[1], -m_front[1], 0.0f,
            m_right[2], m_up[2], -m_front[2], 0.0f,
                  0.0f,    0.0f,        0.0f, 1.0f);

      glm::vec3 eye = m_target.value() - m_front*m_dist.value();
      m_view = v * glm::translate(-eye);
    } else if (m_type == Rect) {
      float tz = -(m_far+m_near)/(m_far-m_near);
      m_projection = glm::mat4(
          2.0f/width,           0,                    0, 0,
                   0, 2.0f/height,                    0, 0,
                -1.f,           0, -2.0f/(m_far-m_near), 0,
                -1.f,           0,                   tz, 1);

      m_view = glm::mat4();
    } else {
      /// @todo implement ortho camera
      m_projection = glm::mat4();
      m_view = glm::mat4();
      assert(false && "Ortho Camera not implemented");
    }

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(glm::value_ptr(m_projection));

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(glm::value_ptr(m_view));
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

    glm::vec3 eye = m_target.value() - m_front*m_dist.value();
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
    m_front = glm::normalize(m_front);
    m_right = glm::normalize(glm::cross(m_front, glm::vec3(0, 1, 0)));
    m_up = glm::normalize(glm::cross(m_right, m_front));
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

  void Camera::rotate(const glm::vec2& diff)
  {
    const glm::vec2 tmp = diff * 5.0f;
    m_dx = m_dx.value() + tmp[0];
    m_dy = m_dy.value() + tmp[1];
    if (m_dy < -M_PI*0.499f) m_dy = -M_PI*0.499f;
    if (m_dy > M_PI*0.499f) m_dy = M_PI*0.499f;
    updateVectors();
  }

  void Camera::translate(const glm::vec2& diff)
  {
    const glm::vec2 tmp = diff * m_dist.value();
    m_target = m_target.value() + m_up*tmp[1] + m_right*tmp[0];
  }

  void Camera::zoom(float diff)
  {
    m_dist = m_dist.value() + diff*m_dist*0.01f;
  }

  void Camera::setTarget(const glm::vec3& target)
  {
    m_target = target;
  }

  const glm::vec3& Camera::target() const
  {
    return m_target;
  }

  void Camera::setLocation(const glm::vec3& location)
  {
    glm::vec3 to = m_target.value() - location;
    m_dist = glm::length(to);
    m_dx = atan2f(to[2], -to[0]);
    m_dy = asinf(-to[1] / m_dist);

    updateVectors();
  }

  const glm::vec3 Camera::location() const
  {
    return m_target.value() - m_front*m_dist.value();
  }

  const glm::vec3 Camera::up() const
  {
    return m_up;
  }

  const glm::vec3 Camera::right() const
  {
    return m_right;
  }

  const glm::vec3 Camera::front() const
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

  glm::mat4 Camera::normToWindow(bool swap_y) const
  {
    float f = swap_y ? -1.0f : 1.0f;

    auto window_scale = glm::mat4(
           m_width * 0.5f,                   0, 0, 0,
                        0, m_height * 0.5f * f, 0, 0,
                        0,                   0, 1, 0,
                        0,                   0, 0, 1);

    // -1..1 to window coordinates
    return window_scale * glm::translate(glm::vec3(1, f, 0));
  }

  glm::mat4 Camera::transform(bool swap_y) const
  {
    return normToWindow(swap_y) * projection() * view();
  }

  void Camera::attributeChanged()
  {
    emit changed(shared_from_this());
  }

} // namespace Shaderkit
