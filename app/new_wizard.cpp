#include "new_wizard.hpp"
#include "ui_new_wizard.h"
#include "shaderdb/shaderdb.hpp"
#include "shaderdb/metainfo.hpp"
#include "welcome.hpp"

#include <QKeyEvent>
#include <QPainter>

#include <cassert>

ToggleButton::ToggleButton(QWidget* parent) : QCheckBox(parent) {
}

void ToggleButton::paintEvent(QPaintEvent*) {
  QSize size = sizeHint();
  QPainter painter(this);
  QPixmap p = icon().pixmap(16, QIcon::Normal, isChecked() ? QIcon::On : QIcon::Off);
  int y = (size.height() - 16) / 2;
  painter.drawPixmap(0, y, p);
  painter.drawText(QRectF(20, 0, size.width()-20, size.height()), text(),
                   QTextOption(Qt::AlignLeft | Qt::AlignVCenter));
}

NewWizard::NewWizard(QWidget* parent)
  : QDialog(parent),
    m_ui(new Ui::NewWizard),
    m_group(new QButtonGroup(this)) {
  m_ui->setupUi(this);

  m_ui->empty_project->hide();
  m_ui->default_project->hide();
  m_ui->advanced->hide();

  /// @todo these should be dependent on the current template selection
  m_ui->renderer->addItem("OpenGL 3.2 compatibility / GLSL 1.50");
  m_ui->renderer->addItem("OpenGL 1.4 / OpenGL 2.0 / GLSL 1.10");

  m_group->setExclusive(true);

  QStringList files = ShaderDB::instance().localProjects();
  foreach (QString file, files) {
    MetaInfo info = MetaInfo::ping(file);

    if (!info.name.isEmpty()) {
      WelcomeButton* btn = 0;
      m_project_names << info.name.toLower();

      if (info.categories.contains("built-in")) {
        if (info.name == "Empty project") {
          btn = m_ui->empty_project;
        } else if (info.name == "Default shader project") {
          btn = m_ui->default_project;
        } else continue;
        btn->show();
        btn->setFilename(file);
      } else {
        btn = new WelcomeButton(m_ui->list, file);
        m_ui->list_layout->insertWidget(0, btn);
      }

      btn->setCheckable(true);
      m_group->addButton(btn);
      btn->setIcon(m_ui->default_project->icon());
      btn->setText(info.name);
      btn->setDescription(info.description);
      connect(btn, SIGNAL(clicked(QString)), this, SLOT(preview(QString)));
    }
  }

  m_ui->name->setText(getUniqName("My Project"));

  connect(m_ui->name, SIGNAL(editingFinished()), this, SLOT(nameEditingFinished()));
  connect(m_ui->name, SIGNAL(textEdited(QString)), this, SLOT(nameEdited(QString)));
  connect(m_ui->advanced_btn, SIGNAL(toggled(bool)), m_ui->advanced, SLOT(setShown(bool)));
  connect(this, SIGNAL(accepted()), SLOT(create()));
}

NewWizard::~NewWizard() {
  delete m_ui;
}

void NewWizard::keyPressEvent(QKeyEvent* e) {
  if (m_ui->name->hasFocus() &&
      (e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return)) {

    QList<QPushButton*> list = qFindChildren<QPushButton*>(this);
    for (int i = 0; i < list.size(); ++i) {
      QPushButton* pb = list.at(i);
      if (pb->isDefault()) {
        pb->setFocus();
        break;
      }
    }
    e->accept();
  } else {
    QDialog::keyPressEvent(e);
  }
}

void NewWizard::nameEditingFinished() {
  QString str = m_ui->name->text();
  QString name = getUniqName(str);
  if (str != name)
    m_ui->name->setText(name);
  m_ui->name_error->clear();
}

void NewWizard::nameEdited(QString str) {
  if (m_project_names.contains(str.toLower())) {
    m_ui->name_error->setText("Name is already in use");
  } else if (str.isEmpty()) {
    m_ui->name_error->setText("Name can't be empty");
  } else {
    m_ui->name_error->clear();
  }
}

void NewWizard::preview(QString file) {

}

void NewWizard::create() {
  WelcomeButton* btn = dynamic_cast<WelcomeButton*>(m_group->checkedButton());
  assert(btn);
  ShaderDB& db = ShaderDB::instance();
  /// @todo Maybe we should have a temporary location for the project until
  ///       user saves it for the first time. Then you could start a new tmp
  ///       project without saving anything to ShaderDB. Then we could have
  ///       a recovery-feature if we find old tmp project on startup.
  ScenePtr scene = db.newLocalProject(m_ui->name->text(), btn->filename());
  MainWindow::instance().openScene(scene);
}

QString NewWizard::getUniqName(QString str) const {
  if (str.isEmpty())
    str = "My Project";

  if (!m_project_names.contains(str.toLower()))
    return str;

  QString newname, base;
  QRegExp r("^(.*)(\\d+)$");
  r.setMinimal(true);
  int i;
  if (r.exactMatch(str)) {
    base = r.cap(1);
    i = r.cap(2).toInt();
  } else {
    base = str;
    if (!str.endsWith("."))
      base += ".";
    i = 0;
  }
  do {
    newname = base + QString::number(++i);
  } while (m_project_names.contains(newname.toLower()));
  return newname;
}
