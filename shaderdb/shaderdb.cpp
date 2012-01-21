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

#include "shaderdb.hpp"
#include "metainfo.hpp"
#include "scene.hpp"
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

QStringList ShaderDB::localScenes() {
  QSet<QString> files;

  foreach (QString path, m_paths)
    files += scanDir(path);

  QStringList ret = files.toList();
  return ret;
}

ScenePtr ShaderDB::newLocalScene(QString name, QString srcfile) {
/*
  QDir dir;
  QString path = makeUniqPath(m_defaultPath, name);
  if (!dir.mkpath(path)) {
    Log::error("Failed to create %s", path.toUtf8().data());
    return ScenePtr();
  }
  dir.setPath(path);
  path = dir.canonicalPath();

  QString target = makeUniqPath(path, name + ".shaderkit");
*/
  ScenePtr scene = srcfile.isEmpty() ? ScenePtr(new Scene()) : Scene::load(srcfile, Scene::New);
  if (!scene) return scene;

  scene->setFilename("");
  MetaInfo& info = scene->metainfo();

  /// @todo use the real version. And shouldn't we use this version to parse
  ///       and interpret everything correctly?
  info.shaderkit_version = "0.0.1";
  if (!info.name.isEmpty())
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

  //scene->setRoot(path);
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

  /*QJson::Serializer serializer;
  QFile file(target);
  const QByteArray str = serializer.serialize(scene->save());
  if (!str.isNull() && file.open(QIODevice::WriteOnly)) {
    file.write(str);
  }*/

  return scene;
}

bool ShaderDB::openNewLimbo(QFile& file, const QString& name) {
  QString out = name;
  out.replace('/', '-');
  if(out.isEmpty()) out = "empty";
  for (int i = 0; i < 10; ++i) {
    QString d = QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss");
    QString path = instance().defaultPath() + "/limbo-" + d + "-" + QString::number(i);
    if (QFile::exists(path)) continue;
    if (!QDir().mkpath(path)) continue;
    QString filename = path + "/" + out + ".shaderkit";
    file.setFileName(filename);
    if (file.open(QFile::WriteOnly))
      return true;
  }
  return false;
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
  search << "*.shaderkit" << "*.zip";

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
