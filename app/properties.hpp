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

class UEditor : public QObject, public QTreeWidgetItem {
public:
  UEditor(QTreeWidgetItem* p, MaterialPtr mat, UniformVar& var);
  virtual ~UEditor() {}
  virtual void updateUI(UniformVar& var) = 0;

  MaterialPtr mat;
  QString name;
};

class FloatEditor : public UEditor {
  Q_OBJECT

public:
  FloatEditor(QTreeWidgetItem* p, MaterialPtr mat, UniformVar& var);
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
class MaterialProperties : public Properties {
  Q_OBJECT

public:
  static MaterialProperties& instance();
  MaterialProperties(QWidget* parent = 0);
  virtual ~MaterialProperties();

public slots:
  /// This shader program in given render pass has changed (usually just relinked)
  void update(MaterialPtr mat);
  void setMaterials(QSet<MaterialPtr> materials);
  /// Remove a shader that is in given pass from the property list
  //void remove(MaterialPtr mat);

protected:
  struct Sub {
    Sub() : item(0) {}
    ~Sub();
    QTreeWidgetItem* item;
    QMap<QString, std::shared_ptr<UEditor> > editors;
  };

  UEditor* createEditor(MaterialPtr mat, UniformVar& var,
                        const ShaderTypeInfo& type, QTreeWidgetItem* p);

  QMap<MaterialPtr, Sub> m_materials;

  static MaterialProperties* s_instance;
};

/**
 * List of all available editable files.
 *
 * This is a singleton class.
 */
class FileList : public QTreeWidget {
  Q_OBJECT

public:
  static FileList& instance();
  FileList(QWidget* parent = 0);
  virtual ~FileList();

signals:
  void openFile(ShaderPtr);

public slots:
  void update(ShaderPtr shader);
  void activateFile(QTreeWidgetItem* item, int column);
  void remove(ShaderPtr shader);

protected:
  QMap<QString, QTreeWidgetItem*> m_files;
  QMap<QTreeWidgetItem*, ShaderPtr> m_items;
  QTreeWidgetItem* m_src;

  static FileList* s_instance;
};

#endif // PROPERTIES_HPP
