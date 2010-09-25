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
