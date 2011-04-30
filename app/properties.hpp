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

#ifndef PROPERTIES_HPP
#define PROPERTIES_HPP

#include "forward.hpp"
#include "shader/uniform.hpp"

#include <QMap>
#include <QTableWidget>

class UEditor : public QObject, public QTableWidgetItem {
public:
  UEditor(QTableWidget* w, int row, MaterialPtr mat, UniformVar& var);
  virtual ~UEditor() {}
  virtual void updateUI(UniformVar& var) = 0;

  MaterialPtr mat;
  QString name;
};

class FloatEditor : public UEditor {
  Q_OBJECT

public:
  FloatEditor(QTableWidget* w, int row, MaterialPtr mat, UniformVar& var);
  virtual ~FloatEditor();

  void updateUI(UniformVar& var);

  QLineEdit* edit;
  QSlider* slider;
  float min, max;

private slots:
  void editingFinished();
  void valueChanged(int);
  void reset();
  UniformVar* getVar();

private:
  QAction* m_reset_action;
};

class Properties : public QTreeWidget {
  Q_OBJECT

public:
  Properties(QWidget* parent = 0);
  virtual ~Properties() {}

protected:
};

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

signals:
  void select(MaterialPtr);

public slots:
  /// This shader program in given render pass has changed (usually just relinked)
  void update(MaterialPtr mat);
  void setActiveMaterials(QSet<MaterialPtr> materials);
  void updateMaterialList(ScenePtr scene);
  void selectionChanged();

protected slots:
  void create();
  void load();
  void duplicate();
  void edit();
  void remove();
  void toggleMode();

  void itemSelected(QTableWidgetItem*);

protected:
  struct Sub {
    Sub() : item(0) {}
    ~Sub();
    QTableWidgetItem* item;
    QMap<QString, UEditor*> editors;
  };
  UEditor* createEditor(MaterialPtr mat, UniformVar& var,
                        const ShaderTypeInfo& type, QTableWidgetItem* p);
  virtual void startDrag(Qt::DropActions supportedActions);
  virtual void contextMenuEvent(QContextMenuEvent* e);
  MaterialPtr get(QTableWidgetItem*& item) const;

  bool viewportEvent(QEvent* event);
  QItemSelectionModel::SelectionFlags selectionCommand(const QModelIndex& index,
                                                       const QEvent* event) const;

  QMap<MaterialPtr, Sub> m_materials;

  QAction *m_only_uniforms, *m_create, *m_open, *m_duplicate, *m_edit, *m_destroy;
  int m_hover_row;

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
