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

#ifndef EDITOR_H
#define EDITOR_H

#include "forward.hpp"
#include "editor/highlighter.hpp"
#include "gl/error.hpp"
#include "core/watcher.hpp"

#include <QModelIndex>
#include <QListWidget>
#include <QDialog>

class QSignalMapper;
class QListWidgetItem;
class QLabel;
class QScrollArea;
class QSplitter;
class QCheckBox;

namespace Shaderkit
{

  class CheckBoxDialog : public QDialog
  {
  public:
    CheckBoxDialog(const QString& text, bool show_checkbox);
    bool checked() const;

  private:
    QCheckBox* m_checkbox;
  };

  /**
   * The (left) margin on the text editor, that shows the line numbers.
   *
   * This class is implemented like the code editor example in Qt documentation.
   */
  class EditorMargin : public QWidget
  {
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

  class FileListWidget : public QListWidget
  {
  public:
    FileListWidget(QWidget* parent);
    int preferredWidth();
  };

  class MultiEditor : public QFrame
  {
    Q_OBJECT

  public:
    MultiEditor(QWidget* parent, MaterialPtr material);
    virtual ~MultiEditor() {}

    MaterialPtr material() const { return m_material; }
    void saveMaterial();

    GLSLEditor* editor(const QString& filename) const;

    /// Moves the focus to given error
    void focusOnError(ShaderError error);

    QList<GLSLEditor*> editors() const;

    bool checkClose();

    QScrollArea* area() const { return m_area; }

    void jump(bool up, GLSLEditor* editor);

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
    void ensureCursorVisible(GLSLEditor* ed = 0);

    void itemChanged(QListWidgetItem*);
    void shaderChanged(ShaderPtr);
    void selectionChanged();
    void docModificationChanged(bool);

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

    QWidget* m_canvas;
    FileListWidget* m_list;
    QScrollArea* m_area;
    QSplitter* m_splitter;

    MaterialPtr m_material;

    QAction* m_create, *m_open, *m_duplicate, *m_destroy;

    /// key is a filename
    QMap<QString, Section> m_sections;

    QSignalMapper* m_mapper;
  };

} // namespace Shaderkit

#endif // EDITOR_H
