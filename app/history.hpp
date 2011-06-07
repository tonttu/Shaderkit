#ifndef HISTORY_HPP
#define HISTORY_HPP

#include "forward.hpp"

#include <QQueue>
#include <QVariantMap>
#include <QMutex>

class QTimer;

class History : public QObject {
  Q_OBJECT

public:
  History(Scene& scene, const QString& filename);
  void changed();
  void setSceneFilename(const QString& filename);

private slots:
  void save();
  void commit();

private:
  Scene& m_scene;
  QString m_filename;
  QTimer* m_timer;

  QQueue<QVariantMap> m_queue;
  QMutex m_queueMutex;

  QMutex m_writerMutex;

  bool m_historyEnabled;
};

#endif // HISTORY_HPP
