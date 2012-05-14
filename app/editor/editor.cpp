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

#include "editor/editor.hpp"

/// @todo shouldn't need to include this, split parts of GLSLEditor to Editor
#include "editor/glsl_editor.hpp"

#include "core/material.hpp"
#include "core/resource_locator.hpp"
#include "gl/program.hpp"
#include "gui/mainwindow.hpp"
#include "core/scene.hpp"

#include <QPainter>
#include <QTextBlock>
#include <QFile>
#include <QToolTip>
#include <QMessageBox>
#include <QScrollArea>
#include <QListWidget>
#include <QVBoxLayout>
#include <QSplitter>
#include <QDebug>
#include <QScrollBar>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QSignalMapper>
#include <QLabel>
#include <QToolBar>
#include <QAction>
#include <QTimer>
#include <QMenu>
#include <QSettings>
#include <QFileDialog>

#include <cassert>

namespace Shaderkit
{

  EditorMargin::EditorMargin(GLSLEditor* editor) : QWidget(editor), m_editor(editor) {}

  QSize EditorMargin::sizeHint() const
  {
    return QSize(m_editor->marginWidth(), 0);
  }

  void EditorMargin::paintEvent(QPaintEvent* event)
  {
    m_editor->marginPaintEvent(event);
  }

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

  CheckBoxDialog::CheckBoxDialog(const QString& text, bool show_checkbox)
    : m_checkbox(0)
  {
    QVBoxLayout* layout = new QVBoxLayout(this);

    QLabel* label = new QLabel(text, this);
    label->setTextInteractionFlags(Qt::TextInteractionFlags(style()->styleHint(QStyle::SH_MessageBox_TextInteractionFlags, 0, this)));
    label->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    label->setWordWrap(true);

    if (show_checkbox)
      m_checkbox = new QCheckBox("Delete the file from the file system", this);

    QDialogButtonBox* box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(box, SIGNAL(accepted()), this, SLOT(accept()));
    connect(box, SIGNAL(rejected()), this, SLOT(reject()));

    layout->addWidget(label);
    if (m_checkbox) layout->addWidget(m_checkbox);
    layout->addWidget(box);
  }

  bool CheckBoxDialog::checked() const
  {
    return m_checkbox && m_checkbox->isChecked();
  }

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

  FileListWidget::FileListWidget(QWidget* parent)
    : QListWidget(parent) {}

  int FileListWidget::preferredWidth()
  {
    return std::max(contentsSize().width() + 10, 150);
  }

