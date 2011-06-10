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

#ifndef SCENE_OBJECT_HPP
#define SCENE_OBJECT_HPP

#include <QVariantMap>
#include <QString>
#include <QStringList>

class SceneObject {
public:
  SceneObject(const QString& name);
  virtual ~SceneObject();

  QString name() const { return m_name; }
  void setName(QString name);

  virtual QVariantMap save() const;
  virtual void load(QVariantMap map);

  void setRef(QString import, QString refname);
  QString ref();
  QStringList refName();

protected:
  QString m_name;
  QString m_ref;
  QStringList m_ref_name;
};

#endif // SCENE_OBJECT_HPP
