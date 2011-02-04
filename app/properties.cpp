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

#include "properties.hpp"
#include "shader/shader.hpp"
#include "shader/program.hpp"
#include "renderpass.hpp"
#include "texture.hpp"
#include "material.hpp"

UEditor::UEditor(QTreeWidgetItem *p, MaterialPtr mat_, UniformVar& var)
 : QTreeWidgetItem(p),
   mat(mat_),
   name(var.name()) {
  setFlags(flags() & ~Qt::ItemIsSelectable);
}

FloatEditor::FloatEditor(QTreeWidgetItem* p, MaterialPtr mat, UniformVar& var)
  : UEditor(p, mat, var),
    edit(new QLineEdit),
    slider(new QSlider(Qt::Horizontal)),
    min(std::numeric_limits<float>::max()), max(std::numeric_limits<float>::min()),
    m_reset_action(new QAction("Reset", slider)) {
  slider->setMinimum(0);
  slider->setMaximum(1000);
  slider->setContextMenuPolicy(Qt::ActionsContextMenu);
  slider->addAction(m_reset_action);

  setText(0, var.name());
  treeWidget()->setItemWidget(this, 1, edit);
  treeWidget()->setItemWidget(this, 2, slider);

  connect(edit, SIGNAL(editingFinished()), this, SLOT(editingFinished()));
  connect(slider, SIGNAL(valueChanged(int)), this, SLOT(valueChanged(int)));
  connect(m_reset_action, SIGNAL(triggered()), this, SLOT(reset()));
}

FloatEditor::~FloatEditor() {
}

void FloatEditor::updateUI(UniformVar& var) {
  float value = var.get();

  QString txt = QString::number(value);
  if (edit->text() != txt)
    edit->setText(txt);

  if (slider->isSliderDown()) return;

  if (var.min() < min) min = var.min();
  if (var.max() > max) max = var.max();
  if (value < min) min = value;
  if (value > max) max = value;

  float tmp = max - min;
  value -= min;
  if (tmp > 0.000000001f) {
    int ivalue = value/tmp*1000;
    if (slider->value() != ivalue)
      slider->setValue(ivalue);
   }
}

void FloatEditor::editingFinished() {
  UniformVar* var = getVar();
  if (var)
    var->set(edit->text().toFloat());
}

void FloatEditor::valueChanged(int v) {
  float tmp = max - min;
  if (tmp > 0.000000001f) {
    tmp = min + tmp * v / 1000.0f;
  }

  UniformVar* var = getVar();
  if (var)
    var->set(tmp);
}

void FloatEditor::reset() {
  UniformVar* var = getVar();
  if (var) {
    min = var->min();
    max = var->max();
    var->set(var->getDefault());
  }
}

UniformVar* FloatEditor::getVar() {
  UniformVar::List& list = mat->uniformList();
  for (int i = 0; i < list.size(); ++i) {
    if (list[i].name() == name)
      return &list[i];
  }
  return 0;
}


MaterialProperties* MaterialProperties::s_instance = 0;
FileList* FileList::s_instance = 0;

MaterialProperties &MaterialProperties::instance() {
  if (s_instance) return *s_instance;
  return *(new MaterialProperties);
}

FileList &FileList::instance() {
  if (s_instance) return *s_instance;
  return *(new FileList);
}

MaterialProperties::Sub::~Sub() {
}

Properties::Properties(QWidget* parent)
  : QTreeWidget(parent) {
//  setResizeMode(Interactive);
}

MaterialProperties::MaterialProperties(QWidget* parent)
  : Properties(parent) {
  if (!s_instance) s_instance = this;
  setColumnCount(3);
  setColumnWidth(0, 90);
  setColumnWidth(1, 45);
  setHeaderLabels(QStringList() << "Uniform" << "Value" << "Editor");
  setAllColumnsShowFocus(true);
  setAnimated(true);
  setIndentation(10);
  setRootIsDecorated(false);
  setItemsExpandable(false);
  setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
  setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
  setDragDropMode(DragOnly);
  //connect(m_manager, SIGNAL(valueChanged(QtProperty*, const QVariant&)),
  //        this, SLOT(valueChanged(QtProperty*, const QVariant&)));
}

MaterialProperties::~MaterialProperties() {
  if (s_instance == this) s_instance = 0;
}

