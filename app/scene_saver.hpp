#ifndef SCENE_SAVER_HPP
#define SCENE_SAVER_HPP

#include "forward.hpp"

#include <QDateTime>

class QTimer;

/// Saving happens after a timeout (m_timeout), but is also restricted by
/// another timeout (m_min_interval). However, if the user keeps using the
/// ui all the time and the first timeout seems never to be passed by,
/// there is a third timeout (m_max_wait).

///                     (not saving before that)        (must save before this)
///                                     v                                 v
/// saved   changed     changed         v           (saving?)             v
/// --x--------x-----------x------------x--------------x------------------x--->
///   ^        ^           ^            ^              ^                  ^
///   ^        ^           |...............m_timeout...|                  ^
///   ^        ^                        ^                                 ^
///   |...........m_min_interval........|                                 ^
///            ^                                                          ^
///            |......................m_max_wait..........................|

class SceneSaver : public QObject {
  Q_OBJECT

public:
  SceneSaver(Scene &scene);
  ~SceneSaver();

  bool enabled() const;

public slots:
  void sceneChanged();
  void stateChanged();

private slots:
  void trigger();

private:
  void updateTimer();

  /// "how long should be idle before triggering the saving process" (secs)
  float m_timeout;
  /// "how long minimum interval should be between two consecutive saves" (secs)
  float m_min_interval;
  /// "How long can we wait before forcing saving" (secs)
  float m_max_wait;

  Scene& m_scene;

  QTimer* m_timer;

  QDateTime m_last_changed;
  QDateTime m_last_saved;
  QDateTime m_first_changed;
};

#endif // SCENE_SAVER_HPP
