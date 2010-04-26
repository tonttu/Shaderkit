#ifndef WATCHER_HPP
#define WATCHER_HPP

#include "forward.hpp"

#include <QFileSystemWatcher>
#include <QMap>
#include <QPair>

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
  void add(WatchablePtr obj, const QString& filename);

private:
  QFileSystemWatcher m_watcher;

  /// Every listening object have one of these in the m_objects map
  struct WatchableObj {
    WatchableObj(WatchablePtr obj, ino_t inode);
    WatchablePtr obj;
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
