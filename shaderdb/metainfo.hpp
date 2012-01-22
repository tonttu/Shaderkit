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

#ifndef METAINFO_HPP
#define METAINFO_HPP

#include <QVariantMap>
#include <QStringList>

class MetaInfo
{
public:
  MetaInfo();

  QVariantMap save() const;
  void load(QVariantMap map);

  static bool ping(const QString& filename, MetaInfo& info);

  QString shaderkit_version;
  QString name;
  QString description;
  QString id;
  int revision;
  QStringList releases;
  QStringList servers;
  QStringList categories;
  QMap<QString, QPair<QString, int>> dependencies;
  QStringList licenses;
  QStringList authors;
  QStringList renderer;
};

#endif // METAINFO_HPP
