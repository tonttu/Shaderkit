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
