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

#ifndef WELCOME_HPP
#define WELCOME_HPP

#include "mainwindow.hpp"

#include <QFrame>
#include <QCommandLinkButton>

namespace Ui {
class Welcome;
}

class WelcomeButton : public QCommandLinkButton {
  Q_OBJECT

public:
  explicit WelcomeButton(QWidget* parent, QString filename);
  virtual ~WelcomeButton() {}

signals:
  void clicked(QString filename);

protected slots:
  void receiveClick();

protected:
  QString m_filename;
};

class Welcome : public QFrame {
  Q_OBJECT

public:
  explicit Welcome(MainWindow & mainwindow);
  virtual ~Welcome();

public slots:
  void open(QString filename);
  void load();

private:
  Ui::Welcome* m_ui;
  MainWindow & m_mainwindow;
};

#endif // WELCOME_HPP
