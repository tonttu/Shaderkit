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

#include "editor.hpp"
#include "material.hpp"
#include "resource_locator.hpp"
#include "shader/program.hpp"
#include "mainwindow.hpp"
#include "scene.hpp"

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

#include <cassert>

EditorMargin::EditorMargin(GLSLEditor* editor) : QWidget(editor), m_editor(editor) {}

QSize EditorMargin::sizeHint() const {
  return QSize(m_editor->marginWidth(), 0);
}

void EditorMargin::paintEvent(QPaintEvent* event) {
  m_editor->marginPaintEvent(event);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

GLSLEditor::GLSLEditor(MultiEditor& me, QWidget* parent, ShaderPtr shader, QTextDocument* doc)
  : QTextEdit(parent), m_multiEditor(me), m_margin(new EditorMargin(this)),
    m_shader(shader), m_marginWidth(10) {

  if (doc) setDocument(doc);

  m_highlighter = new Highlighter(document());

  setObjectName("editor");

  connect(document(), SIGNAL(blockCountChanged(int)), this, SLOT(updateMarginWidth(int)));
  connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));
  connect(this, SIGNAL(textChanged()), this, SLOT(textChangedSlot()));

  /// @todo these should read from configure or something similar
  QFont font;
  font.setFamily("Monospace");
  font.setFixedPitch(true);
  font.setPointSize(10);
  font.setStyleStrategy(QFont::PreferAntialias);

  setFont(font);

  QPalette p = palette();
  p.setColor(QPalette::Base, Qt::white);
  setPalette(p);

  QColor lineColor = QColor(Qt::yellow).lighter(180);
  m_currentLineSelection.format.setBackground(lineColor);
  m_currentLineSelection.format.setProperty(QTextFormat::FullWidthSelection, true);

  // tab stop is 2 characters. That has to be multiplied with 8 to get the correct result.
  /// @todo these should be configurable
  setTabStopWidth(2*8);
  setLineWrapMode(NoWrap);

  updateMarginWidth(0);
  highlightCurrentLine();

  setFrameShape(QFrame::NoFrame);

  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

GLSLEditor::~GLSLEditor() {
}

void GLSLEditor::resizeEvent(QResizeEvent* e) {
  QTextEdit::resizeEvent(e);

  QRect cr = contentsRect();
  m_margin->setGeometry(QRect(cr.left(), cr.top(), marginWidth(), cr.height()));
}

void GLSLEditor::clearErrors() {
  if (m_errors.isEmpty()) return;

  m_errorSelections.clear();
  m_warningSelections.clear();
  m_errorLines.clear();
  m_warningLines.clear();
  m_errors.clear();
  m_highlighter->rehighlight();
  updateExtraSelections();
}

/*void GLSLEditor::readFile(const QString& res) {
  QFile qfile(res);
  if (qfile.open(QFile::ReadOnly | QFile::Text)) {
    m_res = res;
    QString tmp = qfile.readAll();
    if (m_lastdata != tmp) {
      setPlainText(tmp);
    }
  }
}*/

/*void GLSLEditor::fileUpdated(const QString& res) {
  QFile qfile(res);
  if (qfile.open(QFile::ReadOnly | QFile::Text)) {
    m_res = res;
    QString tmp = qfile.readAll();
    if (m_lastdata == tmp) return;

    if (!document()->isModified()) {
      setPlainText(tmp);
      document()->setModified(false);
      /// @todo signal something that forces recompile
    } else if (QMessageBox::question(this,
          tr("File change"),
          tr("<p>File %1 was changed on the disk, but there are some unsaved "
             "changed to the code in the editor.<p>Load the file from the disk and "
             "discard all the changes?").arg(res),
          QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
      setPlainText(tmp);
      document()->setModified(false);
    }
  }
}*/

