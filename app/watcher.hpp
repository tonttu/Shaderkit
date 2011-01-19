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

#ifndef WATCHER_HPP
#define WATCHER_HPP

#include "forward.hpp"

#include <QFileSystemWatcher>
#include <QMap>
#include <QSet>

#include <sys/types.h>

/**
 * Watchable interface for every object that uses Watcher.
 */
class Watchable {
public:
  virtual void fileUpdated(const QString& filename) = 0;
};

/**
 * File system watcher, will call fileUpdated(file) method to changed objects.
 *
 * This is a singleton class.
 *
 * This class will also handle different save strategies different programs use
 * (semi-atomic rename to tmp-file etc).
 */
class Watcher : public QObject {
  Q_OBJECT

public:
  /// singleton instance
  static Watcher& instance();

  /// Adds a new object with filename to watchlist
  void add(Watchable* obj, const QString& filename);

private:
  QFileSystemWatcher m_watcher;

  /// Every listening object have one of these in the m_objects map
  struct WatchableObj {
    WatchableObj();
    WatchableObj(Watchable* obj, ino_t inode);
    Watchable* obj;
    ino_t inode; /// last known inode number for the file
  };
  typedef QMap<QString, WatchableObj> Objects;
  Objects m_objects;

  /// Set of all deleted files that we are expecting to reappear
  QSet<QString> m_waiting;

  /// directory -> set of all watched files
  QMap<QString, QSet<QString> > m_directories;

  Watcher();

  /// If the file is removed, we will start watching the whole directory if it
  /// might come back (like when most code editors save files atomically)
  /// @param filename the name of the wanted file
  void watchDir(const QString& filename);

  /// @param filename the name of the wanted file, not the directory
  void forgetDir(const QString& filename);

  static Watcher* s_instance;

private slots:
  /// Slot for QFileSystemWatcher
  void update(const QString& filename);
  /// Slot for QFileSystemWatcher
  void updateDir(const QString& dir);
};

#endif // WATCHER_HPP
