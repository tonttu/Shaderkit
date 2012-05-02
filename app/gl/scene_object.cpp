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

#include "gl/scene_object.hpp"

namespace Shaderkit
{

  SceneObject::SceneObject(const QString& name)
    : m_name(name)
  {}

  SceneObject::SceneObject(const SceneObject& s)
    : m_name(s.m_name),
      m_ref(s.m_ref),
      m_ref_name(s.m_ref_name)
  {
  }

  SceneObject::~SceneObject() {}

  void SceneObject::setName(const QString& name)
  {
    if (m_name == name) return;
    m_name = name;
    attributeChanged();
  }

  QVariantMap SceneObject::toMap() const
  {
    QVariantMap map;
    if (!m_ref.isEmpty()) {
      QStringList tmp;
      tmp << m_ref;
      tmp += m_ref_name;
      map["ref"] = tmp;
    }
    return map;
  }

  void SceneObject::load(QVariantMap map)
  {
    QStringList tmp = map["ref"].toStringList();
    if (tmp.size() > 0)
      setRef(tmp[0], tmp.mid(1));
  }

  void SceneObject::setRef(const QString& import, const QString& refname)
  {
    setRef(import, QStringList(refname));
  }

  void SceneObject::setRef(const QString& import, const QStringList& refname)
  {
    if (import == m_ref && refname == m_ref_name) return;
    m_ref = import;
    m_ref_name = refname;
    attributeChanged();
  }

} // namespace Shaderkit
