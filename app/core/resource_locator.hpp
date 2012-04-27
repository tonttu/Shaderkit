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

#ifndef RESOURCE_LOCATOR_HPP
#define RESOURCE_LOCATOR_HPP

#include <QAbstractFileEngineHandler>
#include <QMap>

namespace Shaderkit {

class ResourceLocator : public QAbstractFileEngineHandler
{
public:
  ResourceLocator();
  virtual ~ResourceLocator();

  QAbstractFileEngine* create(const QString& fileName) const;

  static void setPath(const QString& prefix, const QString& path);
  static QString ui(const QString& filename);
  static QString rename(const QString& src, const QString& new_base, const QSet<QString>& lst);
  /// Makes sure that src is a file name that isn't included in list
  /// and doesn't exist on file system
  static QString unique(const QString& src, const QSet<QString>& list);

  static void pushPath(const QString& prefix, const QString& opath);
  static void popPath(const QString& prefix, const QString& opath);

  class Path {
  public:
    Path(const QString& prefix, const QString& path)
      : m_prefix(prefix), m_path(path) {
      ResourceLocator::pushPath(prefix, path);
    }

    ~Path() {
      ResourceLocator::popPath(m_prefix, m_path);
    }

  private:
    const QString m_prefix, m_path;
  };

private:
  QMap<QString, QString> m_paths;
  QMap<QString, QStringList> m_pathsOverride;
};

} // namespace Shaderkit

#endif // RESOURCE_LOCATOR_HPP
