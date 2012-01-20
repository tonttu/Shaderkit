/**
 * Copyright 2010,2011 Riku Palomäki.
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

#include "welcome.hpp"
#include "new_wizard.hpp"
#include "ui_welcome.h"
#include "shaderdb/shaderdb.hpp"
#include "shaderdb/metainfo.hpp"
#include "scene.hpp"

#include <QFileInfo>
#include <QDateTime>
#include <QDebug>

#include <queue>

WelcomeButton::WelcomeButton(QWidget* parent, QString filename)
  : QCommandLinkButton(parent), m_filename(filename) {
  connect(this, SIGNAL(clicked()), SLOT(receiveClick()));
  setMinimumHeight(sizeHint().height());
  setToolTip(filename);
}

void WelcomeButton::setFilename(QString filename) {
  m_filename = filename;
}

void WelcomeButton::receiveClick() {
  emit clicked(m_filename);
}


Welcome::Welcome() : QFrame(), m_ui(new Ui::Welcome) {
  m_ui->setupUi(this);

  connect(m_ui->open_button, SIGNAL(clicked()), this, SLOT(load()));
  connect(m_ui->new_button, SIGNAL(clicked()), this, SLOT(newScene()));

  QStringList files = ShaderDB::instance().localScenes();
  int count = 0;
  typedef QPair<QDateTime, QString> V;
  std::priority_queue<V, std::vector<V>> recent;

  foreach (QString file, files) {
    MetaInfo info = MetaInfo::ping(file);
    if (info.name.isEmpty()) continue;

    if (info.categories.contains("example") && count++ < 4) {
      WelcomeButton * btn = new WelcomeButton(m_ui->example_frame, file);
      btn->setIcon(QIcon(":/icons/project_hl.png"));
      m_ui->example_layout->insertWidget(0, btn);
      btn->setText(info.name);
      btn->setDescription(info.description);
      connect(btn, SIGNAL(clicked(QString)), this, SLOT(openExample(QString)));
    } else if (info.categories.contains("user")) {
      QFileInfo finfo(file);
      recent.push(qMakePair(finfo.lastModified(), file));
    }
  }
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());

  for (count = 0; count < 4 && !recent.empty(); ++count) {
    const V& v = recent.top();

    MetaInfo info = MetaInfo::ping(v.second);
    WelcomeButton * btn = new WelcomeButton(m_ui->recent_frame, v.second);
    btn->setIcon(QIcon(":/icons/project_hl.png"));
    m_ui->recent_layout->insertWidget(count, btn);
    btn->setText(info.name);
    btn->setDescription(info.description);
    connect(btn, SIGNAL(clicked(QString)), this, SLOT(open(QString)));

    recent.pop();
  }
  if (count) {
    m_ui->recent_label->setEnabled(true);
  }
}

Welcome::~Welcome() {
  delete m_ui;
}

void Welcome::openExample(QString filename) {
  if (MainWindow::instance().openScene(Scene::load(filename, Scene::ReadOnly))) {
    deleteLater();
  }
}

void Welcome::open(QString filename) {
  if (MainWindow::instance().openScene(Scene::load(filename, Scene::Ok))) {
    deleteLater();
  }
}

void Welcome::load() {
  if (MainWindow::instance().load())
    deleteLater();
}

void Welcome::newScene() {
  NewWizard* w = new NewWizard(this);
  connect(w, SIGNAL(rejected()), this, SLOT(show()));
  connect(w, SIGNAL(accepted()), this, SLOT(deleteLater()));

  w->show();
  hide();
}
