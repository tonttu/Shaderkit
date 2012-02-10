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

#ifndef SHADERDB_HPP
#define SHADERDB_HPP

#include "forward.hpp"

#include <QStringList>
#include <QSet>

class QFile;

class ShaderDB {
public:
  ShaderDB();
  virtual ~ShaderDB();

  void addPath(QString path, bool make_primary = false);
  QStringList localScenes();

  ScenePtr newLocalScene(QString name, QString srcfile);

  QString defaultPath() const { return m_defaultPath; }

  static bool findLimbo(MetaInfo& info, QString& filename);
  static bool openNewLimbo(QFile& file, const QString& name);

  static ShaderDB & instance();

protected:
  QSet<QString> scanDir(QString path, bool subdirs = true);
  QString makeUniqPath(QString path, QString name) const;

  QSet<QString> m_paths;
  QString m_defaultPath;

  static ShaderDB * s_instance;
};

#endif // SHADERDB_HPP
