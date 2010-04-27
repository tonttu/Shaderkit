#ifndef LIGHT_HPP
#define LIGHT_HPP

#include "forward.hpp"

#include <QColor>
#include <QVector3D>

/**
 * OpenGL Light.
 */
class Light {
public:
  enum Type { Spot, Direction };

  /// @param name the object name, eg. "Main light"
  Light(const QString& name);

  /// Enables the light
  void activate(State& state);
  /// Disables the light
  void deactivate(State& state);

  /// Load the light from JSON Value
  void load(const Value& value);

protected:
  QString m_name;
  Type m_type;

  /// The id (beginning from 0) of the light. If it's negative, it will
  /// be chosen automatically in activate().
  int m_id;

  /// Light colors
  QColor m_ambient, m_diffuse, m_specular;

  /// if m_type == Spot, we control the light with position and the target vectors
  QVector3D m_position, m_target;

  /// With direction lights (m_type == Direction) there is only direction vector
  QVector3D m_direction;

  /// The maximum spread angle of a light source.
  float m_spot_cutoff;
};

#endif // LIGHT_HPP
