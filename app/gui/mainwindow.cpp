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

#include "gui/mainwindow.hpp"
#include "ui_mainwindow.h"
#include "editor/editor.hpp"
#include "editor/glsl_editor.hpp"
#include "gl/shader.hpp"
#include "core/scene.hpp"
#include "gui/importer_wizard.hpp"
#include "core/material.hpp"
#include "core/resource_locator.hpp"
#include "gl/program.hpp"
#include "gui/texture_browser.hpp"
#include "gui/glwidget.hpp"
#include "gui/viewport.hpp"
#include "gui/new_wizard.hpp"
#include "gui/save_project.hpp"

#include <QKeyEvent>
#include <QFile>
#include <QFileInfo>
#include <QLabel>
#include <QVBoxLayout>
#include <QSettings>
#include <QFileDialog>
#include <QMessageBox>
#include <QDialogButtonBox>

#include <cassert>

namespace Shaderkit
{

  IconBtn::IconBtn(QWidget* parent) : QPushButton(parent), m_padding(0, 0) {}
  IconBtn::~IconBtn() {}

  QSize IconBtn::sizeHint() const
  {
    return QSize(16, 16) + 2*m_padding;
  }

  void IconBtn::paintEvent(QPaintEvent*)
  {
    QPainter painter(this);
    painter.drawPixmap(m_padding.width(), m_padding.height(), icon().pixmap(16, QIcon::Normal, isChecked() ? QIcon::On : QIcon::Off));
  }

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

  MainWindow* MainWindow::s_instance = 0;

  MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), m_ui(new Ui::MainWindow), m_context(0), m_sync(0),
      m_loadedScenes(0)
  {
    if (!s_instance)
      s_instance = this;

    m_ui->setupUi(this);
    //m_ui->filelist->init();
    m_ui->renderpass_properties->init();
    m_ui->material_properties->init();

    createViewport();

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
    connect(m_ui->action_newscene, SIGNAL(triggered()), this, SLOT(newScene()));
    connect(m_ui->action_savescene_as, SIGNAL(triggered()), this, SLOT(saveSceneAs()));
    /*connect(m_ui->action_new, SIGNAL(triggered()), this, SLOT(open()));
    connect(m_ui->action_saveas, SIGNAL(triggered()), this, SLOT(open()));
    connect(m_ui->action_open, SIGNAL(triggered()), this, SLOT(open()));*/

    connect(m_ui->action_autosave_scene, SIGNAL(triggered(bool)), this, SLOT(setAutosaveScene(bool)));

    connect(m_ui->action_import, SIGNAL(triggered()), this, SLOT(import()));

    connect(m_ui->action_reload, SIGNAL(triggered()), this, SLOT(reload()));

    QAction* actions[] = {m_ui->action_toggle_materials,
                          m_ui->action_toggle_render_passes, m_ui->action_toggle_error_log
                         };
    QDockWidget* widgets[] = {m_ui->materials_dock, m_ui->render_passes_dock, m_ui->error_dock};
    for (size_t i = 0; i < sizeof(actions)/sizeof(*actions); ++i) {
      connect(actions[i], SIGNAL(triggered(bool)), widgets[i], SLOT(setVisible(bool)));
      connect(widgets[i], SIGNAL(visibilityChanged(bool)), actions[i], SLOT(setChecked(bool)));
    }

    connect(m_ui->action_reset_layout, SIGNAL(triggered()),
            this, SLOT(resetLayout()));

    connect(m_ui->action_about, SIGNAL(triggered()),
            this, SLOT(about()));
    connect(m_ui->editor_tabs, SIGNAL(tabCloseRequested(int)),
            this, SLOT(closeEditor(int)));

    connect(m_ui->action_sandbox_compiler, SIGNAL(toggled(bool)),
            this, SLOT(setSandboxCompiler(bool)));

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

  MainWindow::~MainWindow()
  {
    assert (s_instance == this);

    // Some objects (like GLWidget) use MainWindow instance in their destructors.
    // Delete them manually, before MainWindow is totally unusable.
    QObjectList lst = children();
    foreach (QObject* obj, lst)
      delete obj;

    s_instance = 0;
  }

  MultiEditor* MainWindow::createEditor(MaterialPtr material)
  {
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

  void MainWindow::activateEditor(MultiEditor* editor)
  {
    m_ui->editor_tabs->setCurrentWidget(editor->parentWidget());
  }

  MainWindow& MainWindow::instance()
  {
    assert(s_instance);
    return *s_instance;
  }

  void MainWindow::cleanup()
  {
    s_instance = 0;
  }

  ScenePtr MainWindow::scene()
  {
    return instance().m_scene;
  }

  void MainWindow::updateErrors(ShaderErrorList errors)
  {
    QList<GLSLEditor*> editors = findEditors(errors.shader_filename);
    bool changed = false;

    // first step is to remove old error messages
    for (int i = 0; i < m_ui->error_list->rowCount(); ++i) {
      QTableWidgetItem* item = m_ui->error_list->item(i, 0);
      const ShaderError& e = m_error_list_items[item];
      if (errors.shader_filename == e.shaderFilename() &&
          (!errors.shader_filename.isEmpty() || errors.program == e.program())) {
        m_error_list_items.remove(item);
        m_ui->error_list->removeRow(i--);
        changed = true;
      }
    }

    if (!errors.shader_filename.isEmpty())
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
      if (!e.shaderFilename().isEmpty())
        m_ui->error_list->setItem(row, 1, new QTableWidgetItem(ResourceLocator::ui(e.shaderFilename())));
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

  void MainWindow::destroyedGL(QGLWidget* widget)
  {
    assert(!m_context);
    assert(m_glwidgets.contains(widget));
    m_glwidgets.removeAll(widget);

    if (m_glwidgets.isEmpty()) {
      m_context = new QGLContext(widget->format());
      m_context->create(widget->context());
    }
  }

  QGLFormat MainWindow::formatGL() const
  {
    if (m_scene)
      return m_scene->glFormat();
    return QGLFormat::defaultFormat();
  }

  void MainWindow::about()
  {
    About about(this);
    about.exec();
  }

  bool MainWindow::openScene(ScenePtr scene)
  {
    if (!scene)
      return false;

    // do this only once
    bool should_restore_settings = !m_scene;

    if (m_scene) {
      if (!closeScene()) return false;

      disconnect(m_scene.get(), SIGNAL(changed(bool)), this, SLOT(changed(bool)));
      disconnect(m_scene.get(), SIGNAL(saved()), this, SLOT(saved()));

      disconnect(m_scene.get(), SIGNAL(renderPassesListUpdated(QList<RenderPassPtr>)),
                 &RenderPassProperties::instance(), SLOT(listUpdated(QList<RenderPassPtr>)));
      disconnect(m_scene.get(), SIGNAL(materialListUpdated(ScenePtr)),
                 &MaterialProperties::instance(), SLOT(updateMaterialList(ScenePtr)));

      disconnect(m_scene.get(), SIGNAL(progLinked(ShaderErrorList)),
                 this, SLOT(updateErrors(ShaderErrorList)));
      disconnect(m_scene.get(), SIGNAL(progCompiled(ShaderErrorList)),
                 this, SLOT(updateErrors(ShaderErrorList)));
      disconnect(m_scene.get(), SIGNAL(sceneMaterialChanged(MaterialPtr)),
                 &MaterialProperties::instance(), SLOT(update(MaterialPtr)));
    }

    connect(scene.get(), SIGNAL(changed(bool)), this, SLOT(changed(bool)));
    connect(scene.get(), SIGNAL(saved()), this, SLOT(saved()));

    connect(scene.get(), SIGNAL(renderPassesListUpdated(QList<RenderPassPtr>)),
            &RenderPassProperties::instance(), SLOT(listUpdated(QList<RenderPassPtr>)));
    connect(scene.get(), SIGNAL(materialListUpdated(ScenePtr)),
            &MaterialProperties::instance(), SLOT(updateMaterialList(ScenePtr)));

    connect(scene.get(), SIGNAL(progLinked(ShaderErrorList)),
            this, SLOT(updateErrors(ShaderErrorList)));
    connect(scene.get(), SIGNAL(progCompiled(ShaderErrorList)),
            this, SLOT(updateErrors(ShaderErrorList)));
    connect(scene.get(), SIGNAL(sceneMaterialChanged(MaterialPtr)),
            &MaterialProperties::instance(), SLOT(update(MaterialPtr)), Qt::QueuedConnection);

    m_scene = scene;
    ResourceLocator::setPath("scene", scene->root());

    foreach (ProgramPtr prog, m_scene->materialPrograms())
      foreach (ShaderPtr shader, prog->shaders())
        Watcher::instance().add(this, shader->filename());

    if (should_restore_settings)
      restore();

    if (m_scene)
      QGLFormat::setDefaultFormat(m_scene->glFormat());

    foreach (QGLWidget* w, m_glwidgets) {
      GLWidget* gl = dynamic_cast<GLWidget*>(w);
      if (gl) gl->sceneChange(m_scene);
    }

    emit newScene(m_scene);

    resize(sizeHint());

    setSceneChanged(m_scene->isChanged());
    RenderPassProperties::instance().listUpdated(m_scene->renderPasses());
    MaterialProperties::instance().updateMaterialList(m_scene);

    m_scene->setAutomaticSaving(m_ui->action_autosave_scene->isChecked());

    m_ui->statusbar->showMessage("Opened scene " + scene->filename(), 5000);
    show();

    ++m_loadedScenes;

    return true;
  }

  bool MainWindow::openScene(QString filename)
  {
    /// @todo Detect if we are opening a Limbo / Sample
    ///       - maybe add Scene::AutoDetect
    return openScene(Scene::load(filename, Scene::Ok));
  }

  bool MainWindow::reload()
  {
    return false;
    /// @todo Implement reload
    /*
    if (!m_scene) return false;

    ScenePtr scene = Scene::load(m_scene->filename());
    if (scene && openScene(scene)) {
      m_ui->statusbar->showMessage("Reloaded " + scene->filename(), 5000);
      return true;
    } else {
      m_ui->statusbar->showMessage("Failed to reload " + m_scene->filename(), 5000);
      return false;
    }*/
  }

  void MainWindow::setSceneChanged(bool isChanged)
  {
    if (!m_scene)
      return;

    QFont font = m_ui->action_savescene_as->font();
    font.setBold(isChanged && m_scene->state() == Scene::New);
    m_ui->action_savescene_as->setFont(font);

    if (isChanged) {
      setWindowTitle(m_scene->metainfo().name + " (unsaved) - Shaderkit");
    } else {
      setWindowTitle(m_scene->metainfo().name + " - Shaderkit");
    }
  }

  QList<GLSLEditor*> MainWindow::findEditors(ShaderPtr shader)
  {
    return findEditors(shader->filename());
  }

  QList<GLSLEditor*> MainWindow::findEditors(const QString& filename)
  {
    QList<GLSLEditor*> ret;
    foreach (MultiEditor* editor, m_editors.keys()) {
      GLSLEditor* e = editor->editor(filename);
      if (e) ret << e;
    }
    return ret;
  }

  MultiEditor* MainWindow::findEditor(MaterialPtr mat)
  {
    foreach (MultiEditor* editor, m_editors.keys())
      if (editor->material() == mat) return editor;
    return 0;
  }

  MultiEditor* MainWindow::openMaterial(MaterialPtr mat)
  {
    MultiEditor* editor = findEditor(mat);
    if (!editor) editor = createEditor(mat);
    m_ui->editor_tabs->setCurrentWidget(m_editors[editor]);
    return editor;
  }

  void MainWindow::fileUpdated(const QString& /*filename*/)
  {
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

  void MainWindow::errorItemActivated(QTableWidgetItem* item)
  {
    const ShaderError& err = m_error_list_items[m_ui->error_list->item(item->row(), 0)];

    MultiEditor* editor = m_editors.key(m_ui->editor_tabs->currentWidget());
    if (editor) {
      ProgramPtr prog = editor->material()->prog();
      if (prog && (prog->name() == err.program() ||
                   prog->hasShader(err.shaderFilename()))) {
        editor->focusOnError(err);
        return;
      }
    }

    for (auto it = m_editors.begin(); it != m_editors.end(); ++it) {
      ProgramPtr prog = it.key()->material()->prog();
      if (prog && (prog->name() == err.program() ||
                   prog->hasShader(err.shaderFilename()))) {
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
                   prog->hasShader(err.shaderFilename()))) {
        MultiEditor* editor = openMaterial(m);
        editor->focusOnError(err);
        return;
      }
    }
  }

  void MainWindow::modificationChanged(MultiEditor* editor, bool b)
  {
    int idx = m_ui->editor_tabs->indexOf(m_editors[editor]);
    m_ui->editor_tabs->setTabText(idx,
                                  editor->material()->name() + (b ? "*" : ""));
  }

  bool MainWindow::autoCompileEnabled() const
  {
    return m_sync->isChecked();
  }

  void MainWindow::saveMaterial(int index)
  {
    QWidget* widget = 0;
    if (index != -1)
      widget = m_ui->editor_tabs->widget(index);

    if (!widget)
      widget = m_ui->editor_tabs->currentWidget();

    if (!widget)
      return;

    /// @todo What if the scene is in ro-state?

    // Put "New" scene to limbo state before saving any individual files
    if (m_scene->state() == Scene::New) {
      QVariantMap scene = m_scene->toMap();
      m_scene->save(scene);
    }

    MultiEditor* editor = m_editors.key(widget);
    assert(editor);
    editor->saveMaterial();
  }

  void MainWindow::saveScene()
  {
    if (m_scene->state() == Scene::ReadOnly) {
      QString n = m_scene->metainfo().name;
      QMessageBox box(QMessageBox::Question, QString("Saving project %1").arg(n),
                      QString("Project %1 is currently opened in a read-only mode.\n"
                              "Do you want to save the project with a different name or "
                              "force overwrite of the read-only project?").arg(n));
      box.addButton(QMessageBox::Cancel);
      QPushButton* overwrite_btn = box.addButton("Overwrite", QMessageBox::DestructiveRole);
      QPushButton* save_btn = box.addButton("Save as...", QMessageBox::YesRole);
      box.setDefaultButton(save_btn);
      box.exec();
      if (box.clickedButton() == save_btn) {
        saveSceneAs();
        return;
      } else if (box.clickedButton() == overwrite_btn) {
        Log::info("Trying to overwrite the read-only project");
      } else {
        return;
      }
    } else if (m_scene->state() == Scene::New || m_scene->state() == Scene::Limbo) {
      Log::info("This is a new project, using save as dialog");
      saveSceneAs();
      return;
    }

    QString err;
    if (m_scene->save(m_scene->filename(), err)) {
      m_ui->statusbar->showMessage("Saved project to " + m_scene->filename(), 5000);
    } else {
      QString emsg = QString("Failed to save project to %1: %2").
          arg(m_scene->filename(), err);
      m_ui->statusbar->showMessage(emsg, 5000);
      QMessageBox::warning(this, "Failed to save project", emsg);
    }
  }

  void MainWindow::saveSceneAs()
  {
    SaveProject::save(m_scene);
  }

  void MainWindow::newScene()
  {
    if (!closeScene()) return;
    NewWizard* w = new NewWizard(this);
    w->show();
  }

  bool MainWindow::load()
  {
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

  void MainWindow::closeEditor(int index)
  {
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

  bool MainWindow::closeScene()
  {
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
          QString err;
          if (!m_scene->save(m_scene->filename(), err)) {
            QString emsg = QString("Failed to save project to %1: %2").
                arg(m_scene->filename(), err);
            m_ui->statusbar->showMessage(emsg, 5000);
            QMessageBox::warning(this, "Failed to save project", emsg);
            return false;
          }
        } else if (ret != QMessageBox::Close) {
          return false;
        }
      }
    }

    return true;
  }

  void MainWindow::closeEvent(QCloseEvent* event)
  {
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

  void MainWindow::changed(bool wasChangedBefore)
  {
    if (!wasChangedBefore)
      setSceneChanged(true);
  }

  void MainWindow::setSandboxCompiler(bool v)
  {
    Shader::setSandboxCompile(v);
    QSettings settings("Shaderkit", "Shaderkit");
    settings.setValue("core/use_sandbox_compiler", v);
  }

  void MainWindow::import()
  {
    ImporterWizard* w = new ImporterWizard(m_scene);
    w->show();
  }

  void MainWindow::resetLayout()
  {
    /// @todo implement
  }

  void MainWindow::compileAll()
  {
    foreach (MultiEditor* me, m_editors.keys()) {
      foreach (GLSLEditor* e, me->editors()) {
        QString tmp = e->toPlainText();
        foreach (ShaderPtr shader, m_scene->shaders(e->shader()->filename()))
          shader->loadSrc(tmp);
      }
    }
  }

  void MainWindow::openTextureBrowser()
  {
    TextureBrowser::instance().show();
  }

  void MainWindow::setAutosaveScene(bool v)
  {
    QSettings settings("Shaderkit", "Shaderkit");
    settings.setValue("gui/autosave_scene", v);
    m_scene->setAutomaticSaving(v);
  }

  void MainWindow::saved()
  {
    /// @todo update recent projects -menu
    ResourceLocator::setPath("scene", m_scene->root());
    setSceneChanged(false);
  }

  void MainWindow::createViewport()
  {
    QSet<int> used;
    QRegExp match("^Viewport(\\s\\d+)?$");
    foreach (QDockWidget* d, findChildren<QDockWidget*>(match))
      if (match.exactMatch(d->objectName()))
        used << match.cap(1).toInt();

    int id = 0;
    while (used.contains(id)) ++id;

    QDockWidget* gldock = new QDockWidget(this);
    if (id == 0) gldock->setObjectName("Viewport");
    else gldock->setObjectName(QString("Viewport %1").arg(id));
    gldock->setWindowTitle(gldock->objectName());

    gldock->setWidget(new Viewport);

    addDockWidget(Qt::TopDockWidgetArea, gldock);
    connect(m_ui->action_toggle_viewport, SIGNAL(triggered(bool)), gldock, SLOT(setVisible(bool)));
    connect(gldock, SIGNAL(visibilityChanged(bool)), m_ui->action_toggle_viewport, SLOT(setChecked(bool)));
  }

  Viewport* MainWindow::mainViewport() const
  {
    /// @todo should take "main" viewport instead of first one
    QRegExp match("^Viewport(\\s\\d+)?$");
    foreach (QDockWidget* d, findChildren<QDockWidget*>(match)) {
      Viewport* view = dynamic_cast<Viewport*>(d->widget());
      if (view) return view;
    }
    return 0;
  }

  void MainWindow::restore()
  {
    QSettings settings("Shaderkit", "Shaderkit");
    m_ui->action_autosave_layout->setChecked(settings.value("gui/autosave_layout", true).toBool());
    /// @todo create all gl widgets
    restoreGeometry(settings.value("gui/geometry").toByteArray());
    restoreState(settings.value("gui/windowState").toByteArray());
  }

  void MainWindow::keyPressEvent(QKeyEvent* e)
  {
    QWidget::keyPressEvent(e);
  }

  void MainWindow::changeEvent(QEvent* e)
  {
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
      m_ui->retranslateUi(this);
      break;
    default:
      break;
    }
  }

  About::About(QWidget* parent) : QDialog(parent)
  {
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

} // namespace Shaderkit
