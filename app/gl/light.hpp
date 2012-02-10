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

#ifndef LIGHT_HPP
#define LIGHT_HPP

#include "forward.hpp"
#include "gl/scene_object.hpp"

#include <QColor>
#include <QVector3D>
#include <QVariant>

/**
 * OpenGL Light.
 */
class Light : public SceneObject {
public:
  enum Type { Spot, Direction };

  /// @param name the object name, eg. "Main light"
  Light(const QString& name);

  /// Enables the light
  void activate(State& state);
  /// Disables the light
  void deactivate(State& state);

  virtual QVariantMap toMap() const;

  /// Load the light from map
  virtual void load(QVariantMap map);

  LightPtr clone() const;

  void setType(Type t);
  Type type() const { return m_type; }

  void setAmbient(const QColor& color);
  void setDiffuse(const QColor& color);
  void setSpecular(const QColor& color);
  const QColor& ambient() const { return m_ambient; }
  const QColor& diffuse() const { return m_diffuse; }
  const QColor& specular() const { return m_specular; }

  const QVector3D location() const { return m_location; }
  void setLocation(const QVector3D& loc);

  const QVector3D target() const { return m_target; }
  void setTarget(const QVector3D& target);

  void setDirection(const QVector3D& dir);
  const QVector3D direction() const { return m_direction; }

  float spotCutoff() const { return m_spot_cutoff; }
  void setSpotCutoff(float v);

protected:
  Type m_type;

  /// The id (beginning from 0) of the light. If it's negative, it will
  /// be chosen automatically in activate().
  int m_id;

  /// Light colors
  QColor m_ambient, m_diffuse, m_specular;

  /// if m_type == Spot, we control the light with location and the target vectors
  QVector3D m_location, m_target;

  /// With direction lights (m_type == Direction) there is only direction vector
  QVector3D m_direction;

  /// The maximum spread angle of a light source, if m_type == Spot.
  float m_spot_cutoff;
};

#endif // LIGHT_HPP
