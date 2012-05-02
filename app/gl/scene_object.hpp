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

#ifndef SCENE_OBJECT_HPP
#define SCENE_OBJECT_HPP

#include <QVariantMap>
#include <QString>
#include <QStringList>

namespace Shaderkit
{
  class SceneObject
  {
  public:
    SceneObject(const QString& name);
    virtual ~SceneObject();

    const QString& name() const { return m_name; }
    void setName(const QString& name);

    virtual QVariantMap toMap() const;
    virtual void load(QVariantMap map);

    void setRef(const QString& import, const QString& refname);
    void setRef(const QString& import, const QStringList& refname);

    const QString& ref() { return m_ref; }
    const QStringList& refName() { return m_ref_name; }

    virtual void attributeChanged() = 0;

  protected:
    explicit SceneObject(const SceneObject& s);

  private:
    QString m_name;
    QString m_ref;
    QStringList m_ref_name;
  };

} // namespace Shaderkit

#endif // SCENE_OBJECT_HPP
