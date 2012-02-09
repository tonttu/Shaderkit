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

#include "light.hpp"
#include "state.hpp"
#include "scene.hpp"
#include "opengl.hpp"

/// helper method to convert QColor to 4 element GLfloat array
inline void getColor(const QColor& color, GLfloat* tmp) {
  // color.getRgbF(tmp+0, tmp+1, tmp+2, tmp+3);
  tmp[0] = color.redF();
  tmp[1] = color.greenF();
  tmp[2] = color.blueF();
  tmp[3] = color.alphaF();
}

Light::Light(const QString& name)
  : SceneObject(name), m_type(Direction), m_id(-1),
  m_ambient(0, 0, 0, 255), m_diffuse(255, 255, 255, 255), m_specular(255, 255, 255, 255),
  m_direction(0, 0, 1), m_spot_cutoff(180) {}

void Light::activate(State& state) {
  if (m_id < 0) m_id = state.nextFreeLight();
  state.setLight(m_id, true);

  GLfloat tmp[4];

  getColor(m_ambient, tmp);
  glRun(glLightfv(GL_LIGHT0+m_id, GL_AMBIENT, tmp));

  getColor(m_diffuse, tmp);
  glRun(glLightfv(GL_LIGHT0+m_id, GL_DIFFUSE, tmp));

  getColor(m_specular, tmp);
  glRun(glLightfv(GL_LIGHT0+m_id, GL_SPECULAR, tmp));

  if (m_type == Spot) {
    tmp[0] = m_location.x(); tmp[1] = m_location.y(); tmp[2] = m_location.z();
    tmp[3] = 1.0f;
    glRun(glLightfv(GL_LIGHT0+m_id, GL_POSITION, tmp));

    QVector3D n = m_target - m_location;
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
  state.setLight(m_id, false);
  m_id = -1;
}

QVariantMap Light::toMap() const {
  QVariantMap map = SceneObject::toMap();
  if (m_type == Spot) {
    map["type"] = "spot";
    map["location"] = toList(m_location);
    map["target"] = toList(m_target);
    map["spot cutoff"] = m_spot_cutoff;
  } else if (m_type == Direction) {
    map["type"] = "direction";
    map["direction"] = toList(m_direction);
  }

  map["ambient"] = toList(m_ambient);
  map["diffuse"] = toList(m_diffuse);
  map["specular"] = toList(m_specular);

  return map;
}

void Light::load(QVariantMap map) {
  SceneObject::load(map);

  if (map["type"] == "spot") m_type = Spot;
  if (map["type"] == "direction") m_type = Direction;

  // At least Qt 4.7 saves color as "#RRGGBB" without alpha,
  // so we don't use native implementation.
  m_ambient = toColor(map["ambient"]);
  m_diffuse = toColor(map["diffuse"]);
  m_specular = toColor(map["specular"]);
  m_location = toVector(map["location"]);
  m_target = toVector(map["target"]);
  m_direction = toVector(map["direction"]);
  m_spot_cutoff = map["spot cutoff"].toFloat();
}

LightPtr Light::clone() const {
  return LightPtr(new Light(*this));
}

void Light::setType(Type t) {
  m_type = t;
}

void Light::setAmbient(const QColor& color) {
  m_ambient = color;
}

void Light::setDiffuse(const QColor& color) {
  m_diffuse = color;
}

void Light::setSpecular(const QColor& color) {
  m_specular = color;
}

void Light::setLocation(const QVector3D& loc) {
  m_location = loc;
  //m_type = Spot;
}

void Light::setTarget(const QVector3D& target) {
  m_target = target;
  //m_type = Spot;
}

void Light::setDirection(const QVector3D& dir) {
  m_direction = dir;
  m_type = Direction;
}

void Light::setSpotCutoff(float v) {
  m_spot_cutoff = v;
}
