/**
 * Copyright 2010,2011 Riku Palomäki.
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

#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "editor.hpp"
#include "shader/shader.hpp"
#include "project.hpp"
#include "scene.hpp"
#include "importer_wizard.hpp"

#include <QKeyEvent>
#include <QFile>
#include <QFileInfo>
#include <QLabel>

#include <cassert>

IconBtn::IconBtn(QWidget* parent) : QPushButton(parent), m_padding(0, 0) {}
IconBtn::~IconBtn() {}

QSize IconBtn::sizeHint() const {
  return QSize(16, 16) + 2*m_padding;
}

void IconBtn::paintEvent(QPaintEvent*) {
  QPainter painter(this);
  painter.drawPixmap(m_padding.width(), m_padding.height(), icon().pixmap(16, QIcon::Normal, isChecked() ? QIcon::On : QIcon::Off));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

MainWindow * MainWindow::s_instance = 0;

MainWindow::MainWindow(QWidget* parent)
  : QMainWindow(parent), m_ui(new Ui::MainWindow), m_projectChanged(false) {
  if (!s_instance)
    s_instance = this;

  m_ui->setupUi(this);
  m_ui->filelist->init();
  m_ui->renderpass_properties->init();
  m_ui->material_properties->init();

  /*  QDockWidget *dw = new QDockWidget;
  dw->setObjectName("Foo");
  dw->setWindowTitle("Bar");
  dw->setWidget(new QTextEdit);

             addDockWidget(Qt::LeftDockWidgetArea, dw);*/
  //m_ui->properties
  connect(m_ui->error_list, SIGNAL(itemActivated(QTableWidgetItem*)),
          this, SLOT(errorItemActivated(QTableWidgetItem*)));
  connect(m_ui->action_save, SIGNAL(triggered()), this, SLOT(save()));
  connect(m_ui->action_open, SIGNAL(triggered()), this, SLOT(load()));
  connect(m_ui->action_saveproject, SIGNAL(triggered()), this, SLOT(saveProject()));
  /*connect(m_ui->action_new, SIGNAL(triggered()), this, SLOT(open()));
  connect(m_ui->action_saveas, SIGNAL(triggered()), this, SLOT(open()));
  connect(m_ui->action_open, SIGNAL(triggered()), this, SLOT(open()));*/

  connect(m_ui->action_import, SIGNAL(triggered()), this, SLOT(import()));

  connect(m_ui->action_reload, SIGNAL(triggered()), this, SLOT(reload()));

  QAction* actions[] = {m_ui->action_glwidget, m_ui->action_material_properties,
      m_ui->action_render_properties, m_ui->action_file_list, m_ui->action_error_log};
  QDockWidget* widgets[] = {m_ui->gldock, m_ui->shaderdock, m_ui->renderdock, m_ui->filesdock, m_ui->errordock};
  for (size_t i = 0; i < sizeof(actions)/sizeof(*actions); ++i) {
    connect(actions[i], SIGNAL(triggered(bool)), widgets[i], SLOT(setVisible(bool)));
    connect(widgets[i], SIGNAL(visibilityChanged(bool)), actions[i], SLOT(setChecked(bool)));
  }

  connect(m_ui->action_about, SIGNAL(triggered()),
          this, SLOT(about()));
  connect(m_ui->editor_tabs, SIGNAL(tabCloseRequested(int)),
         this, SLOT(closeEditor(int)));

  connect(m_ui->action_sandbox_compiler, SIGNAL(toggled(bool)),
          this, SLOT(setSandboxCompiler(bool)));

  QSettings settings("GLSL-Lab", "GLSL-Lab");
  m_ui->action_sandbox_compiler->setChecked(settings.value("core/use_sandbox_compiler", true).toBool());
}

MainWindow::~MainWindow() {
  if (s_instance == this)
    s_instance = 0;
}

void MainWindow::setProject(ProjectPtr p) {
  bool should_restore = !m_project;
  m_project = p;
  connect(p.get(), SIGNAL(sceneChanged(ScenePtr)),
          m_ui->gl_widget, SLOT(sceneChange(ScenePtr)));
  if (should_restore)
    restore();
}

