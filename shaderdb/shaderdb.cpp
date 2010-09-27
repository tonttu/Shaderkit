/**
 * Copyright 2010 Riku Palomäki.
 * This file is part of GLSL Lab.
 *
 * GLSL Lab is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GLSL Lab is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GLSL Lab.  If not, see <http://www.gnu.org/licenses/>.
 */

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
