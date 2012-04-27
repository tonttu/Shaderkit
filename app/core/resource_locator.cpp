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

#include "core/resource_locator.hpp"
#include "core/log.hpp"

#include <cassert>

#include <QSet>

namespace
{
  Shaderkit::ResourceLocator* s_instance = 0;
}

namespace Shaderkit
{

  ResourceLocator::ResourceLocator()
  {
    if (!s_instance) s_instance = this;
  }

  ResourceLocator::~ResourceLocator()
  {
    if (s_instance == this) s_instance = 0;
  }

  void ResourceLocator::setPath(const QString& prefix, const QString& opath)
  {
    if (!s_instance) return;
    QString path = opath;
    while (path.endsWith('/'))
      path.truncate(path.length()-1);
    assert(!path.startsWith('$'));
    s_instance->m_paths[prefix] = path;
  }

  void ResourceLocator::pushPath(const QString& prefix, const QString& opath)
  {
    if (!s_instance) return;
    s_instance->m_pathsOverride[prefix] << opath;
  }

  void ResourceLocator::popPath(const QString& prefix, const QString& opath)
  {
    if (!s_instance) return;
    QStringList list = s_instance->m_pathsOverride.value(prefix);
    if (!list.isEmpty()) {
      for (int i = list.size() - 1; i >= 0; --i) {
        if (list[i] == opath) {
          list.removeAt(i);
          if (list.isEmpty())
            s_instance->m_pathsOverride.remove(prefix);
          else
            s_instance->m_pathsOverride[prefix] = list;
          return;
        }
      }
    }
    Log::error("ResourceLocator::popPath - Failed to find path %s %s",
               prefix.toUtf8().data(), opath.toUtf8().data());
  }

  QAbstractFileEngine* ResourceLocator::create(const QString& fileName) const
  {
    QRegExp re("^\\$([^/]+)(/.*)$");
    if (re.exactMatch(fileName)) {
      QStringList paths = m_pathsOverride.value(re.cap(1));
      if (!paths.isEmpty()) {
        return QAbstractFileEngine::create(paths.last() + re.cap(2));
      }
      QString path = m_paths.value(re.cap(1));
      if (!path.isEmpty()) {
        return QAbstractFileEngine::create(path + re.cap(2));
      }
    }
    return 0;
  }

  QString ResourceLocator::ui(const QString& filename)
  {
    int idx = filename.lastIndexOf('/');
    if (idx == -1) return "";
    return filename.mid(idx+1);
  }

  QString ResourceLocator::rename(const QString& src, const QString& new_base, const QSet<QString>& lst)
  {
    QFileInfo fi(src), fi2(new_base);
    return unique(fi.path() + "/" + fi2.fileName(), lst);
  }

  QString ResourceLocator::unique(const QString& src, const QSet<QString>& lst)
  {
    QFileInfo fi(src);
    QString start = fi.path() + "/" + fi.baseName();
    QString suffix = fi.completeSuffix().isEmpty() ? "" : "." + fi.completeSuffix();

    QString file = src;
    QFileInfo fi2(file);
    int i = -1;
    while (QFile::exists(file) || lst.contains(fi2.absoluteFilePath())) {
      if (i == -1) {
        QRegExp re("(\\d+)$");
        int p = re.indexIn(start);
        if (p >= 0) {
          start = start.left(p);
          i = re.cap(1).toInt() + 1;
        } else {
          i = 1;
        }
      }

      file = start + QString::number(i++) + suffix;
      fi2.setFile(file);
    }

    return file;
  }

} // namespace Shaderkit
