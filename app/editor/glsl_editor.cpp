#include "editor/glsl_editor.hpp"
#include "editor/editor.hpp"

#include "core/scene.hpp"

#include "gl/shader.hpp"

#include "gui/mainwindow.hpp"

#include <QScrollArea>
#include <QScrollBar>
#include <QToolTip>
#include <QMessageBox>

GLSLEditor::GLSLEditor(MultiEditor& me, QWidget* parent, ShaderPtr shader, QTextDocument* doc)
  : QTextEdit(parent), m_multiEditor(me), m_margin(new EditorMargin(this)),
    m_shader(shader), m_marginWidth(10) {

  if (doc) setDocument(doc);

  m_highlighter = new Highlighter(document(), m_parser);

  m_process_timeout = new AfterIdleOperation(this, 0.4f, 1.0f, 120.0f);
  connect(this, SIGNAL(textChanged()), m_process_timeout, SLOT(action()));
  connect(m_process_timeout, SIGNAL(timeout()), this, SLOT(process()));

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

void GLSLEditor::keyPressEvent(QKeyEvent* e) {
  if (e == QKeySequence::MoveToPreviousPage) {
    scrollPage(true, true);
  } else if (e == QKeySequence::MoveToNextPage) {
    scrollPage(false, true);
  } else if (e == QKeySequence::SelectPreviousPage) {
    scrollPage(true, false);
  } else if (e == QKeySequence::SelectNextPage) {
    scrollPage(false, false);
  } else if (e == QKeySequence::MoveToNextLine || e == QKeySequence::MoveToPreviousLine) {
    QTextCursor c = textCursor();
    QTextEdit::keyPressEvent(e);
    if (c == textCursor())
      m_multiEditor.jump(e == QKeySequence::MoveToPreviousLine, this);
  } else {
    QTextEdit::keyPressEvent(e);
  }
}

void GLSLEditor::focusInEvent(QFocusEvent* event) {
  updateExtraSelections();
  QTextEdit::focusInEvent(event);
}

void GLSLEditor::focusOutEvent(QFocusEvent* event) {
  updateExtraSelections();
  QTextEdit::focusOutEvent(event);
}

void GLSLEditor::scrollPage(bool up, bool moveAnchor) {
  QTextCursor c = textCursor();
  bool moved = false;
  qreal lastY = cursorRect(c).top();
  qreal distance = 0;
  // move using movePosition to keep the cursor's x
  do {
    qreal y = cursorRect(c).top();
    distance += qAbs(y - lastY);
    lastY = y;
    moved = c.movePosition(up ? QTextCursor::Up : QTextCursor::Down,
                           moveAnchor ? QTextCursor::MoveAnchor : QTextCursor::KeepAnchor);
  } while (moved && distance < m_multiEditor.area()->height());

  if (moved) {
    if (up) {
      c.movePosition(QTextCursor::Down, moveAnchor ? QTextCursor::MoveAnchor : QTextCursor::KeepAnchor);
      verticalScrollBar()->triggerAction(QAbstractSlider::SliderPageStepSub);
    } else {
      c.movePosition(QTextCursor::Up, moveAnchor ? QTextCursor::MoveAnchor : QTextCursor::KeepAnchor);
      verticalScrollBar()->triggerAction(QAbstractSlider::SliderPageStepAdd);
    }
  }
  if (c == textCursor()) {
    m_multiEditor.jump(up, this);
  } else {
    setTextCursor(c);
  }
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

    c.select(QTextCursor::WordUnderCursor);
    std::string token = c.selectedText().toStdString();
    if(!token.empty()) {
      auto& macros = m_parser.pp().macros();
      auto map_it = macros.find(c.blockNumber());
      if (map_it != macros.end()) {
        const std::list<GLSLpp::MacroValue>& list = map_it->second;
        for (auto it = list.begin(); it != list.end(); ++it) {
          if (it->name == token) {
            const GLSLpp::MacroValue& value = *it;
            QString txt;
            if (value.params.empty()) {
              txt = QString("#define %1 <b>%2</b>").arg(Qt::escape(value.name.c_str()),
                                                        Qt::escape(value.src.c_str()));
            } else {
              QStringList tmp;
              for (int i = 0; i < value.params.size(); ++i)
                tmp << value.params[i].c_str();
              if (value.value.empty()) {
                txt = QString("#define %1(%2) %3").arg(
                      Qt::escape(value.name.c_str()), Qt::escape(tmp.join(", ")),
                      Qt::escape(value.src.c_str()));
              } else {
                txt = QString("#define %1(%2) %3\n\n &raquo; <b>%4</b>").arg(
                      Qt::escape(value.name.c_str()), Qt::escape(tmp.join(", ")),
                      Qt::escape(value.src.c_str()), Qt::escape(value.value.c_str()));
              }
            }
            QToolTip::showText(he->globalPos(), "<p style=\"white-space:pre\">"+txt+"</p>");
            return true;
          }
        }
      }
    }

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
  QFile file(m_shader->filename());
  if (file.open(QIODevice::WriteOnly)) {
    file.write(toPlainText().toUtf8());
    document()->setModified(false);
  }
}

bool GLSLEditor::checkClose() {
  if (document()->isModified()) {
    int ret = QMessageBox::question(this, "Unsaved changes", QString(
                                    "The shader %1 has some unsaved changes, what to do?").
                                    arg(shader()->filename()),
                                    QMessageBox::Save | QMessageBox::Close | QMessageBox::Cancel);
    if (ret == QMessageBox::Save) {
      save();
    } else if (ret != QMessageBox::Close) {
      return false;
    }
  }
  return true;
}

void GLSLEditor::updateExtraSelections() {
  QList<QTextEdit::ExtraSelection> extraSelections;

  if (hasFocus())
    extraSelections.append(m_currentLineSelection);
  extraSelections.append(m_warningSelections);
  extraSelections.append(m_errorSelections);
  extraSelections.append(m_commentedSelections);

  setExtraSelections(extraSelections);
}

void GLSLEditor::highlightCurrentLine() {
  if (isReadOnly()) return;

  m_currentLineSelection.cursor = textCursor();
  m_currentLineSelection.cursor.clearSelection();

  updateExtraSelections();
}

void GLSLEditor::process() {
  m_commentedSelections.clear();
  if (m_parser.parse(toPlainText().toUtf8())) {
    QTextEdit::ExtraSelection in_comment;

    in_comment.format.setBackground(QColor(235, 235, 235));
    in_comment.format.setProperty(QTextFormat::FullWidthSelection, true);
    in_comment.cursor = QTextCursor(document());

    const std::vector<bool>& ln = m_parser.pp().lineValues();
    for (int i = 0; i < ln.size(); ++i) {
      if (ln[i]) continue;
      const int anchor = document()->findBlockByLineNumber(i).position();
      in_comment.cursor.setPosition(anchor);
      m_commentedSelections.append(in_comment);
    }
  }
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
      foreach (ShaderPtr shader, MainWindow::scene()->shaders(m_shader->filename()))
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
