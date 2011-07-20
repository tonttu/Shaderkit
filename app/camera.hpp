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

#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "forward.hpp"
#include "scene_object.hpp"

#include "Eigen/Geometry"

#include <QVariant>

#ifdef _WIN32
#undef near
#undef far
#endif

/**
 * OpenGL Camera class, handles perspective and ortho projections.
 */
class Camera : public SceneObject {
public:
  enum Type { Perspective, Ortho, Rect };

  Camera(const QString& name);

  /// Set the camera with screen size width x height
  void prepare(int width, int height);

  QVariantMap save() const;

  /// Load the camera from map
  void load(QVariantMap map);

  CameraPtr clone() const;

  /// Sets this camera to use orthographic matrix that fills the screen
  void setRect(float near = -1.0f, float far = 1.0f);

  float near() const { return m_near; }
  float far() const { return m_far; }

  float fov() const { return m_fov; }

  void rotate(QPointF diff);
  void translate(QPointF diff);
  void zoom(float diff);

  void setTarget(const Eigen::Vector3f& target);
  void setLocation(const Eigen::Vector3f& location);

  const Eigen::Vector3f location() const;

  Type type() const { return m_type; }

  void setPickDisplay(float x, float y);

  const Eigen::Projective3f& projection() const { return m_projection; }
  const Eigen::Affine3f& view() const { return m_view; }

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
protected:
  void updateVectors();

  Type m_type;

  /// The point the camera is looking at (Perspective)
  Eigen::Vector3f m_target;
  /// Camera up vector (Perspective)

  Eigen::Vector3f m_up;
  Eigen::Vector3f m_right;
  Eigen::Vector3f m_front;

  float m_dx, m_dy, m_dist;

  Eigen::Projective3f m_projection;
  Eigen::Affine3f m_view;

  /// Field of view, in degrees, in the y direction (Perspective)
  float m_fov;
  /// Near clipping plane distance
  float m_near;
  /// Far clipping plane distance
  float m_far;
};

#endif // CAMERA_HPP