Editor* MainWindow::createEditor(ShaderPtr shader) {
  QWidget* widget = new QWidget();
  QBoxLayout* layout = new QBoxLayout(QBoxLayout::TopToBottom, widget);
  layout->setMargin(0);
  layout->setSpacing(0);

  QWidget* bottom = new QWidget(widget);
  QHBoxLayout* bottom_layout = new QHBoxLayout(bottom);
  bottom_layout->setMargin(5);
  bottom_layout->setSpacing(0);

  Editor* editor = new Editor(widget, shader);
  connect(editor, SIGNAL(modificationChanged(bool)), this, SLOT(modificationChanged(bool)));

  QPushButton* btn = new IconBtn(bottom);
  bottom_layout->addWidget(btn);

  QIcon icon;
  icon.addFile(":/btns/nosync.png", QSize(), QIcon::Normal, QIcon::Off);
  icon.addFile(":/btns/sync.png", QSize(), QIcon::Normal, QIcon::On);

  btn->setIcon(icon);
  btn->setCheckable(true);
  btn->setChecked(true);
  btn->setToolTip("Auto-compile");

  connect(btn, SIGNAL(toggled(bool)), editor, SLOT(syncToggled(bool)));

  layout->addWidget(editor);
  layout->addWidget(bottom);

  editor->readFile(shader->filename());

  m_editors.push_back(editor);

  QFileInfo fi(shader->filename());
  m_ui->editor_tabs->addTab(widget, fi.fileName());
  m_ui->editor_tabs->setCurrentIndex(m_ui->editor_tabs->count()-1);
  return editor;
}

void MainWindow::activateEditor(Editor* editor) {
  m_ui->editor_tabs->setCurrentWidget(editor->parentWidget());
}

MainWindow& MainWindow::instance() {
  assert(s_instance);
  return *s_instance;
}

void MainWindow::shaderCompiled(ShaderPtr shader, ShaderError::List errors) {
  bool changed = false;
  for (int i = 0; i < m_ui->error_list->rowCount(); ++i) {
    QTableWidgetItem* item = m_ui->error_list->item(i, 0);
    if (m_error_list_items[item].shader() == shader) {
      m_error_list_items.remove(item);
      m_ui->error_list->removeRow(i--);
      changed = true;
    }
  }

  for (ShaderError::List::iterator e = errors.begin(); e != errors.end(); ++e) {
    int r = m_ui->error_list->rowCount();
    m_ui->error_list->setRowCount(r+1);

    /// @todo change order, put this on top
    QTableWidgetItem* msg = new QTableWidgetItem(e->msg());
    msg->setIcon(QIcon(QPixmap(e->type() == "warning" ? ":/icons/warning.png" : ":/icons/error.png")));
    m_ui->error_list->setItem(r, 0, msg);
    if (shader) {
      QFileInfo fi(shader->filename());
      m_ui->error_list->setItem(r, 1, new QTableWidgetItem(fi.fileName()));
    }
    m_ui->error_list->setItem(r, 2, new QTableWidgetItem(QString::number(e->line()+1)));
    m_error_list_items[msg] = *e;
    changed = true;
  }

  if (changed) {
    m_ui->error_list->resizeColumnsToContents();
    m_ui->error_list->update();
  }
}

void MainWindow::about() {
  About about(this);
  about.exec();
}

bool MainWindow::openScene(ScenePtr scene) {
  if (!scene)
    return false;

  ProjectPtr project(new Project(*this, scene->filename()));
  setProject(project);
  project->setScene(scene);
  resize(sizeHint());

  setProjectChanged(false);

  m_ui->statusbar->showMessage("Opened project " + m_project->filename(), 5000);
  show();

  return true;
}

bool MainWindow::openProject(QString filename) {
  return openScene(Project::load(filename));
}

bool MainWindow::reload() {
  ScenePtr scene = Project::load(m_project->filename());
  if (!scene) {
    m_ui->statusbar->showMessage("Failed to reload " + m_project->filename(), 5000);
    return false;
  }

  m_project->setScene(scene);

  m_ui->statusbar->showMessage("Reloaded " + m_project->filename(), 5000);
  setProjectChanged(false);
  m_projectChanged = false;

  return true;
}

void MainWindow::setProjectChanged(bool status) {
  if (!m_project)
    return;
  m_projectChanged = status;
  m_ui->action_saveproject->setEnabled(status);
  if (status) {
    setWindowTitle(m_project->activeScene()->metainfo().name + " (unsaved) - GLSL Lab");
  } else {
    setWindowTitle(m_project->activeScene()->metainfo().name + " - GLSL Lab");
  }
}

void MainWindow::errorItemActivated(QTableWidgetItem* item) {
  ShaderError err = m_error_list_items[m_ui->error_list->item(item->row(), 0)];

  for (int idx = 0; idx < m_editors.size(); ++idx) {
    if (m_editors[idx]->shader() == err.shader()) {
      m_editors[idx]->focusOnError(err);
      m_ui->editor_tabs->setCurrentIndex(idx);
      break;
    }
  }
}

