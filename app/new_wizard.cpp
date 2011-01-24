#include "new_wizard.hpp"
#include "ui_new_wizard.h"
#include "shaderdb/shaderdb.hpp"
#include "shaderdb/metainfo.hpp"
#include "welcome.hpp"

NewWizard::NewWizard(QWidget* parent)
  : QDialog(parent),
  m_ui(new Ui::NewWizard) {
  m_ui->setupUi(this);

  QButtonGroup* group = new QButtonGroup(this);
  group->setExclusive(true);
  group->addButton(m_ui->default_project);
  group->addButton(m_ui->empty_project);

  QStringList files = ShaderDB::instance().localProjects();
  foreach (QString file, files) {
    MetaInfo info = MetaInfo::ping(file);

    if (!info.name.isEmpty()) {
      WelcomeButton * btn = new WelcomeButton(m_ui->list, file);
      btn->setCheckable(true);
      group->addButton(btn);
      //btn->setAutoExclusive(true);
      btn->setIcon(m_ui->default_project->icon());
      m_ui->list_layout->insertWidget(0, btn);
      btn->setText(info.name);
      btn->setDescription(info.description);
      //connect(btn, SIGNAL(clicked(QString)), this, SLOT(open(QString)));
    }
  }
}

NewWizard::~NewWizard() {
  delete m_ui;
}