  MultiEditor::MultiEditor(QWidget* parent, MaterialPtr material)
    : QFrame(parent),
      m_canvas(new QWidget(this)),
      m_material(material),
      m_mapper(new QSignalMapper(this))
  {
    setFrameShape(QFrame::NoFrame);

    QVBoxLayout* main_layout = new QVBoxLayout(this);
    main_layout->setContentsMargins(0, 0, 0, 0);

    m_splitter = new QSplitter(Qt::Horizontal, this);

    m_area = new QScrollArea(m_splitter);
    m_area->setWidgetResizable(true);
    m_area->setWidget(m_canvas);
    m_area->setFrameShape(QFrame::NoFrame);

    QWidget* sidebar = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(sidebar);
    layout->setMargin(0);
    layout->setSpacing(0);

    m_list = new FileListWidget(sidebar);
    m_list->setFrameShape(QFrame::NoFrame);
    layout->addWidget(m_list);

    QToolBar* tb = new QToolBar(sidebar);
    tb->setFloatable(false);
    tb->setMovable(false);
    tb->setIconSize(QSize(16, 16));
    layout->addWidget(tb);

    m_create = tb->addAction(QIcon(":/icons/new2.png"), "New shader file",
                             this, SLOT(create()));
    m_open = tb->addAction(QIcon(":/icons/load_textfile.png"), "Open a shader file",
                           this, SLOT(load()));
    tb->addSeparator();
    m_duplicate = tb->addAction(QIcon(":/icons/duplicate.png"), "Duplicate shader file",
                                this, SLOT(duplicate()));
    tb->addSeparator();
    m_destroy = tb->addAction(QIcon(":/icons/delete.png"), "Delete shader file",
                              this, SLOT(remove()));

    m_duplicate->setEnabled(false);
    m_destroy->setEnabled(false);

    m_splitter->addWidget(sidebar);
    m_splitter->addWidget(m_area);
    m_splitter->setStretchFactor(0, 0);
    m_splitter->setStretchFactor(1, 1);
    m_splitter->setCollapsible(1, false);

    main_layout->addWidget(m_splitter);

    new QVBoxLayout(m_canvas);

    connect(m_mapper, SIGNAL(mapped(QString)), this, SLOT(autosize(QString)));

    ProgramPtr prog = m_material->prog();
    if (prog) {
      foreach (ShaderPtr shader, prog->shaders())
        addShader(shader);
    }
    connect(m_material.get(), SIGNAL(changed(MaterialPtr)), this, SLOT(materialChanged()));

    connect(m_list, SIGNAL(clicked(QModelIndex)),
            this, SLOT(scrollTo(QModelIndex)));
    connect(m_list, SIGNAL(itemChanged(QListWidgetItem*)),
            this, SLOT(itemChanged(QListWidgetItem*)));
    connect(m_list, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
    connect(material.get(), SIGNAL(shaderChanged(ShaderPtr)),
            this, SLOT(shaderChanged(ShaderPtr)));
  }

  void MultiEditor::addShader(ShaderPtr shader)
  {
    QList<GLSLEditor*> editors = MainWindow::instance().findEditors(shader);
    QTextDocument* doc = editors.isEmpty() ? 0 : editors[0]->document();

    QString src = shader->src();

    Section& s = m_sections[shader->filename()];

    s.item = new QListWidgetItem(shader->icon(), ResourceLocator::ui(shader->filename()));
    s.item->setFlags(s.item->flags() | Qt::ItemIsUserCheckable | Qt::ItemIsEditable);
    /// @todo handle hide/show
    s.item->setCheckState(Qt::Checked);
    s.item->setData(Qt::UserRole, shader->filename());
    m_list->addItem(s.item);

    s.header = new QWidget(m_canvas);
    QHBoxLayout* l = new QHBoxLayout(s.header);

    s.label = new QLabel("<b>"+ResourceLocator::ui(shader->filename())+"</b>", s.header);
    s.icon = new QLabel(s.header);
    s.icon->setPixmap(shader->icon().pixmap(16));
    l->addWidget(s.icon);
    l->addWidget(s.label);
    l->addStretch();
    l->setContentsMargins(2, 2, 0, 0);
    m_canvas->layout()->addWidget(s.header);

    s.editor = new GLSLEditor(*this, m_canvas, shader, doc);

    m_canvas->layout()->addWidget(s.editor);

    m_mapper->setMapping(s.editor->document()->documentLayout(), shader->filename());
    connect(s.editor->document()->documentLayout(), SIGNAL(documentSizeChanged(QSizeF)),
            m_mapper, SLOT(map()));

    connect(s.editor->document(), SIGNAL(modificationChanged(bool)),
            this, SLOT(editorModified(bool)));
    connect(s.editor, SIGNAL(cursorPositionChanged()),
            this, SLOT(ensureCursorVisible()));
    connect(s.editor->document(), SIGNAL(modificationChanged(bool)),
            this, SLOT(docModificationChanged(bool)));

    if (!doc) {
      s.editor->setText(src);

      QFile f(shader->filename());
      bool changed = true;
      if (f.open(QFile::ReadOnly))
        changed = f.readAll() != src;
      if (changed)
        s.editor->document()->setModified(true);
    }
    autosize(shader->filename());
  }

  void MultiEditor::relayout()
  {
    int w = m_list->preferredWidth();
    m_list->parentWidget()->setMaximumWidth(w);
    QList<int> sizes = m_splitter->sizes();
    sizes[0] = w;
    m_splitter->setSizes(sizes);
  }

  void MultiEditor::editorModified(bool b)
  {
    MainWindow::instance().modificationChanged(this, b);
  }

  void MultiEditor::materialChanged()
  {
    ProgramPtr prog = m_material->prog();
    if (prog) {
      QSet<ShaderPtr> target = prog->shaders();
      QSet<ShaderPtr> current;
      foreach (const Section& s, m_sections)
        current << s.editor->shader();

      foreach (ShaderPtr s, current - target) {
        Section section = m_sections.take(s->filename());
        delete section.item;
        delete section.header;
        section.editor->deleteLater();
      }
      foreach (ShaderPtr s, target - current) {
        addShader(s);
      }
    }
  }

  void MultiEditor::ensureCursorVisible(GLSLEditor* ed)
  {
    if (!ed) ed = qobject_cast<GLSLEditor*>(sender());
    assert(ed);
    QRect r = ed->cursorRect().adjusted(-40, -10, 40, 10);
    r.moveCenter(ed->viewport()->mapTo(m_canvas, r.center()));
    m_area->ensureVisible(r.center().x(), r.center().y(),
                          r.width(), r.height());
  }

  void MultiEditor::itemChanged(QListWidgetItem* item)
  {
    foreach (const Section& s, m_sections) {
      if (s.item != item) continue;
      QString from = s.editor->shader()->filename();
      if (ResourceLocator::ui(from) == item->text()) continue;
      QString to = ResourceLocator::rename(from, item->text(), MainWindow::scene()->filenames());
      MainWindow::scene()->renameShaderFile(from, to, false);
      break;
    }
  }

  void MultiEditor::shaderChanged(ShaderPtr shader)
  {
    for (auto it = m_sections.begin(); it != m_sections.end(); ++it) {
      if (it->editor->shader() == shader) {
        Section s = *it;
        // we remove the section temporarily, because a) we want to change the key and
        // b) setText/setData will trigger itemChanged again, and this will be called
        // again. if the endless recursion won't kill us, then the invalidated it will
        // do that later
        m_sections.erase(it);
        const QString e = s.editor->document()->isModified() ? "*" : "";
        QFont font = s.item->font();
        font.setItalic(s.editor->document()->isModified());
        s.item->setFont(font);
        /// @todo can't do this because of the file renaming
        // s.item->setText(ResourceLocator::ui(shader->filename()) + e);
        s.item->setText(ResourceLocator::ui(shader->filename()));
        s.item->setData(Qt::UserRole, shader->filename());
        s.label->setText("<b>"+ResourceLocator::ui(shader->filename())+e+"</b>");
        m_sections[shader->filename()] = s;
        break;
      }
    }
  }

  void MultiEditor::selectionChanged()
  {
    const Section* s = selected();
    if (s) {
      m_duplicate->setEnabled(true);
      m_destroy->setEnabled(true);
    } else {
      m_duplicate->setEnabled(false);
      m_destroy->setEnabled(false);
    }
  }

  void MultiEditor::docModificationChanged(bool m)
  {
    QTextDocument* doc = dynamic_cast<QTextDocument*>(sender());
    if (!doc) return;

    for (auto it = m_sections.begin(); it != m_sections.end(); ++it) {
      if (it->editor->document() == doc) {
        Section& s = *it;
        const QString e = m ? "*" : "";
        s.label->setText("<b>"+ResourceLocator::ui(s.editor->shader()->filename())+e+"</b>");
        QFont font = s.item->font();
        font.setItalic(m);
        s.item->setFont(font);
        //s.item->setText(ResourceLocator::ui(s.editor->shader()->filename()) + e);
      }
    }
  }

  void MultiEditor::create()
  {
    QMenu menu("Create a new shader file", this);
    menu.addAction(Shader::icon(Shader::Vertex), "New vertex shader file")->setData(Shader::Vertex);
    menu.addAction(Shader::icon(Shader::TessCtrl), "New tesselation control shader file")->setData(Shader::TessCtrl);
    menu.addAction(Shader::icon(Shader::TessEval), "New tesselation evaluation shader file")->setData(Shader::TessEval);
    menu.addAction(Shader::icon(Shader::Geometry), "New geometry shader file")->setData(Shader::Geometry);
    menu.addAction(Shader::icon(Shader::Fragment), "New fragment shader file")->setData(Shader::Fragment);
    QAction* a = menu.exec(QCursor::pos());
    if (!a) return;

    int t = a->data().toInt();
    QString f;
    if (t == Shader::Vertex) f = ".vert";
    else if (t == Shader::TessCtrl) f = ".tsctrl";
    else if (t == Shader::TessEval) f = ".tseval";
    else if (t == Shader::Geometry) f = ".geom";
    else if (t == Shader::Fragment) f = ".frag";
    else return;

    f = ResourceLocator::unique("$scene/untitled" + f, MainWindow::scene()->filenames());
    ShaderPtr shader = m_material->prog(true)->addShaderSrc("", (Shader::Type)t);
    if (shader) shader->setFilename(f);
  }

  void MultiEditor::load()
  {
    QMap<QString, Shader::Type> shaders;
    foreach (ProgramPtr p, MainWindow::scene()->materialPrograms())
      foreach (ShaderPtr s, p->shaders())
        shaders[s->filename()] = s->type();

    if (m_material->prog())
      foreach (ShaderPtr s, m_material->prog()->shaders())
        shaders.remove(s->filename());

    if (!shaders.isEmpty()) {
      QMenu menu("Load a shader file", this);
      for (auto it = shaders.begin(); it != shaders.end(); ++it) {
        /// @todo Handle ui-name duplicates right
        menu.addAction(Shader::icon(*it), ResourceLocator::ui(it.key()))->setData(it.key());
      }
      menu.addSeparator();
      menu.addAction(QIcon(":/icons/load_textfile.png"), "Browse...");
      QAction* a = menu.exec(QCursor::pos());
      if (!a) return;

      QString res = a->data().toString();
      if (shaders.contains(res)) {
        m_material->prog(true)->addShader(res, shaders[res]);
        return;
      }
    }

    QSettings settings("Shaderkit", "Shaderkit");
    QString dir = settings.value("history/last_import_dir",
                                 settings.value("history/last_dir",
                                     QVariant(QDir::currentPath()))).toString();
    QString filter = "Shaders (*.frag *.vert *.geom *.fs *.vs *.gs);;All files (*)";
    QString file = QFileDialog::getOpenFileName(this, "Open a shader file", dir, filter);
    if (!file.isEmpty()) {
      QFileInfo fi(file);

      Shader::Type type = Shader::guessType(fi.absoluteFilePath());
      if (type == Shader::Unknown) {
        QMenu menu("Type of the shader", this);
        menu.addAction(Shader::icon(Shader::Vertex), "Vertex shader")->setData(Shader::Vertex);
        menu.addAction(Shader::icon(Shader::TessCtrl), "Tesselation control shader")->setData(Shader::TessCtrl);
        menu.addAction(Shader::icon(Shader::TessEval), "Tesselation evaluation shader")->setData(Shader::TessEval);
        menu.addAction(Shader::icon(Shader::Geometry), "Geometry shader")->setData(Shader::Geometry);
        menu.addAction(Shader::icon(Shader::Fragment), "Fragment shader")->setData(Shader::Fragment);
        QAction* a = menu.exec(QCursor::pos());
        if (!a) return;
        type = (Shader::Type)a->data().toInt();
      }

      settings.setValue("history/last_import_dir", fi.absolutePath());
      m_material->prog(true)->addShader(fi.absoluteFilePath(), type);
    }
  }

  void MultiEditor::duplicate()
  {
    const Section* s = selected();
    if (!s) return;

    ShaderPtr shader = s->editor->shader();
    ProgramPtr prog = shader->program();
    if (!shader || !prog) return;

    ShaderPtr cloned = shader->clone(prog);

    cloned->setFilename(ResourceLocator::unique(cloned->filename(), MainWindow::scene()->filenames()));
    prog->addShader(cloned);
  }

  void MultiEditor::remove()
  {
    const Section* s = selected();
    if (!s) return;

    const QString& filename = s->editor->shader()->filename();
    QList<ShaderPtr> all = MainWindow::scene()->shaders(filename);

    ShaderPtr shader = s->editor->shader();
    ProgramPtr prog = shader->program();
    if (!shader || !prog) return;

    CheckBoxDialog dialog(QString("Remove %1 from the project.").arg(filename), all.size() == 1);
    dialog.setMinimumWidth(550);
    if (dialog.exec() == QDialog::Accepted) {
      if (prog->removeShader(shader) && all.size() == 1 && dialog.checked()) {
        Log::info("Removing file %s", filename.toUtf8().data());
        QFile::remove(filename);
      }
    }
  }

  void MultiEditor::autosize(QString filename)
  {
    Section s = m_sections.value(filename);
    if (s.editor) {
      QSize size = s.editor->document()->size().toSize();
      size.setWidth(size.width() + s.editor->marginWidth());
      s.editor->setMinimumSize(size);
    }
  }

  void MultiEditor::refresh()
  {
    foreach (QString res, m_sections.keys()) {
      autosize(res);
    }
    relayout();
  }

  void MultiEditor::scrollTo(QModelIndex idx)
  {
    QString file = idx.data(Qt::UserRole).toString();
    Section s = m_sections.value(file);
    if (s.header) {
      m_area->verticalScrollBar()->setValue(s.header->pos().y() - 5);
    }
  }

  void MultiEditor::focusOnError(ShaderError)
  {
    /// @todo implement
    /*  Editor* e = editor(error.res());
      if (e) {
        /// @todo this really won't work!
        e->focusOnError(error);
      }*/
    /*
      QTextCursor cursor =
      QMap<ShaderError, QTextCursor>
      setTextCursor(m_errors[error]);
      ensureCursorVisible();*/
  }

  QList<GLSLEditor*> MultiEditor::editors() const
  {
    QList<GLSLEditor*> ret;
    foreach (const Section& s, m_sections) {
      if (s.editor) ret << s.editor;
    }
    return ret;
  }

  bool MultiEditor::checkClose()
  {
    foreach (GLSLEditor* editor, editors()) {
      if (!editor->checkClose()) return false;
    }
    return true;
  }

  void MultiEditor::jump(bool up, GLSLEditor* editor)
  {
    QLayout* l = m_canvas->layout();
    GLSLEditor* prev = 0;
    bool next = false;
    for (int i = 0, m = l->count(); i < m; ++i) {
      QWidgetItem* wi = dynamic_cast<QWidgetItem*>(l->itemAt(i));
      if (!wi) continue;
      GLSLEditor* tmp = dynamic_cast<GLSLEditor*>(wi->widget());
      if (!tmp) continue;

      if (next) {
        tmp->setFocus();
        QTextCursor c = tmp->textCursor();
        c.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
        tmp->setTextCursor(c);
        ensureCursorVisible(tmp);
        break;
      } else if (editor == tmp) {
        if (up) {
          if (prev) {
            prev->setFocus();
            QTextCursor c = prev->textCursor();
            c.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
            prev->setTextCursor(c);
            ensureCursorVisible(prev);
          }
          break;
        } else next = true;
      } else {
        prev = tmp;
      }
    }
  }

  void MultiEditor::showEvent(QShowEvent* event)
  {
    QFrame::showEvent(event);
    refresh();
  }

  void MultiEditor::saveMaterial()
  {
    foreach (GLSLEditor* e, editors()) {
      if (e->document()->isModified()) {
        e->save();
      }
    }
  }

  GLSLEditor* MultiEditor::editor(const QString& filename) const
  {
    foreach (const Section& s, m_sections) {
      if (s.editor && s.editor->shader()->filename() == filename)
        return s.editor;
    }
    return 0;
  }

  const MultiEditor::Section* MultiEditor::selected() const
  {
    auto lst = m_list->selectedItems();
    if (lst.size() == 1) {
      foreach (const Section& s, m_sections) {
        if (s.item != lst[0]) continue;
        return &s;
      }
    }
    return 0;
  }

} // namespace Shaderkit
