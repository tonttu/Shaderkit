#ifndef EDITOR_H
#define EDITOR_H

#include "forward.hpp"
#include "highlighter.hpp"
#include "shader/error.hpp"

#include <QPlainTextEdit>

/**
 * The (left) margin on the text editor, that shows the line numbers.
 *
 * This class is implemented like the code editor example in Qt documentation.
 */
class EditorMargin : public QWidget {
  Q_OBJECT

public:
  EditorMargin(Editor* editor);

  /// Asks the width from Editor::marginWidth()
  QSize sizeHint() const;

protected:
  /// Asks the width from Editor::marginWidth()
  void paintEvent(QPaintEvent* event);

  Editor* m_editor;
};

/**
 * GLSL code editor.
 *
 * @todo Split Editor to GLSLEditor and Editor, so we could make plain text editor,
 *       JSON editor and maybe even C++ editor.
 */
class Editor : public QPlainTextEdit {
  Q_OBJECT

public:
  Editor(QWidget* parent = 0);

  /// Renders the line numbers.
  void marginPaintEvent(QPaintEvent* event);

  /// Margin width, that is, the width of the are with line numbers.
  int marginWidth() const;

  /// The filename of the file that is open in the editor.
  const QString& filename() const { return m_filename; }

  /// Opens the editor with the contents of filename.
  void readFile(const QString& filename);

  /// Moves the focus to error number idx, starting from zero.
  /// @todo some other way to identify the errors would be better than index.
  void focusOnError(int idx);

  /// Forgets all compile errors.
  void clearErrors();

  /// We capture all events on the viewport to be able to show smart tooltips.
  virtual bool viewportEvent(QEvent* event);

signals:
  /// When signaled, the code could be recompiled.
  void canRecompile(Editor&);
  /// Code was changed.
  void codeChange(Editor&);

protected:
  /// Capture the resize event to set the margin geometry correctly.
  void resizeEvent(QResizeEvent* event);

public slots:
  /// Adds a new error to list and sets the error text styles to correct place.
  void compileError(const ShaderError& e);
  /// For internal use, we wan't to separate real text changes from format changes.
  void textChangedSlot();

private slots:
  /// Called when line number is changed
  void updateMarginWidth(int blockCount);
  /// When the text cursor moves, this is called and the line is highlighted
  void highlightCurrentLine();
  void updateMargin(const QRect& rect, int dy);

private:
  EditorMargin* m_margin;
  /// Syntax Highlight provider
  Highlighter* m_highlighter;

  /// Extra selections are used to highlight the current line and show
  /// compiler errors. When m_currentLineSelection or m_errorSelections
  /// are changed, this should be called to update the change to editor.
  void updateExtraSelections();

  QTextEdit::ExtraSelection m_currentLineSelection;
  QList<QTextEdit::ExtraSelection> m_errorSelections;
  QList<QTextEdit::ExtraSelection> m_warningSelections;

  /// All lines that have an error, starting from zero
  QSet<int> m_errorLines;
  /// All lines that have a warning, starting from zero
  QSet<int> m_warningLines;

  QVector<ShaderError> m_errors;

  QString m_filename;

  /// Currently for every keypress we are updating the whole data to this
  /// variable to be able to actually separate correct codeChange
  /// signals from all of those textChanged-signals.
  /// @todo isn't there any better way to do this?
  QString m_lastdata;
};

#endif // EDITOR_H
