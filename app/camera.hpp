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

#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "forward.hpp"

#include <QVector3D>
#include <QVariant>

/**
 * OpenGL Camera class, handles perspective and ortho projections.
 */
class Camera {
public:
  enum Type { Perspective, Ortho, Rect };

  Camera(const QString& name);

  /// Set the camera with screen size width x height
  void prepare(int width, int height);

  /// Load the camera from map
  void load(QVariantMap map);

  /// Sets this camera to use orthographic matrix that fills the screen
  void setRect(float near = -1.0f, float far = 1.0f);

  QString name() const { return m_name; }

protected:
  QString m_name;
  Type m_type;

  /// Camera position
  QVector3D m_position;
  /// The point the camera is looking at (Perspective)
  QVector3D m_target;
  /// Camera up vector (Perspective)
  QVector3D m_up;

  /// Field of view, in degrees, in the y direction (Perspective)
  float m_fov;
  /// Near clipping plane distance
  float m_near;
  /// Far clipping plane distance
  float m_far;
};

#endif // CAMERA_HPP
