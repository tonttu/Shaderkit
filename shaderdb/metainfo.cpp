#include "metainfo.hpp"

#include <qjson/parser.h>

#include <QFile>

QStringList getList(QVariantMap map, QString name) {
  QVariant var = map[name];
  if (var.type() == QVariant::StringList) {
    return var.toStringList();
  } else {
    QString tmp = var.toString();
    if (tmp.isEmpty() && var.type() != QVariant::String)
      return QStringList();
    return QStringList(tmp);
  }
}

MetaInfo::MetaInfo() {
}

void MetaInfo::load(QVariantMap map) {
  lab_version = map["lab version"].toString();
  name = map["name"].toString();
  description = map["description"].toString();
  id = map["id"].toString();
  revision = map["revision"].toInt();
  releases = getList(map, "release");
  servers = getList(map, "server");
  categories = getList(map, "category");
  licenses = getList(map, "license");
  authors = getList(map, "author");

  QVariantMap tmp = map["dependencies"].toMap();
  for (QVariantMap::iterator it = tmp.begin(); it != tmp.end(); ++it) {
    QVariantList lst = it.value().toList();
    if (lst.isEmpty()) {
      dependencies[it.key()] = qMakePair(it.value().toString(), -1);
    } else {
      dependencies[it.key()] = qMakePair(lst[0].toString(),
                                         lst.size() > 1 ? lst[1].toInt() : -1);
    }
  }
}

MetaInfo MetaInfo::ping(QString filename) {
  MetaInfo info;
  QJson::Parser parser;
  bool ok;
  QFile file(filename);
  QVariant data = parser.parse(&file, &ok);
  if (ok) {
    info.load(data.toMap()["lab"].toMap());
  }
  return info;
}
