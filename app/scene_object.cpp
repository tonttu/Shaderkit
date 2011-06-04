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

#include "scene_object.hpp"

SceneObject::SceneObject(const QString& name) : m_name(name) {}
SceneObject::~SceneObject() {}

void SceneObject::setName(QString name) {
  m_name = name;
}

QVariantMap SceneObject::save() const {
  QVariantMap map;
  if (!m_ref.isEmpty()) {
    QStringList tmp;
    tmp << m_ref;
    tmp += m_ref_name;
    map["ref"] = tmp;
  }
  return map;
}

void SceneObject::load(QVariantMap map) {
  QStringList tmp = map["ref"].toStringList();
  if (tmp.size() > 0) {
    m_ref = tmp[0];
    m_ref_name = tmp.mid(1);
  }
}

void SceneObject::setRef(QString import, QStringList iname) {
  m_ref = import;
  m_ref_name = iname;
}

QString SceneObject::ref() {
  return m_ref;
}

QStringList SceneObject::refName() {
  return m_ref_name;
}