void MainWindow::modificationChanged(bool b) {
  Editor* editor = dynamic_cast<Editor*>(sender());
  if (editor) {
    QFileInfo fi(editor->filename());
    m_ui->editor_tabs->setTabText(m_editors.indexOf(editor),
                                  fi.fileName() + (b ? "*" : ""));
  }
}

void MainWindow::save(int index) {
  if (index == -1)
    index = m_ui->editor_tabs->currentIndex();

  if (index == -1)
    return;

  Editor* editor = m_editors[index];
  QFile file(editor->filename());
  if (file.open(QIODevice::WriteOnly)) {
    file.write(editor->toPlainText().toUtf8());
    editor->document()->setModified(false);
  }
}

void MainWindow::saveProject() {
  if (m_project->save(m_project->filename())) {
    setProjectChanged(false);
    m_ui->statusbar->showMessage("Saved project to " + m_project->filename(), 5000);
  } else {
    m_ui->statusbar->showMessage("Failed to save project to " + m_project->filename(), 5000);
  }
}

bool MainWindow::load() {
  QSettings settings("GLSL-Lab", "GLSL-Lab");
  QString dir = settings.value("history/last_dir", QVariant(QDir::currentPath())).toString();
  QString file = QFileDialog::getOpenFileName(this, tr("Open Project"), dir,
                                              tr("GLSL Lab projects (*.lab *.zip)"));
  if (!file.isEmpty()) {
    QFileInfo fi(file);
    settings.setValue("history/last_dir", fi.absolutePath());
    return openProject(file);
  }

  return false;
}

void MainWindow::closeEditor(int index) {
  if (index < 0)
    index = m_ui->editor_tabs->currentIndex();

  QWidget* widget = m_ui->editor_tabs->widget(index);
  if (widget) {
    Editor* editor = widget->findChild<Editor*>("editor");
    if (editor) {
      if (editor->document()->isModified()) {
        int ret = QMessageBox::question(this, "Unsaved changes", "The file has some unsaved changes, what to do?",
                                        QMessageBox::Save | QMessageBox::Close | QMessageBox::Cancel);
        if (ret == QMessageBox::Save) {
          save(index);
        } else if (ret != QMessageBox::Close) {
          return;
        }
      }

      m_editors.removeAll(editor);
      m_ui->editor_tabs->removeTab(index);
    }
  }
}

void MainWindow::closeEvent(QCloseEvent* event) {
  /// @todo some of these should be project-specific
  QSettings settings("GLSL-Lab", "GLSL-Lab");
  if (m_ui->action_autosave_layout->isChecked()) {
    settings.setValue("gui/geometry", saveGeometry());
    settings.setValue("gui/windowState", saveState());
  }
  /// @todo rest of the gui options here
  settings.setValue("gui/autosave_layout", m_ui->action_autosave_layout->isChecked());
  QMainWindow::closeEvent(event);
}

void MainWindow::changed(RenderPassPtr) {
  if (!m_projectChanged)
    setProjectChanged(true);
}

void MainWindow::setSandboxCompiler(bool v) {
  Shader::setSandboxCompile(v);
  QSettings settings("GLSL-Lab", "GLSL-Lab");
  settings.setValue("core/use_sandbox_compiler", v);
}

void MainWindow::import() {
  ImporterWizard* w = new ImporterWizard(m_project->activeScene());
  w->show();
}

void MainWindow::restore() {
  QSettings settings("GLSL-Lab", "GLSL-Lab");
  m_ui->action_autosave_layout->setChecked(settings.value("gui/autosave_layout", true).toBool());
  restoreGeometry(settings.value("gui/geometry").toByteArray());
  restoreState(settings.value("gui/windowState").toByteArray());
}

void MainWindow::keyPressEvent(QKeyEvent* e) {
  QWidget::keyPressEvent(e);
}

void MainWindow::changeEvent(QEvent* e) {
  QMainWindow::changeEvent(e);
  switch (e->type()) {
  case QEvent::LanguageChange:
    m_ui->retranslateUi(this);
    break;
  default:
    break;
  }
}

About::About(QWidget* parent) : QDialog(parent) {
  setWindowTitle(tr("About GLSL Lab"));

  QHBoxLayout* layout1 = new QHBoxLayout(this);
  QWidget* container = new QWidget;
  QVBoxLayout* layout2 = new QVBoxLayout(container);

  QLabel* text = new QLabel(tr("<h1>GLSL Lab</h1>"));
  text->setWordWrap(true);

  QDialogButtonBox* bbox = new QDialogButtonBox(QDialogButtonBox::Close);
  connect(bbox, SIGNAL(rejected()), this, SLOT(reject()));

  layout2->addWidget(text);
  layout2->addWidget(bbox);

  QLabel* logo = new QLabel;
  layout1->addWidget(logo);
  layout1->addWidget(container);
}
