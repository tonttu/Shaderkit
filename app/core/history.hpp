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

#ifndef HISTORY_HPP
#define HISTORY_HPP

#include "forward.hpp"

#include <QQueue>
#include <QVariantMap>
#include <QMutex>

class QTimer;

namespace Shaderkit
{

  class History : public QObject
  {
    Q_OBJECT

  public:
    History(Scene& scene, const QString& filename);
    void changed();
    void setSceneFilename(const QString& filename);

    void sync();

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

} // namespace Shaderkit

#endif // HISTORY_HPP
