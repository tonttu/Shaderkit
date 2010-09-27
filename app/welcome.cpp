/**
 * Copyright 2010 Riku Palomäki.
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

#include "welcome.hpp"
#include "ui_welcome.h"
#include "shaderdb/shaderdb.hpp"
#include "shaderdb/metainfo.hpp"

#include <QDebug>

WelcomeButton::WelcomeButton(QWidget* parent, QString filename)
  : QCommandLinkButton(parent), m_filename(filename) {
  connect(this, SIGNAL(clicked()), SLOT(receiveClick()));
}

void WelcomeButton::receiveClick() {
  emit clicked(m_filename);
}


Welcome::Welcome(MainWindow & mainwindow) : QFrame(), m_ui(new Ui::Welcome),
    m_mainwindow(mainwindow) {
  m_ui->setupUi(this);

  connect(m_ui->open_button, SIGNAL(clicked()), this, SLOT(load()));

  QStringList files = ShaderDB::instance().localProjects();
  int count = 0;
  foreach (QString file, files) {
    MetaInfo info = MetaInfo::ping(file);

    if (info.categories.contains("example") && !info.name.isEmpty()) {
      WelcomeButton * btn = new WelcomeButton(m_ui->example_frame, file);
      m_ui->example_layout->insertWidget(0, btn);
      btn->setText(info.name);
      btn->setDescription(info.description);
      connect(btn, SIGNAL(clicked(QString)), this, SLOT(open(QString)));
      if (++count >= 4)
        break;
    }
  }
}

Welcome::~Welcome() {
  delete m_ui;
}

void Welcome::open(QString filename) {
  if (m_mainwindow.openProject(filename)) {
    hide();
    //deleteLater();
  }
}

void Welcome::load() {
  if (m_mainwindow.load())
    hide();
}
