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
#include "shader/program.hpp"

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

EditorMargin::EditorMargin(Editor* editor) : QWidget(editor), m_editor(editor) {}

QSize EditorMargin::sizeHint() const {
  return QSize(m_editor->marginWidth(), 0);
}

void EditorMargin::paintEvent(QPaintEvent* event) {
  m_editor->marginPaintEvent(event);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

Editor::Editor(QWidget* parent, ShaderPtr shader)
  : QTextEdit(parent), m_margin(new EditorMargin(this)),
    m_highlighter(new Highlighter(this->document())), m_shader(shader), m_sync(true),
    m_marginWidth(10) {
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
  setTabStopWidth(2*8);
  setLineWrapMode(NoWrap);

  updateMarginWidth(0);
  highlightCurrentLine();

  setFrameShape(QFrame::NoFrame);

  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void Editor::resizeEvent(QResizeEvent* e) {
  QTextEdit::resizeEvent(e);

  QRect cr = contentsRect();
  m_margin->setGeometry(QRect(cr.left(), cr.top(), marginWidth(), cr.height()));
}

void Editor::clearErrors() {
  if (m_errors.isEmpty()) return;

  m_errorSelections.clear();
  m_warningSelections.clear();
  m_errorLines.clear();
  m_warningLines.clear();
  m_errors.clear();
  m_highlighter->rehighlight();
  updateExtraSelections();
}

void Editor::readFile(const QString& filename) {
  QFile qfile(filename);
  if (qfile.open(QFile::ReadOnly | QFile::Text)) {
    m_filename = filename;
    QString tmp = qfile.readAll();
    if (m_lastdata != tmp) {
      setPlainText(tmp);
    }
  }
}

void Editor::fileUpdated(const QString& filename) {
  QFile qfile(filename);
  if (qfile.open(QFile::ReadOnly | QFile::Text)) {
    m_filename = filename;
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
             "discard all the changes?").arg(filename),
          QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
      setPlainText(tmp);
      document()->setModified(false);
    }
  }
}

void Editor::focusOnError(ShaderError error) {
  setTextCursor(m_errors[error]);
  ensureCursorVisible();
}

bool Editor::viewportEvent(QEvent* event) {
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

void Editor::updateExtraSelections() {
  QList<QTextEdit::ExtraSelection> extraSelections;

  extraSelections.append(m_currentLineSelection);
  extraSelections.append(m_warningSelections);
  extraSelections.append(m_errorSelections);

  setExtraSelections(extraSelections);
}

void Editor::highlightCurrentLine() {
  if (isReadOnly()) return;

  m_currentLineSelection.cursor = textCursor();
  m_currentLineSelection.cursor.clearSelection();

  updateExtraSelections();
}

void Editor::compileError(const ShaderError& e) {
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

void Editor::textChangedSlot() {
  QString tmp = toPlainText();
  if (m_lastdata != tmp) {
    m_lastdata = tmp;
    emit codeChanged(*this);
  }
}

void Editor::syncToggled(bool sync) {
  m_sync = sync;
}

void Editor::marginPaintEvent(QPaintEvent* event) {
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

void Editor::updateMarginWidth(int blockCount) {
  int digits = 1;
  int lines = qMax(1, blockCount);
  while (lines >= 10) lines /= 10, ++digits;

  m_marginWidth = 8 + fontMetrics().width(QLatin1Char('9')) * digits;
  setViewportMargins(m_marginWidth, 0, 0, 0);
}

int Editor::marginWidth() const {
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
  setFrameShape(QFrame::StyledPanel);
  setFrameShadow(QFrame::Sunken);

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

  main_layout->addWidget(m_splitter);

  new QVBoxLayout(m_viewport);

  connect(m_mapper, SIGNAL(mapped(QString)), this, SLOT(autosize(QString)));

  ProgramPtr prog = m_material->prog();
  if (prog) {
    foreach(ShaderPtr shader, prog->shaders())
      addShader(shader);
  }

  connect(m_list, SIGNAL(doubleClicked(QModelIndex)),
          this, SLOT(scrollTo(QModelIndex)));
}

void MultiEditor::addShader(ShaderPtr shader) {
  QFileInfo fi(shader->filename());
  QFile f(shader->filename());
  if(f.open(QFile::ReadOnly)) {
    Section& s = m_sections[shader->filename()];

    s.item = new QListWidgetItem(shader->icon(), fi.fileName());
    s.item->setFlags(s.item->flags() | Qt::ItemIsUserCheckable);
    /// @todo handle hide/show
    s.item->setCheckState(Qt::Checked);
    s.item->setData(Qt::UserRole, shader->filename());
    m_list->addItem(s.item);

    QByteArray data = f.readAll();

    s.header = new QWidget(m_viewport);
    QHBoxLayout* l = new QHBoxLayout(s.header);

    s.label = new QLabel("<b>"+fi.fileName()+"</b>", s.header);
    s.icon = new QLabel(s.header);
    s.icon->setPixmap(shader->icon().pixmap(16));
    l->addWidget(s.icon);
    l->addWidget(s.label);
    l->addStretch();
    l->setContentsMargins(2, 2, 0, 0);
    m_viewport->layout()->addWidget(s.header);

    s.editor = new Editor(this, shader);

    m_viewport->layout()->addWidget(s.editor);

    m_mapper->setMapping(s.editor->document()->documentLayout(), shader->filename());
    connect(s.editor->document()->documentLayout(), SIGNAL(documentSizeChanged(QSizeF)),
            m_mapper, SLOT(map()));

    connect(s.editor->document(), SIGNAL(modificationChanged(bool)),
            this, SLOT(editorModified(bool)));

    s.editor->setText(data);
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
  emit modificationChanged(b);
}

void MultiEditor::autosize(QString file) {
  Section s = m_sections.value(file);
  if (s.editor) {
    QSize size = s.editor->document()->size().toSize();
    size.setWidth(size.width() + s.editor->marginWidth());
    s.editor->setMinimumSize(size);
  }
}

void MultiEditor::scrollTo(QModelIndex idx) {
  QString file = idx.data(Qt::UserRole).toString();
  Section s = m_sections.value(file);
  if (s.header) {
    m_area->verticalScrollBar()->setValue(s.header->pos().y() - 5);
  }
}

void MultiEditor::focusOnError(ShaderError error) {
  Editor* e = editor(error.shader());
  if (e) {
    /// @todo this really won't work!
    e->focusOnError(error);
  }
/*
  QTextCursor cursor =
  QMap<ShaderError, QTextCursor>
  setTextCursor(m_errors[error]);
  ensureCursorVisible();*/
}

void MultiEditor::syncToggled(bool sync) {

}

void MultiEditor::save() {
  /// @todo
}

Editor* MultiEditor::editor(ShaderPtr shader) const {
  foreach (const Section& s, m_sections) {
    /// @todo fix this so ->filename wouldn't be necessary
    if (s.editor->shader()->filename() == shader->filename())
      return s.editor;
  }
  return 0;
}