bool GLSLEditor::viewportEvent(QEvent* event) {
  if (event->type() == QEvent::ToolTip) {
    const QHelpEvent* he = static_cast<QHelpEvent*>(event);

    QTextCursor c = cursorForPosition(he->pos());
    int pos = c.position();

    /// @todo Implement this somehow more generic
    for (int i = 0; i < m_errorSelections.size(); ++i) {
      const QTextEdit::ExtraSelection& s = m_errorSelections.at(i);
      if (s.cursor.selectionStart() <= pos && s.cursor.selectionEnd() >= pos &&
          !s.format.toolTip().isEmpty()) {
        QToolTip::showText(he->globalPos(), s.format.toolTip());
        return true;
      }
    }
    for (int i = 0; i < m_warningSelections.size(); ++i) {
      const QTextEdit::ExtraSelection& s = m_warningSelections.at(i);
      if (s.cursor.selectionStart() <= pos && s.cursor.selectionEnd() >= pos &&
          !s.format.toolTip().isEmpty()) {
        QToolTip::showText(he->globalPos(), s.format.toolTip());
        return true;
      }
    }
  }
  return QTextEdit::viewportEvent(event);
}

void GLSLEditor::save() {
  /// @todo atomic writing
  QFile file(m_shader->res());
  if (file.open(QIODevice::WriteOnly)) {
    file.write(toPlainText().toUtf8());
    document()->setModified(false);
  }
}

void GLSLEditor::updateExtraSelections() {
  QList<QTextEdit::ExtraSelection> extraSelections;

  extraSelections.append(m_currentLineSelection);
  extraSelections.append(m_warningSelections);
  extraSelections.append(m_errorSelections);

  setExtraSelections(extraSelections);
}

void GLSLEditor::highlightCurrentLine() {
  if (isReadOnly()) return;

  m_currentLineSelection.cursor = textCursor();
  m_currentLineSelection.cursor.clearSelection();

  updateExtraSelections();
}

void GLSLEditor::compileError(const ShaderError& e) {
  if (isReadOnly()) return;

  bool warning = e.type() == "warning";
  QSet<int>& lines = warning ? m_warningLines : m_errorLines;
  QList<QTextEdit::ExtraSelection>& sel = warning ? m_warningSelections : m_errorSelections;

  if (!lines.contains(e.line())) {
    lines.insert(e.line());
    QTextEdit::ExtraSelection selection;

    QColor lineColor = QColor(warning ? Qt::yellow : Qt::red).lighter(180);
    selection.format.setBackground(lineColor);
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);

    const int anchor = document()->findBlockByLineNumber(e.line()).position();

    selection.cursor = QTextCursor(document());
    selection.cursor.setPosition(anchor);

    sel.append(selection);
  }

  QTextEdit::ExtraSelection selection;

  selection.format.setUnderlineColor(warning ? Qt::yellow : Qt::red);
  selection.format.setUnderlineStyle(QTextCharFormat::WaveUnderline);

  const int anchor = document()->findBlockByLineNumber(e.line()).position() + e.column();

  selection.cursor = QTextCursor(document());
  selection.cursor.setPosition(anchor);
  // Save the beginning position as the error position
  m_errors[e] = selection.cursor;

  selection.cursor.setPosition(anchor + e.length(), QTextCursor::KeepAnchor);
  selection.format.setToolTip(e.msg());

  sel.append(selection);

  updateExtraSelections();
}

void GLSLEditor::textChangedSlot() {
  QString tmp = toPlainText();
  if (m_lastdata != tmp) {
    m_lastdata = tmp;
    if (MainWindow::instance().autoCompileEnabled() && MainWindow::scene()) {
      foreach (ShaderPtr shader, MainWindow::scene()->shaders(m_shader->res()))
        shader->loadSrc(tmp);
    }
  }
}

