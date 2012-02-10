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

#include "core/scene_saver.hpp"
#include "core/scene.hpp"

#include <QTimer>

SceneSaver::SceneSaver(Scene& scene)
  : m_scene(scene),
    m_timeout(5.0f),
    m_min_interval(15.0f),
    m_max_wait(60.0f),
    m_timer(new QTimer(this))
{
  m_timer->setSingleShot(true);
  connect(m_timer, SIGNAL(timeout()), this, SLOT(trigger()));
}

SceneSaver::~SceneSaver() {
}

void SceneSaver::sceneChanged() {
  m_last_changed = QDateTime::currentDateTime();
  if (m_first_changed.isNull())
    m_first_changed = m_last_changed;
  if (enabled()) updateTimer();
}

void SceneSaver::stateChanged() {
  if (enabled()) updateTimer();
}

bool SceneSaver::enabled() const {
  return m_scene.isChanged() && m_scene.automaticSaving() && m_scene.state() != Scene::ReadOnly;
}

void SceneSaver::trigger() {
  if (!enabled()) return;

  QVariantMap scene = m_scene.toMap();
  if (m_scene.automaticSaving())
    m_scene.save(scene);

  /// @todo history

  m_last_changed = QDateTime();
  m_last_saved = QDateTime::currentDateTime();
  m_first_changed = QDateTime();
}

void SceneSaver::updateTimer() {
  if (m_timeout <= 0 || m_last_changed.isNull()) return;

  QDateTime target = m_last_changed.addSecs(m_timeout);

  if (m_min_interval > 0 && m_last_saved.isValid()) {
    QDateTime min = m_last_saved.addSecs(m_min_interval);
    target = std::max(target, min);
  }

  if (m_max_wait > 0 && m_first_changed.isValid()) {
    QDateTime max = m_first_changed.addSecs(m_max_wait);
    target = std::min(target, max);
  }

  int delay = QDateTime::currentDateTime().secsTo(target);
  if (delay <= 0) m_timer->start(1);
  else m_timer->start(delay * 1000);
}
