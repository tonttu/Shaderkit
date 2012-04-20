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

#include "gl/light.hpp"
#include "gl/state.hpp"
#include "gl/opengl.hpp"
#include "core/utils.hpp"

Light::Light(const QString& name)
  : SceneObject(name), m_type(Direction), m_id(-1),
  m_ambient(0, 0, 0, 255), m_diffuse(255, 255, 255, 255), m_specular(255, 255, 255, 255),
  m_direction(0, 0, 1), m_spot_cutoff(180) {}

void Light::activate(State& state) {
  if (m_id < 0) m_id = state.nextFreeLight();
  state.setLight(m_id, this);

  GLfloat tmp[4];

  glRun(glLightfv(GL_LIGHT0+m_id, GL_AMBIENT, m_ambient.data()));

  glRun(glLightfv(GL_LIGHT0+m_id, GL_DIFFUSE, m_diffuse.data()));

  glRun(glLightfv(GL_LIGHT0+m_id, GL_SPECULAR, m_specular.data()));

  if (m_type == Spot) {
    tmp[0] = m_location.x(); tmp[1] = m_location.y(); tmp[2] = m_location.z();
    tmp[3] = 1.0f;
    glRun(glLightfv(GL_LIGHT0+m_id, GL_POSITION, tmp));

    Eigen::Vector3f n = m_target - m_location;
    n.normalize();

    tmp[0] = n.x(); tmp[1] = n.y(); tmp[2] = n.z();
    glRun(glLightfv(GL_LIGHT0+m_id, GL_SPOT_DIRECTION, tmp));

    glRun(glLightf(GL_LIGHT0+m_id, GL_SPOT_CUTOFF, m_spot_cutoff));
  } else {
    tmp[0] = m_direction.x(); tmp[1] = m_direction.y(); tmp[2] = m_direction.z();
    tmp[3] = 0.0f;
    glRun(glLightfv(GL_LIGHT0+m_id, GL_POSITION, tmp));
  }
}

void Light::deactivate(State& state) {
  state.setLight(m_id, 0);
  m_id = -1;
}

QVariantMap Light::toMap() const {
  QVariantMap map = SceneObject::toMap();
  if (m_type == Spot) {
    map["type"] = "spot";
    map["location"] = Utils::toList(m_location);
    map["target"] = Utils::toList(m_target);
    map["spot cutoff"] = m_spot_cutoff;
  } else if (m_type == Direction) {
    map["type"] = "direction";
    map["direction"] = Utils::toList(m_direction);
  }

  map["ambient"] = m_ambient.toQVariant();
  map["diffuse"] = m_diffuse.toQVariant();
  map["specular"] = m_specular.toQVariant();

  return map;
}

void Light::load(QVariantMap map) {
  SceneObject::load(map);

  if (map["type"] == "spot") m_type = Spot;
  if (map["type"] == "direction") m_type = Direction;

  // At least Qt 4.7 saves color as "#RRGGBB" without alpha,
  // so we don't use native implementation.
  m_ambient = Color::fromQVariant(map["ambient"]);
  m_diffuse = Color::fromQVariant(map["diffuse"]);
  m_specular = Color::fromQVariant(map["specular"]);
  m_location = Utils::toVector3(map["location"]);
  m_target = Utils::toVector3(map["target"]);
  m_direction = Utils::toVector3(map["direction"]);
  m_spot_cutoff = map["spot cutoff"].toFloat();
}

LightPtr Light::clone() const {
  return LightPtr(new Light(*this));
}

void Light::setType(Type t) {
  m_type = t;
}

void Light::setAmbient(const Color& color) {
  m_ambient = color;
}

void Light::setDiffuse(const Color& color) {
  m_diffuse = color;
}

void Light::setSpecular(const Color& color) {
  m_specular = color;
}

void Light::setLocation(const Eigen::Vector3f& loc) {
  m_location = loc;
  //m_type = Spot;
}

void Light::setTarget(const Eigen::Vector3f& target) {
  m_target = target;
  //m_type = Spot;
}

void Light::setDirection(const Eigen::Vector3f& dir) {
  m_direction = dir;
  m_type = Direction;
}

void Light::setSpotCutoff(float v) {
  m_spot_cutoff = v;
}