void GLSLEditor::marginPaintEvent(QPaintEvent* event) {
  QPainter painter(m_margin);
  painter.fillRect(event->rect(), QColor(Qt::lightGray).lighter(120));

  QTextDocument* doc = document();
  QAbstractTextDocumentLayout* l = doc->documentLayout();

  QTextBlock block = doc->begin();
  int blockNumber = block.blockNumber();
  int top = l->blockBoundingRect(block).top();
  int bottom = top + l->blockBoundingRect(block).height();

  while (block.isValid() && top <= event->rect().bottom()) {
    if (block.isVisible() && bottom >= event->rect().top()) {
      QString number = QString::number(blockNumber + 1);
      painter.setPen(Qt::gray);
      painter.drawText(0, top, m_margin->width()-5, fontMetrics().height(),
                       Qt::AlignRight, number);
    }

    block = block.next();
    if(block.isValid()) {
      top = bottom;
      bottom = top + l->blockBoundingRect(block).height();
      ++blockNumber;
    }
  }
}

void GLSLEditor::updateMarginWidth(int blockCount) {
  int digits = 1;
  int lines = qMax(1, blockCount);
  while (lines >= 10) lines /= 10, ++digits;

  m_marginWidth = 8 + fontMetrics().width(QLatin1Char('9')) * digits;
  setViewportMargins(m_marginWidth, 0, 0, 0);
}

