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

#ifndef PROPERTIES_HPP
#define PROPERTIES_HPP

#include "forward.hpp"
#include "shader/uniform.hpp"

#include <QMap>
#include <QTableWidget>

/**
 * This line editor updates its size hints automatically based on the content.
 */
class LineEdit : public QLineEdit {
  Q_OBJECT

public:
  LineEdit(QWidget* parent = 0);

  QSize minimumSizeHint() const;
  QSize sizeHint() const;

private slots:
  void updateSizes();
};

class HeaderWidget;
class MaterialProperties;
class UniformEditor;

struct MaterialItem {
  MaterialPtr material;
  HeaderWidget* header;

  QPersistentModelIndex header_index;
  QPersistentModelIndex last_index;

  QMap<QString, UniformEditor*> uniform_editors;
};

class UniformEditor : public QObject {
public:
  UniformEditor(MaterialProperties& prop, int row, MaterialPtr mat, UniformVar& var);
  virtual ~UniformEditor() {}
  virtual void updateUI(UniformVar& var) = 0;

  QLabel* label() const { return m_label; }
  QPersistentModelIndex index() const { return m_index; }

protected:
  MaterialPtr m_mat;
  QString m_name;

  QLabel* m_label;

  QPersistentModelIndex m_index;
  UniformVar* getVar();
};

class FloatEditor : public UniformEditor {
  Q_OBJECT

public:
  FloatEditor(MaterialProperties& prop, int row, MaterialPtr mat, UniformVar& var);
  virtual ~FloatEditor();

  void updateUI(UniformVar& var);

private slots:
  void editingFinished();
  void valueChanged(int);
  void reset();

private:
  LineEdit* m_edit;
  QSlider* m_slider;
  float m_min, m_max;

  QAction* m_reset_action;
};

class TextureEditor : public UniformEditor {
  Q_OBJECT

public:
  TextureEditor(MaterialProperties& prop, int row, MaterialPtr mat, UniformVar& var);
  virtual ~TextureEditor();

  void updateUI(UniformVar& var);

private slots:
  void showPreview();
  void browse();

private:
  QLabel* m_texname;
  TextureWidgetGL* m_icon;
};

class Properties : public QTreeWidget {
  Q_OBJECT

public:
  Properties(QWidget* parent = 0);
  virtual ~Properties() {}

protected:
};

class PropertyLayoutData;

/**
 * Real-time property editor for shader properties, like uniform variables.
 *
 * This is a singleton class.
 */
class MaterialProperties : public QTableWidget {
  Q_OBJECT

public:
  static MaterialProperties& instance();
  MaterialProperties(QWidget* parent = 0);
  virtual ~MaterialProperties();

  void init();

  std::shared_ptr<PropertyLayoutData> layoutData() { return m_data; }

public slots:
  /// This shader program in given render pass has changed (usually just relinked)
  void update(MaterialPtr mat);
  void setActiveMaterials(QSet<MaterialPtr> materials);
  void updateMaterialList(ScenePtr scene);
  void selectionChanged();
  void itemSelected(int row, int column);

protected slots:
  void create();
  void load();
  void duplicate();
  void edit();
  void remove();
  void toggleMode();

  void materialChanged(MaterialPtr);

protected:
  QMap<MaterialPtr, MaterialItem> m_materials;
  UniformEditor* createEditor(MaterialPtr mat, UniformVar& var,
                              const ShaderTypeInfo& type, int row);
  virtual void startDrag(Qt::DropActions supportedActions);
  virtual void contextMenuEvent(QContextMenuEvent* e);

  MaterialPtr get(QModelIndex index) const;
  MaterialPtr getSelected() const;
  // bool viewportEvent(QEvent* event);

  QItemSelectionModel::SelectionFlags selectionCommand(const QModelIndex& index,
                                                       const QEvent* event) const;
  QAction *m_only_uniforms, *m_create, *m_open, *m_duplicate, *m_edit, *m_destroy;
  // int m_hover_row;

  std::shared_ptr<PropertyLayoutData> m_data;

  MaterialPtr m_selected;

  static MaterialProperties* s_instance;
};

/**
 * List of all available editable files.
 *
 * This is a singleton class.
 */
/*
class FileList : public QTreeWidget {
  Q_OBJECT

public:
  static FileList& instance();
  FileList(QWidget* parent = 0);
  virtual ~FileList();

  void init();

signals:
  void openFile(ShaderPtr);

public slots:
  void update(ShaderPtr shader);
  void activateFile(QTreeWidgetItem* item, int column);
  void remove(ShaderPtr shader);
  void selectionChanged();

protected:
  QMap<QString, QTreeWidgetItem*> m_files;
  QMap<QTreeWidgetItem*, ShaderPtr> m_items;
  QTreeWidgetItem* m_src;

  QAction *m_create, *m_open, *m_duplicate, *m_edit, *m_destroy;

  static FileList* s_instance;
};
*/
#endif // PROPERTIES_HPP
