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

#include <QPainter>
#include <QTextBlock>
#include <QFile>
#include <QToolTip>
#include <QMessageBox>

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
  : QPlainTextEdit(parent), m_margin(new EditorMargin(this)),
    m_highlighter(new Highlighter(this->document())), m_shader(shader), m_sync(true) {
  setObjectName("editor");

  connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateMarginWidth(int)));
  connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateMargin(QRect,int)));
  connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));
  connect(this, SIGNAL(textChanged()), this, SLOT(textChangedSlot()));

  /// @todo these should read from configure or something similar
  QFont font;
  font.setFamily("Monospace");
  font.setFixedPitch(true);
  font.setPointSize(10);
  font.setStyleStrategy(QFont::PreferAntialias);

  setFont(font);

  QColor lineColor = QColor(Qt::yellow).lighter(180);
  m_currentLineSelection.format.setBackground(lineColor);
  m_currentLineSelection.format.setProperty(QTextFormat::FullWidthSelection, true);

  // tab stop is 2 characters. That has to be multiplied with 8 to get the correct result.
  setTabStopWidth(2*8);
  setLineWrapMode(NoWrap);

  updateMarginWidth(0);
  highlightCurrentLine();
}

void Editor::updateMargin(const QRect& rect, int dy) {
  if (dy)
    m_margin->scroll(0, dy);
  else
    m_margin->update(0, rect.y(), m_margin->width(), rect.height());

  if (rect.contains(viewport()->rect()))
    updateMarginWidth(0);
}

void Editor::resizeEvent(QResizeEvent* e) {
  QPlainTextEdit::resizeEvent(e);

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
  centerCursor();
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
  return QPlainTextEdit::viewportEvent(event);
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

  QTextBlock block = firstVisibleBlock();
  int blockNumber = block.blockNumber();
  int top = static_cast<int>(blockBoundingGeometry(block).translated(contentOffset()).top());
  int bottom = top + static_cast<int>(blockBoundingRect(block).height());

  while (block.isValid() && top <= event->rect().bottom()) {
    if (block.isVisible() && bottom >= event->rect().top()) {
      QString number = QString::number(blockNumber + 1);
      painter.setPen(Qt::gray);
      painter.drawText(0, top, m_margin->width()-5, fontMetrics().height(),
                       Qt::AlignRight, number);
    }

    block = block.next();
    top = bottom;
    bottom = top + static_cast<int>(blockBoundingRect(block).height());
    ++blockNumber;
  }
}

void Editor::updateMarginWidth(int /*blockCount*/) {
  setViewportMargins(marginWidth(), 0, 0, 0);
}

int Editor::marginWidth() const {
  int digits = 1;
  int lines = qMax(1, blockCount());
  while (lines >= 10) lines /= 10, ++digits;

  return 8 + fontMetrics().width(QLatin1Char('9')) * digits;
}
