#include "shaderdb.hpp"

#include <QDebug>
#include <QDir>

#include <cassert>

ShaderDB * ShaderDB::s_instance = 0;

ShaderDB::ShaderDB() {
  if (!s_instance)
    s_instance = this;
}

ShaderDB::~ShaderDB() {
  if (s_instance == this) {
    s_instance = 0;
  }
}

void ShaderDB::addPath(QString path) {
  QDir dir(path);
  path = dir.canonicalPath();
  if (!path.isEmpty())
    m_paths << path;
}

QStringList ShaderDB::localProjects() {
  QSet<QString> files;

  foreach (QString path, m_paths)
    files += scanDir(path);

  QStringList ret = files.toList();
  return ret;
}

ShaderDB & ShaderDB::instance() {
  if (!s_instance)
    new ShaderDB;
  assert(s_instance);
  return *s_instance;
}

QSet<QString> ShaderDB::scanDir(QString path, bool subdirs)
{
  QSet<QString> ret;

  QStringList search;
  search << "*.lab" << "*.zip";

  QDir::Filters filters = QDir::NoDotAndDotDot | QDir::Files;
  if (subdirs)
    filters |= QDir::AllDirs;

  QDir dir(path);
  QFileInfoList lst = dir.entryInfoList(search, filters);

  foreach (QFileInfo info, lst) {
    if (info.isDir() && subdirs) {
      ret += scanDir(info.absoluteFilePath(), false);
    } else if (info.isFile()) {
      QString name = info.canonicalFilePath();
      if (!name.isEmpty())
        ret << name;
    }
  }

  return ret;
}
