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

#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "editor.hpp"
#include "shader/shader.hpp"
#include "scene.hpp"
#include "importer_wizard.hpp"
#include "material.hpp"
#include "resource_locator.hpp"
#include "shader/program.hpp"
#include "texture_browser.hpp"

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
  : QMainWindow(parent), m_ui(new Ui::MainWindow), m_sync(0), m_sceneChanged(false) {
  if (!s_instance)
    s_instance = this;

  m_ui->setupUi(this);
  //m_ui->filelist->init();
  m_ui->renderpass_properties->init();
  m_ui->material_properties->init();

  /*  QDockWidget *dw = new QDockWidget;
  dw->setObjectName("Foo");
  dw->setWindowTitle("Bar");
  dw->setWidget(new QTextEdit);

             addDockWidget(Qt::LeftDockWidgetArea, dw);*/
  //m_ui->properties

  connect(m_ui->action_textures, SIGNAL(triggered()), this, SLOT(openTextureBrowser()));

  connect(m_ui->error_list, SIGNAL(itemActivated(QTableWidgetItem*)),
          this, SLOT(errorItemActivated(QTableWidgetItem*)));
  connect(m_ui->action_savematerial, SIGNAL(triggered()), this, SLOT(saveMaterial()));
  connect(m_ui->action_open, SIGNAL(triggered()), this, SLOT(load()));
  connect(m_ui->action_savescene, SIGNAL(triggered()), this, SLOT(saveScene()));
  /*connect(m_ui->action_new, SIGNAL(triggered()), this, SLOT(open()));
  connect(m_ui->action_saveas, SIGNAL(triggered()), this, SLOT(open()));
  connect(m_ui->action_open, SIGNAL(triggered()), this, SLOT(open()));*/

  connect(m_ui->action_autosave_scene, SIGNAL(triggered(bool)), this, SLOT(setAutosaveScene(bool)));

  connect(m_ui->action_import, SIGNAL(triggered()), this, SLOT(import()));

  connect(m_ui->action_reload, SIGNAL(triggered()), this, SLOT(reload()));

  QAction* actions[] = {m_ui->action_glwidget, m_ui->action_material_properties,
      m_ui->action_render_properties, m_ui->action_error_log};
  QDockWidget* widgets[] = {m_ui->gldock, m_ui->shaderdock, m_ui->renderdock, m_ui->errordock};
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

  connect(&ShaderManager::instance(), SIGNAL(linked(ShaderErrorList)),
          this, SLOT(updateErrors(ShaderErrorList)));
  connect(&ShaderManager::instance(), SIGNAL(compiled(ShaderErrorList)),
          this, SLOT(updateErrors(ShaderErrorList)));

  QSettings settings("Shaderkit", "Shaderkit");
  m_ui->action_autosave_scene->setChecked(settings.value("gui/autosave_scene", true).toBool());
  m_ui->action_sandbox_compiler->setChecked(settings.value("core/use_sandbox_compiler", true).toBool());

  {
    m_ui->editor_menu->layout()->setMargin(0);

    QIcon icon(":/btns/nosync.png");
    icon.addFile(":/btns/sync.png", QSize(), QIcon::Normal, QIcon::On);
    m_sync = m_ui->editor_menu->addAction(icon, "Auto-compile");
    m_sync->setCheckable(true);
    m_sync->setChecked(true);

    icon.addFile(":/btns/sync.png", QSize(), QIcon::Normal, QIcon::On);
    m_ui->editor_menu->addAction(QIcon(":/btns/compile.png"), "Compile all",
                                 this, SLOT(compileAll()));
  }
}

MainWindow::~MainWindow() {
  if (s_instance == this)
    s_instance = 0;
}

MultiEditor* MainWindow::createEditor(MaterialPtr material) {
  QWidget* widget = new QWidget();
  QBoxLayout* layout = new QBoxLayout(QBoxLayout::TopToBottom, widget);
  layout->setMargin(0);
  layout->setSpacing(0);

  MultiEditor* editor = new MultiEditor(widget, material);

  layout->addWidget(editor, 1);

  m_editors[editor] = widget;

  m_ui->editor_tabs->addTab(widget, material->name());
  m_ui->editor_tabs->setCurrentIndex(m_ui->editor_tabs->count()-1);
  return editor;
}

