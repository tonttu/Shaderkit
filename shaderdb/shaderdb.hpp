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

#ifndef SHADERDB_HPP
#define SHADERDB_HPP

#include <QStringList>
#include <QSet>

class ShaderDB {
public:
  ShaderDB();
  virtual ~ShaderDB();

  void addPath(QString path);
  QStringList localProjects();

  static ShaderDB & instance();

protected:
  QSet<QString> scanDir(QString path, bool subdirs = true);
  QSet<QString> m_paths;

  static ShaderDB * s_instance;
};

#endif // SHADERDB_HPP
