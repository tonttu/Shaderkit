#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "forward.hpp"

#include <QVector3D>

/**
 * OpenGL Camera class, handles perspective and ortho projections.
 */
class Camera {
public:
  enum Type { Perspective, Ortho };

  Camera(const QString& name);

  /// Set the camera with screen size width x height
  void prepare(int width, int height);

  /// Load the light from JSON Value
  void load(const Value& value);

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
