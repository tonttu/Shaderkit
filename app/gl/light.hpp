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

#ifndef LIGHT_HPP
#define LIGHT_HPP

#include "forward.hpp"

#include "core/attribute.hpp"
#include "core/color.hpp"

#include "gl/scene_object.hpp"

#include <QVariant>

namespace Shaderkit
{

  /**
   * OpenGL Light.
   */
  class Light : public QObject, public std::enable_shared_from_this<Light>,
      public SceneObject
  {
    Q_OBJECT

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
    Type type() const { return m_type.value(); }

    void setAmbient(const Color& color);
    void setDiffuse(const Color& color);
    void setSpecular(const Color& color);
    const Color& ambient() const { return m_ambient.value(); }
    const Color& diffuse() const { return m_diffuse.value(); }
    const Color& specular() const { return m_specular.value(); }

    const Eigen::Vector3f& location() const { return m_location.value(); }
    void setLocation(const Eigen::Vector3f& loc);

    const Eigen::Vector3f& target() const { return m_target.value(); }
    void setTarget(const Eigen::Vector3f& target);

    void setDirection(const Eigen::Vector3f& dir);
    const Eigen::Vector3f& direction() const { return m_direction.value(); }

    float spotCutoff() const { return m_spot_cutoff.value(); }
    void setSpotCutoff(float v);

    virtual void attributeChanged();

  signals:
    void changed(LightPtr);

  protected:
    explicit Light(const Light& l);

  protected:
    Attribute<Type> m_type;

    /// The id (beginning from 0) of the light. If it's negative, it will
    /// be chosen automatically in activate().
    int m_id;

    /// Light colors
    Attribute<Color> m_ambient, m_diffuse, m_specular;

    /// if m_type == Spot, we control the light with location and the target vectors
    Attribute<Eigen::Vector3f> m_location, m_target;

    /// With direction lights (m_type == Direction) there is only direction vector
    Attribute<Eigen::Vector3f> m_direction;

    /// The maximum spread angle of a light source, if m_type == Spot.
    Attribute<float> m_spot_cutoff;
  };

} // namespace Shaderkit

#endif // LIGHT_HPP
