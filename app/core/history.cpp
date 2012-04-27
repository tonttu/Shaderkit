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

#include "core/history.hpp"
#include "core/log.hpp"
#include "core/scene.hpp"

#include <QTimer>
#include <QThread>
#include <QDebug>
#include <QFile>
#include <QtConcurrentRun>
#include <QtEndian>
#include <QBuffer>

namespace
{
  const char s_magic[] = "\nShdrktHstr\x5b\xf2\x48\xda\xbd\x27\x72\xb7\n";
}

namespace Shaderkit
{

  History::History(Scene& scene, const QString& filename)
    : m_scene(scene), m_timer(new QTimer(this)),
      m_historyEnabled(false)
  {
    setSceneFilename(filename);
    m_timer->setSingleShot(true);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(save()));
  }

  void History::setSceneFilename(const QString& filename)
  {
    // this could block a long time, but better to do that than break everything
    QMutexLocker writerLock(&m_writerMutex);
    m_filename = filename + ".history";
    changed();
  }

  void History::changed()
  {
    m_timer->start(5000);
  }

  void History::sync()
  {
    /// @todo this and save should be combined

    bool automatic = m_scene.automaticSaving();

    if (!automatic && !m_historyEnabled) return;

    QVariantMap scene = m_scene.toMap();

    if (automatic) {
      m_scene.save(scene);
    }

    if (!m_historyEnabled) {
      QVariantMap state;
      state["scene"] = scene;
      state["time"] = QDateTime::currentDateTime().toUTC();
      // Qt 4.7 only: QDateTime::currentDateTimeUtc();

      {
        QMutexLocker lock(&m_queueMutex);
        m_queue << state;
      }
      commit();
    }
  }

  void History::save()
  {
    bool automatic = m_scene.automaticSaving();

    if (!automatic && !m_historyEnabled) return;

    QVariantMap scene = m_scene.toMap();

    if (automatic) {
      m_scene.save(scene);
    }

    if (!m_historyEnabled) {
      QVariantMap state;
      state["scene"] = scene;
      state["time"] = QDateTime::currentDateTime().toUTC();
      /// @todo save all changed shader files

      QMutexLocker lock(&m_queueMutex);
      m_queue << state;
      QtConcurrent::run(std::bind(&History::commit, this));
    }
  }

  void History::commit()
  {
    /// things read from the queue have to be written in order, that's why the lock
    QMutexLocker writerLock(&m_writerMutex);
    for (;;) {
      QVariantMap state;
      {
        QMutexLocker lock(&m_queueMutex);
        if (m_queue.isEmpty()) return;
        state = m_queue.takeFirst();
      }

      if (!state.isEmpty()) {
        QByteArray buffer_array;
        QBuffer buffer(&buffer_array);
        buffer.open(QIODevice::WriteOnly);

        QDataStream out(&buffer);
        out << state;

        const QByteArray data = qCompress(buffer_array);

        QFile file(m_filename);
        if (file.open(QFile::WriteOnly | QFile::Append)) {
          qint32 len = qToBigEndian(data.length());
          file.write(s_magic, sizeof(s_magic));
          file.write((const char*)&len, sizeof(len));
          file.write(data);
        }
      }
    }
  }

} // namespace Shaderkit
