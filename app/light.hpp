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
#include "scene_object.hpp"

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

  virtual QVariantMap save() const;

  /// Load the light from map
  virtual void load(QVariantMap map);

  LightPtr clone() const;

  void setAmbient(const QColor& color);
  void setDiffuse(const QColor& color);
  void setSpecular(const QColor& color);
  void setDirection(const QVector3D& dir);

protected:
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
