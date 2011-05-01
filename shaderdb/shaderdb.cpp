/**
 * Copyright 2010,2011 Riku Palomäki.
 * This file is part of GLSL Lab.
 *
 * GLSL Lab is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License Version 3 as
 * published by the Free Software Foundation.
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
#include "metainfo.hpp"
#include "scene.hpp"
#include "project.hpp"
#include "shader/program.hpp"
#include "shader/shader.hpp"

#include <parser.h>
#include <serializer.h>

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

void ShaderDB::addPath(QString path, bool make_primary) {
  QDir dir(path);
  if (make_primary)
    m_defaultPath = path;
  path = dir.canonicalPath();
  if (!path.isEmpty()) {
    m_paths << path;
    if (make_primary)
      m_defaultPath = path;
  }
}

QStringList ShaderDB::localProjects() {
  QSet<QString> files;

  foreach (QString path, m_paths)
    files += scanDir(path);

  QStringList ret = files.toList();
  return ret;
}

ScenePtr ShaderDB::newLocalProject(QString name, QString srcfile) {
  QDir dir;
  QString path = makeUniqPath(m_defaultPath, name);
  if (!dir.mkpath(path)) {
    Log::error("Failed to create %s", path.toUtf8().data());
    return ScenePtr();
  }
  dir.setPath(path);
  path = dir.canonicalPath();

  QString target = makeUniqPath(path, name + ".lab");

  ScenePtr scene = Scene::load(srcfile);
  if (!scene) return scene;

  scene->setFilename(target);
  MetaInfo& info = scene->metainfo();

  /// @todo use the real version. And shouldn't we use this version to parse
  ///       and interpret everything correctly?
  info.lab_version = "0.0.1";
  info.description = "Based on " + info.name;
  info.name = name;
  info.id = "";
  info.revision = 0;
  info.releases = QStringList() << "dev";
  info.servers.clear();
  info.categories.removeAll("example");
  info.categories.removeAll("built-in");
  info.categories.removeAll("user");
  info.categories << "user";

  /// @todo copy all file-based external 3D objects and texture images

  scene->setRoot(path);
  /// @todo
/*  foreach (ProgramPtr p, scene->shaders()) {
    foreach (ShaderPtr s, p->shaders()) {
      if (!s->filename().isEmpty()) {
        QFileInfo d(s->filename());
        QString target = path + "/" + d.fileName();
        QFile::copy(s->filename(), target);
        s->setFilename(target);
      }
    }
  }*/

  QJson::Serializer serializer;
  QFile file(target);
  const QByteArray str = serializer.serialize(scene->save());
  if (!str.isNull() && file.open(QIODevice::WriteOnly)) {
    file.write(str);
  }

  return scene;
}


ShaderDB & ShaderDB::instance() {
  if (!s_instance)
    new ShaderDB;
  assert(s_instance);
  return *s_instance;
}

QSet<QString> ShaderDB::scanDir(QString path, bool subdirs) {
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

QString ShaderDB::makeUniqPath(QString path, QString name) const {
  QString base = path + "/" + name.replace("/", "_");
  path = base;
  QDir dir;
  for (int i = 0; dir.exists(path); ++i)
    path = base + "-" + QString::number(i);

  return path;
}