void MaterialProperties::update(MaterialPtr mat) {
  UniformVar::List list = mat->uniformList();

  Sub& sub = m_materials[mat];
  if (!sub.item) {
    sub.item = new QTreeWidgetItem(this);
    sub.item->setFlags(sub.item->flags() | Qt::ItemIsDragEnabled);
    /// @todo change icon when the type changes
    // sub.item->setIcon(0, mat->icon());
    expandItem(sub.item);
    sub.item->setFirstColumnSpanned(true);
    /// @todo The last thing we want to use is hard coded color values when
    ///       everything else is controlled by qt style. fix this.
    sub.item->setBackgroundColor(0, QColor(240, 240, 240));
    QFont font = sub.item->font(0);
    font.setBold(true);
    sub.item->setFont(0, font);
  }

  if (!mat->prog()) {
    sub.item->setText(0, mat->name() + " (fixed pipeline)");
  } else {
    sub.item->setText(0, mat->name());
  }

  QSet<QString> names;
  foreach (UniformVar var, list) {
    const ShaderTypeInfo& type = var.typeinfo();
    names << var.name();

    UEditor* editor = sub.editors[var.name()];
    if (!editor) {
      editor = createEditor(mat, var, type, sub.item);
      sub.editors[var.name()] = editor;
    }

    if (editor)
      editor->updateUI(var);
  }

  foreach (QString name, sub.editors.keys().toSet() - names)
    sub.editors.remove(name);
}

void MaterialProperties::setMaterials(QSet<MaterialPtr> materials) {
  QSet<MaterialPtr> current = m_materials.keys().toSet();
  foreach (MaterialPtr m, current - materials) {
    delete m_materials[m].item;
    m_materials.remove(m);
  }
  foreach (MaterialPtr m, materials - current)
    update(m);
}

UEditor* MaterialProperties::createEditor(MaterialPtr mat, UniformVar& var,
                                        const ShaderTypeInfo& type, QTreeWidgetItem* p) {
  if (var.arraySize() == 1) {
    if (type.type == GL_FLOAT) {
      return new FloatEditor(p, mat, var);
    } else {
      /// @todo implement
    }
  } else {
    /// @todo implement
  }
  return 0;
}

void MaterialProperties::startDrag(Qt::DropActions supportedActions) {
  MaterialPtr material;
  foreach (QTreeWidgetItem* item, selectedItems()) {
    for (QMap<MaterialPtr, Sub>::iterator it = m_materials.begin(); it != m_materials.end(); ++it) {
      if (it->item == item) {
        material = it.key();
        break;
      }
    }
  }
  if (!material) return;

  QMimeData* data = new QMimeData;
  data->setData("text/x-glsl-lab-material", material->name().toUtf8());

  QDrag* drag = new QDrag(this);
  drag->setMimeData(data);
  drag->setPixmap(QPixmap(":/icons/shader.png"));
  drag->setHotSpot(QPoint(8, 8));
  drag->exec(supportedActions);
 // drag->setDragCursor(QCursor(Qt::BlankCursor).pixmap(), Qt::CopyAction);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FileList::FileList(QWidget* parent)
  : QTreeWidget(parent),
    m_src(new QTreeWidgetItem(this)) {
  if (!s_instance) s_instance = this;

  setIndentation(10);
  setRootIsDecorated(false);
  setItemsExpandable(false);

  /// @todo fix this
  m_src->setBackgroundColor(0, QColor(240, 240, 240));
  QFont font = m_src->font(0);
  font.setBold(true);
  m_src->setFont(0, font);
  m_src->setText(0, "Sources");
  expandItem(m_src);

  connect(this, SIGNAL(itemActivated(QTreeWidgetItem*, int)),
          this, SLOT(activateFile(QTreeWidgetItem*, int)));
}

FileList::~FileList() {
  if (s_instance == this) s_instance = 0;
}

void FileList::update(ShaderPtr shader) {
  QTreeWidgetItem* item = m_files[shader->filename()];

  if (!item) {
    item = new QTreeWidgetItem(m_src);
    m_files[shader->filename()] = item;
    m_items[item] = shader;
    QDir file(shader->filename());
    item->setText(0, file.dirName());
    item->setIcon(0, shader->icon());
//    QTreeWidget::update();
  }
}

void FileList::activateFile(QTreeWidgetItem* item, int /*column*/) {
  ShaderPtr shader = m_items[item];
  if(shader) {
    emit openFile(shader);
  }
}

void FileList::remove(ShaderPtr shader) {
  QTreeWidgetItem* item = m_files[shader->filename()];
  if (item) {
    m_items.remove(item);
    m_files.remove(shader->filename());
    m_src->removeChild(item);
    delete item;
  }
}
