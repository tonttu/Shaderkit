#ifndef METAINFO_HPP
#define METAINFO_HPP

#include <QVariantMap>
#include <QStringList>

class MetaInfo
{
public:
  MetaInfo();
  void load(QVariantMap map);

  static MetaInfo ping(QString filename);

  QString lab_version;
  QString name;
  QString description;
  QString id;
  int revision;
  QStringList releases;
  QStringList servers;
  QStringList categories;
  QMap<QString, QPair<QString, int> > dependencies;
  QStringList licenses;
  QStringList authors;
};

#endif // METAINFO_HPP