int GLSLEditor::marginWidth() const {
  return m_marginWidth;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FileListWidget::FileListWidget(QWidget* parent)
  : QListWidget(parent) {}

int FileListWidget::preferredWidth() {
  return contentsSize().width() + 10;
}

MultiEditor::MultiEditor(QWidget* parent, MaterialPtr material)
  : QFrame(parent),
    m_viewport(new QWidget(this)),
    m_list(new FileListWidget(this)),
    m_material(material),
    m_mapper(new QSignalMapper(this)) {
  setFrameShape(QFrame::NoFrame);

  QVBoxLayout* main_layout = new QVBoxLayout(this);
  main_layout->setContentsMargins(0, 0, 0, 0);

  m_splitter = new QSplitter(Qt::Horizontal, this);

  m_area = new QScrollArea(m_splitter);
  m_area->setWidgetResizable(true);
  m_area->setWidget(m_viewport);
  m_area->setFrameShape(QFrame::NoFrame);

  m_list->setFrameShape(QFrame::NoFrame);

  m_splitter->addWidget(m_list);
  m_splitter->addWidget(m_area);
  m_splitter->setStretchFactor(0, 0);
  m_splitter->setStretchFactor(1, 1);
  m_splitter->setCollapsible(1, false);

  main_layout->addWidget(m_splitter);

  new QVBoxLayout(m_viewport);

  connect(m_mapper, SIGNAL(mapped(QString)), this, SLOT(autosize(QString)));

  ProgramPtr prog = m_material->prog();
  if (prog) {
    foreach(ShaderPtr shader, prog->shaders())
      addShader(shader);
    connect(prog.get(), SIGNAL(changed()), this, SLOT(materialChanged()));
  }
  connect(m_material.get(), SIGNAL(changed()), this, SLOT(materialChanged()));

  connect(m_list, SIGNAL(doubleClicked(QModelIndex)),
          this, SLOT(scrollTo(QModelIndex)));
}

void MultiEditor::addShader(ShaderPtr shader) {
  QList<GLSLEditor*> editors = MainWindow::instance().findEditors(shader);
  QTextDocument* doc = editors.isEmpty() ? 0 : editors[0]->document();

  QFile f(shader->res());
  if(doc || f.open(QFile::ReadOnly)) {
    Section& s = m_sections[shader->res()];

    s.item = new QListWidgetItem(shader->icon(), ResourceLocator::ui(shader->res()));
    s.item->setFlags(s.item->flags() | Qt::ItemIsUserCheckable);
    /// @todo handle hide/show
    s.item->setCheckState(Qt::Checked);
    s.item->setData(Qt::UserRole, shader->res());
    m_list->addItem(s.item);

    s.header = new QWidget(m_viewport);
    QHBoxLayout* l = new QHBoxLayout(s.header);

    s.label = new QLabel("<b>"+ResourceLocator::ui(shader->res())+"</b>", s.header);
    s.icon = new QLabel(s.header);
    s.icon->setPixmap(shader->icon().pixmap(16));
    l->addWidget(s.icon);
    l->addWidget(s.label);
    l->addStretch();
    l->setContentsMargins(2, 2, 0, 0);
    m_viewport->layout()->addWidget(s.header);

    s.editor = new GLSLEditor(*this, m_viewport, shader, doc);

    m_viewport->layout()->addWidget(s.editor);

    m_mapper->setMapping(s.editor->document()->documentLayout(), shader->res());
    connect(s.editor->document()->documentLayout(), SIGNAL(documentSizeChanged(QSizeF)),
            m_mapper, SLOT(map()));

    connect(s.editor->document(), SIGNAL(modificationChanged(bool)),
            this, SLOT(editorModified(bool)));
    connect(s.editor, SIGNAL(cursorPositionChanged()),
            this, SLOT(ensureCursorVisible()));

    if (!doc) {
      s.editor->setText(f.readAll());
    } else {
      /// @todo this is not enough, maybe using a timer?
      /// @todo ensurePolished?
      autosize(shader->res());
    }
    QTimer::singleShot(50, this, SLOT(relayout()));
  }
}

void MultiEditor::relayout() {
  int w = m_list->preferredWidth();
  m_list->setMaximumWidth(w);
  QList<int> sizes = m_splitter->sizes();
  sizes[0] = w;
  m_splitter->setSizes(sizes);
}

void MultiEditor::editorModified(bool b) {
  MainWindow::instance().modificationChanged(this, b);
}

void MultiEditor::materialChanged() {
  ProgramPtr prog = m_material->prog();
  if (prog) {
    QSet<ShaderPtr> target = prog->shaders();
    QSet<ShaderPtr> current;
    foreach (const Section& s, m_sections)
      current << s.editor->shader();

    foreach (ShaderPtr s, current - target) {
      /// @todo remove shader
    }
    foreach (ShaderPtr s, target - current) {
      addShader(s);
    }
  }
}

void MultiEditor::ensureCursorVisible() {
  GLSLEditor* ed = qobject_cast<GLSLEditor*>(sender());
  assert(ed);
  QRect r = ed->cursorRect().adjusted(-40, -10, 40, 10);
  r.moveCenter(ed->viewport()->mapTo(m_viewport, r.center()));
  m_area->ensureVisible(r.center().x(), r.center().y(),
                        r.width(), r.height());
}

void MultiEditor::autosize(QString res) {
  Section s = m_sections.value(res);
  if (s.editor) {
    QSize size = s.editor->document()->size().toSize();
    size.setWidth(size.width() + s.editor->marginWidth());
    s.editor->setMinimumSize(size);
  }
}

void MultiEditor::refresh() {
  foreach (QString res, m_sections.keys()) {
    autosize(res);
  }
  relayout();
}

void MultiEditor::scrollTo(QModelIndex idx) {
  QString file = idx.data(Qt::UserRole).toString();
  Section s = m_sections.value(file);
  if (s.header) {
    m_area->verticalScrollBar()->setValue(s.header->pos().y() - 5);
  }
}

void MultiEditor::focusOnError(ShaderError error) {
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

QList<GLSLEditor*> MultiEditor::editors() const {
  QList<GLSLEditor*> ret;
  foreach (const Section& s, m_sections) {
    if (s.editor) ret << s.editor;
  }
  return ret;
}

void MultiEditor::showEvent(QShowEvent* event) {
  QFrame::showEvent(event);
  refresh();
}

void MultiEditor::saveMaterial() {
  foreach (GLSLEditor* e, editors()) {
    if (e->document()->isModified()) {
      e->save();
    }
  }
}

GLSLEditor* MultiEditor::editor(QString res) const {
  foreach (const Section& s, m_sections) {
    /// @todo handle other res choices too (material, shader)
    if (s.editor && s.editor->shader()->res() == res)
      return s.editor;
  }
  return 0;
}
