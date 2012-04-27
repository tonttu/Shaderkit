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

#include "metainfo.hpp"
#include "version.hpp"

#include "parser.h"

#include <QFile>
namespace Shaderkit
{
  QStringList getList(QVariantMap map, QString name)
  {
    QVariant var = map[name];
    if (var.type() == QVariant::StringList || var.type() == QVariant::List) {
      return var.toStringList();
    } else {
      QString tmp = var.toString();
      if (tmp.isEmpty() && var.type() != QVariant::String)
        return QStringList();
      return QStringList(tmp);
    }
  }

  QVariant putList(QStringList lst)
  {
    if (lst.size() == 1)
      return lst[0];
    else
      return lst;
  }

  MetaInfo::MetaInfo()
  {
  }

  QVariantMap MetaInfo::toMap() const
  {
    QVariantMap map;

    map["shaderkit version"] = Shaderkit::STR;
    if (!name.isEmpty()) map["name"] = name;
    if (!description.isEmpty()) map["description"] = description;
    //if (!id.isEmpty()) map["id"] = id;
    //map["revision"] = revision;

    //if (!releases.isEmpty()) map["release"] = putList(releases);
    //if (!servers.isEmpty()) map["server"] = putList(servers);
    if (!categories.isEmpty()) map["category"] = putList(categories);
    //if (!renderer.isEmpty()) map["renderer"] = putList(renderer);
    if (!licenses.isEmpty()) map["license"] = putList(licenses);
    if (!authors.isEmpty()) map["authors"] = putList(authors);

    QVariantMap dep;
    foreach (QString name, dependencies.keys()) {
      QPair<QString, int> d = dependencies[name];
      if (d.second == -1) {
        dep[name] = d.first;
      } else {
        QVariantList lst;
        lst << d.first << d.second;
        dep[name] = lst;
      }
    }
    if (!dep.isEmpty()) map["dependencies"] = dep;

    return map;
  }

  void MetaInfo::load(QVariantMap map)
  {
    shaderkit_version = map["shaderkit version"].toString();
    name = map["name"].toString();
    description = map["description"].toString();
    id = map["id"].toString();
    revision = map["revision"].toInt();
    releases = getList(map, "release");
    servers = getList(map, "server");
    categories = getList(map, "category");
    renderer = getList(map, "renderer");
    licenses = getList(map, "license");
    authors = getList(map, "authors");

    QVariantMap tmp = map["dependencies"].toMap();
    for (auto it = tmp.begin(); it != tmp.end(); ++it) {
      QVariantList lst = it.value().toList();
      if (lst.isEmpty()) {
        dependencies[it.key()] = qMakePair(it.value().toString(), -1);
      } else {
        dependencies[it.key()] = qMakePair(lst[0].toString(),
                                           lst.size() > 1 ? lst[1].toInt() : -1);
      }
    }
  }

  bool MetaInfo::ping(const QString& filename, MetaInfo& info)
  {
    QJson::Parser parser;
    bool ok = false;
    QFile file(filename);
    QVariant data = parser.parse(&file, &ok);
    if (ok) {
      info.load(data.toMap()["shaderkit"].toMap());
    }
    return ok;
  }

} // namespace Shaderkit