void MainWindow::activateEditor(MultiEditor* editor) {
  m_ui->editor_tabs->setCurrentWidget(editor->parentWidget());
}

MainWindow& MainWindow::instance() {
  assert(s_instance);
  return *s_instance;
}

ScenePtr MainWindow::scene() {
  return instance().m_scene;
}

GLWidget* MainWindow::glwidget() {
  return m_ui->gl_widget;
}

void MainWindow::updateErrors(ShaderErrorList errors) {
  QList<GLSLEditor*> editors = findEditors(errors.shader);
  bool changed = false;

  // first step is to remove old error messages
  for (int i = 0; i < m_ui->error_list->rowCount(); ++i) {
    QTableWidgetItem* item = m_ui->error_list->item(i, 0);
    const ShaderError& e = m_error_list_items[item];
    if (errors.shader == e.shader() &&
        (!errors.shader.isEmpty() || errors.program == e.program())) {
      m_error_list_items.remove(item);
      m_ui->error_list->removeRow(i--);
      changed = true;
    }
  }

  if (!errors.shader.isEmpty())
    foreach (GLSLEditor* e, editors)
      e->clearErrors();

  int row = 0;
  foreach (ShaderError e, errors) {
    foreach (GLSLEditor* ed, editors)
      ed->compileError(e);

    bool skip = false;
    // make sure that there isn't equivalent error already in the list
    foreach (const ShaderError& e2, m_error_list_items) {
      if (e.isDuplicate(e2)) {
        skip = true;
        break;
      }
    }
    if (skip) continue;

    // error list hasn't material/program column on purpose, since one error
    // can point to many different materials, and will use magical logic to
    // determine the correct one when clicked
    QTableWidgetItem* msg = new QTableWidgetItem(e.msg());
    msg->setIcon(QIcon(QPixmap(e.type() == "warning" ? ":/icons/warning.png" : ":/icons/error.png")));
    m_ui->error_list->insertRow(row);
    m_ui->error_list->setItem(row, 0, msg);
    if (!e.shader().isEmpty())
      m_ui->error_list->setItem(row, 1, new QTableWidgetItem(ResourceLocator::ui(e.shader())));
    m_ui->error_list->setItem(row, 2, new QTableWidgetItem(QString::number(e.line()+1)));
    m_error_list_items[msg] = e;
    changed = true;
    ++row;
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

  // do this only once
  bool should_restore_settings = !m_scene;

  if (m_scene) {
    if (!closeScene()) return false;
    foreach (RenderPassPtr p, m_scene->renderPasses()) {
      RenderPassProperties::instance().remove(p);
    }

    disconnect(m_scene.get(), SIGNAL(changed()), this, SLOT(changed()));
    disconnect(m_scene.get(), SIGNAL(saved()), this, SLOT(saved()));
  }

  connect(scene.get(), SIGNAL(changed()), this, SLOT(changed()));
  connect(scene.get(), SIGNAL(saved()), this, SLOT(saved()));

  m_scene = scene;
  ResourceLocator::setPath("scene", scene->root());

  foreach (ProgramPtr prog, m_scene->materialPrograms()) {
    foreach (ShaderPtr shader, prog->shaders()) {
      Watcher::instance().add(this, shader->res());
    }
  }

  if (should_restore_settings)
    restore();

  m_ui->gl_widget->sceneChange(m_scene);
  emit newScene(m_scene);

  resize(sizeHint());

  setSceneChanged(false);

  m_scene->setAutomaticSaving(m_ui->action_autosave_scene->isChecked());

  m_ui->statusbar->showMessage("Opened scene " + scene->filename(), 5000);
  show();

  return true;
}

bool MainWindow::openScene(QString filename) {
  return openScene(Scene::load(filename));
}

bool MainWindow::reload() {
  if (!m_scene) return false;

  ScenePtr scene = Scene::load(m_scene->filename());
  if (scene && openScene(scene)) {
    m_ui->statusbar->showMessage("Reloaded " + scene->filename(), 5000);
    return true;
  } else {
    m_ui->statusbar->showMessage("Failed to reload " + m_scene->filename(), 5000);
    return false;
  }
}

void MainWindow::setSceneChanged(bool status) {
  if (!m_scene)
    return;
  m_sceneChanged = status;
  m_ui->action_savescene->setEnabled(status);
  if (status) {
    setWindowTitle(m_scene->metainfo().name + " (unsaved) - Shaderkit");
  } else {
    setWindowTitle(m_scene->metainfo().name + " - Shaderkit");
  }
}

QList<GLSLEditor*> MainWindow::findEditors(ShaderPtr shader) {
  return findEditors(shader->res());
}

QList<GLSLEditor*> MainWindow::findEditors(QString res) {
  QList<GLSLEditor*> ret;
  foreach (MultiEditor* editor, m_editors.keys()) {
    GLSLEditor* e = editor->editor(res);
    if (e) ret << e;
  }
  return ret;
}

MultiEditor* MainWindow::findEditor(MaterialPtr mat) {
  foreach(MultiEditor* editor, m_editors.keys())
    if(editor->material() == mat) return editor;
  return 0;
}

MultiEditor* MainWindow::openMaterial(MaterialPtr mat) {
  MultiEditor* editor = findEditor(mat);
  if(!editor) editor = createEditor(mat);
  m_ui->editor_tabs->setCurrentWidget(m_editors[editor]);
  return editor;
}

void MainWindow::fileUpdated(const QString& filename) {
  /// @todo implement
/*  Editor* editor = findEditor(filename);
  // If there is an editor, it is a job for it to handle the reloading
  if (editor) {
    editor->fileUpdated(filename);
  } else {
    QList<ShaderPtr> lst = m_active_scene->shadersByFilename(filename);
    foreach (ShaderPtr s, lst)
      s->loadFile(filename);
  }*/
}

void MainWindow::errorItemActivated(QTableWidgetItem* item) {
  const ShaderError& err = m_error_list_items[m_ui->error_list->item(item->row(), 0)];

  MultiEditor* editor = m_editors.key(m_ui->editor_tabs->currentWidget());
  if (editor) {
    ProgramPtr prog = editor->material()->prog();
    if (prog && (prog->name() == err.program() ||
                 prog->hasShader(err.shader()))) {
      editor->focusOnError(err);
      return;
    }
  }

  for (auto it = m_editors.begin(); it != m_editors.end(); ++it) {
    ProgramPtr prog = it.key()->material()->prog();
    if (prog && (prog->name() == err.program() ||
                 prog->hasShader(err.shader()))) {
      m_ui->editor_tabs->setCurrentWidget(*it);
      it.key()->focusOnError(err);
      return;
    }
  }

  if (err.material()) {
    MultiEditor* editor = openMaterial(err.material());
    editor->focusOnError(err);
    return;
  }

  foreach (MaterialPtr m, m_scene->materials().values()) {
    ProgramPtr prog = m->prog();
    if (prog && (prog->name() == err.program() ||
                 prog->hasShader(err.shader()))) {
      MultiEditor* editor = openMaterial(m);
      editor->focusOnError(err);
      return;
    }
  }
}

void MainWindow::modificationChanged(MultiEditor* editor, bool b) {
  int idx = m_ui->editor_tabs->indexOf(m_editors[editor]);
  m_ui->editor_tabs->setTabText(idx,
      editor->material()->name() + (b ? "*" : ""));
}

bool MainWindow::autoCompileEnabled() const {
  return m_sync->isChecked();
}

void MainWindow::saveMaterial(int index) {
  QWidget* widget = 0;
  if (index != -1)
    widget = m_ui->editor_tabs->widget(index);

  if (!widget)
    widget = m_ui->editor_tabs->currentWidget();

  if (!widget)
    return;

  MultiEditor* editor = m_editors.key(widget);
  editor->saveMaterial();
}

void MainWindow::saveScene() {
  if (m_scene->save(m_scene->filename())) {
    setSceneChanged(false);
    m_ui->statusbar->showMessage("Saved project to " + m_scene->filename(), 5000);
  } else {
    m_ui->statusbar->showMessage("Failed to save project to " + m_scene->filename(), 5000);
  }
}

bool MainWindow::load() {
  QSettings settings("Shaderkit", "Shaderkit");
  QString dir = settings.value("history/last_dir", QVariant(QDir::currentPath())).toString();
  QString file = QFileDialog::getOpenFileName(this, tr("Open Project"), dir,
                                              tr("Shaderkit projects (*.shaderkit *.zip)"));
  if (!file.isEmpty()) {
    QFileInfo fi(file);
    settings.setValue("history/last_dir", fi.absolutePath());
    return openScene(file);
  }

  return false;
}

void MainWindow::closeEditor(int index) {
  QWidget* widget = 0;
  if (index == -1) {
    widget = m_ui->editor_tabs->currentWidget();
  } else {
    widget = m_ui->editor_tabs->widget(index);
  }

  if (widget) {
    MultiEditor* editor = m_editors.key(widget);
    if (!editor->checkClose()) return;

    m_editors.remove(editor);
    m_ui->editor_tabs->removeTab(index);
    delete widget;
  }
}

bool MainWindow::closeScene() {
  /// @todo replace all of these with a single checkable list of changed files
  QMap<QTextDocument*, GLSLEditor*> editors;
  foreach (MultiEditor* meditor, m_editors.keys())
    foreach (GLSLEditor* editor, meditor->editors())
      editors[editor->document()] = editor;

  foreach (GLSLEditor* editor, editors)
    if (!editor->checkClose())
      return false;

  if (m_scene->isChanged()) {
    if (m_scene->automaticSaving()) {
      m_scene->syncHistory();
    } else {
      int ret = QMessageBox::question(this, "Unsaved changes", "The project has some unsaved changes, what to do?",
                                      QMessageBox::Save | QMessageBox::Close | QMessageBox::Cancel);
      if (ret == QMessageBox::Save) {
        if (!m_scene->save(m_scene->filename())) {
          /// @todo inform user?
          return false;
        }
      } else if (ret != QMessageBox::Close) {
        return false;
      }
    }
  }

  return true;
}

void MainWindow::closeEvent(QCloseEvent* event) {
  if (!closeScene()) {
    event->ignore();
    return;
  }

  /// @todo some of these should be project-specific - should they?
  QSettings settings("Shaderkit", "Shaderkit");
  if (m_ui->action_autosave_layout->isChecked()) {
    settings.setValue("gui/geometry", saveGeometry());
    settings.setValue("gui/windowState", saveState());
  }
  /// @todo rest of the gui options here
  settings.setValue("gui/autosave_layout", m_ui->action_autosave_layout->isChecked());
  QMainWindow::closeEvent(event);
}

void MainWindow::changed() {
  if (!m_sceneChanged)
    setSceneChanged(true);
}

void MainWindow::setSandboxCompiler(bool v) {
  Shader::setSandboxCompile(v);
  QSettings settings("Shaderkit", "Shaderkit");
  settings.setValue("core/use_sandbox_compiler", v);
}

void MainWindow::import() {
  ImporterWizard* w = new ImporterWizard(m_scene);
  w->show();
}

void MainWindow::compileAll() {
  foreach (MultiEditor* me, m_editors.keys()) {
    foreach (GLSLEditor* e, me->editors()) {
      QString tmp = e->toPlainText();
      foreach (ShaderPtr shader, m_scene->shaders(e->shader()->res()))
        shader->loadSrc(tmp);
    }
  }
}

void MainWindow::openTextureBrowser() {
  TextureBrowser::instance().show();
}

void MainWindow::setAutosaveScene(bool v) {
  QSettings settings("Shaderkit", "Shaderkit");
  settings.setValue("gui/autosave_scene", v);
  m_scene->setAutomaticSaving(v);
}

void MainWindow::saved() {
  setSceneChanged(false);
}

void MainWindow::restore() {
  QSettings settings("Shaderkit", "Shaderkit");
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
  setWindowTitle(tr("About Shaderkit"));

  QHBoxLayout* layout1 = new QHBoxLayout(this);
  QWidget* container = new QWidget;
  QVBoxLayout* layout2 = new QVBoxLayout(container);

  QLabel* text = new QLabel(tr("<h1>Shaderkit</h1>"));
  text->setWordWrap(true);

  QDialogButtonBox* bbox = new QDialogButtonBox(QDialogButtonBox::Close);
  connect(bbox, SIGNAL(rejected()), this, SLOT(reject()));

  layout2->addWidget(text);
  layout2->addWidget(bbox);

  QLabel* logo = new QLabel;
  layout1->addWidget(logo);
  layout1->addWidget(container);
}
