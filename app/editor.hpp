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

#ifndef EDITOR_H
#define EDITOR_H

#include "forward.hpp"
#include "highlighter.hpp"
#include "shader/error.hpp"
#include "watcher.hpp"

#include <QPlainTextEdit>
#include <QModelIndex>
#include <QListWidget>
#include <QDialog>

class QSignalMapper;
class QListWidgetItem;
class QLabel;
class QScrollArea;
class QSplitter;
class QCheckBox;

class CheckBoxDialog : public QDialog {
public:
  CheckBoxDialog(QString text, bool show_checkbox);
  bool checked() const;

private:
  QCheckBox* m_checkbox;
};

/**
 * The (left) margin on the text editor, that shows the line numbers.
 *
 * This class is implemented like the code editor example in Qt documentation.
 */
class EditorMargin : public QWidget {
  Q_OBJECT

public:
  EditorMargin(GLSLEditor* editor);

  /// Asks the width from Editor::marginWidth()
  QSize sizeHint() const;

protected:
  /// Asks the width from Editor::marginWidth()
  void paintEvent(QPaintEvent* event);

  GLSLEditor* m_editor;
};

/**
 * GLSL code editor.
 *
 * @todo Split Editor to GLSLEditor and Editor, so we could make plain text editor,
 *       JSON editor and maybe even C++ editor.
 */
class GLSLEditor : public QTextEdit {
  Q_OBJECT

public:
  GLSLEditor(MultiEditor& me, QWidget* parent, ShaderPtr shader, QTextDocument* doc);
  virtual ~GLSLEditor();

  /// Renders the line numbers.
  void marginPaintEvent(QPaintEvent* event);

  /// Margin width, that is, the width of the are with line numbers.
  int marginWidth() const;

  /// Opens the editor with the contents of file.
  //void readFile(const QString& res);

  /// The file was changed on the disk.
  //void fileUpdated(const QString& res);

  /// Forgets all compile errors.
  void clearErrors();

  /// We capture all events on the viewport to be able to show smart tooltips.
  virtual bool viewportEvent(QEvent* event);

  /// Return the shader the object is editing.
  ShaderPtr shader() { return m_shader; }

  void save();

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

private:
  MultiEditor& m_multiEditor;
  EditorMargin* m_margin;
  /// Syntax Highlight provider
  Highlighter* m_highlighter;

  ShaderPtr m_shader;

  /// Extra selections are used to highlight the current line and show
  /// compiler errors. When m_currentLineSelection or m_errorSelections etc
  /// are changed, this should be called to update the change to editor.
  void updateExtraSelections();

  QTextEdit::ExtraSelection m_currentLineSelection;
  QList<QTextEdit::ExtraSelection> m_errorSelections;
  QList<QTextEdit::ExtraSelection> m_warningSelections;

  /// All lines that have an error, starting from zero
  QSet<int> m_errorLines;
  /// All lines that have a warning, starting from zero
  QSet<int> m_warningLines;

  /// All errors and their locations
  QMap<ShaderError, QTextCursor> m_errors;

  /// Currently for every keypress we are updating the whole data to this
  /// variable to be able to actually separate correct codeChange
  /// signals from all of those textChanged-signals.
  /// @todo isn't there any better way to do this?
  QString m_lastdata;

  int m_marginWidth;
};

class FileListWidget : public QListWidget {
public:
  FileListWidget(QWidget* parent);
  int preferredWidth();
};

class MultiEditor : public QFrame {
  Q_OBJECT

public:
  MultiEditor(QWidget* parent, MaterialPtr material);
  virtual ~MultiEditor() {}

  MaterialPtr material() const { return m_material; }
  void saveMaterial();

  GLSLEditor* editor(QString res) const;

  /// Moves the focus to given error
  void focusOnError(ShaderError error);

  QList<GLSLEditor*> editors() const;

protected:
  virtual void showEvent(QShowEvent* event);

public slots:
  void refresh();

private slots:
  void autosize(QString);
  void scrollTo(QModelIndex);
  void relayout();
  void editorModified(bool);
  void materialChanged();
  void ensureCursorVisible();

  void itemChanged(QListWidgetItem*);
  void shaderChanged(ShaderPtr);
  void selectionChanged();

  void create();
  void load();
  void duplicate();
  void remove();

private:
  struct Section {
    Section() : item(0), header(0), editor(0), icon(0), label(0) {}
    QListWidgetItem* item;
    QWidget* header;
    GLSLEditor* editor;
    QLabel* icon;
    QLabel* label;
  };

  void addShader(ShaderPtr shader);
  const Section* selected() const;

  QWidget* m_viewport;
  FileListWidget* m_list;
  QScrollArea* m_area;
  QSplitter* m_splitter;

  MaterialPtr m_material;

  QAction *m_create, *m_open, *m_duplicate, *m_destroy;

  /// key is a res
  QMap<QString, Section> m_sections;

  QSignalMapper* m_mapper;
};

#endif // EDITOR_H
