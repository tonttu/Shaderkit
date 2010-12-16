/**
 * Copyright 2010 Riku Palomäki.
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

UEditor::UEditor(RenderPassPtr pass_, UniformVar& var)
 : pass(pass_),
   name(var.name()) {}

FloatEditor::FloatEditor(RenderPassPtr pass, UniformVar& var)
  : UEditor(pass, var),
    edit(new QLineEdit),
    slider(new QSlider(Qt::Horizontal)),
    min(0), max(0) {
  slider->setMinimum(0);
  slider->setMaximum(1000);

  connect(edit, SIGNAL(editingFinished()), this, SLOT(editingFinished()));
  connect(slider, SIGNAL(valueChanged(int)), this, SLOT(valueChanged(int)));
}

void FloatEditor::updateUI(UniformVar& var) {
  float value = var.get();

  QString txt = QString::number(value);
  if (edit->text() != txt)
    edit->setText(txt);

  if (slider->isSliderDown()) return;

  min = var.min();
  max = var.max();
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
  UniformVar::List list = pass->uniformList();
  for (int i = 0; i < list.size(); ++i) {
    UniformVar& var = list[i];
    if (var.name() == name) {
      var.set(edit->text().toFloat());
      pass->setUniformList(list);
      break;
    }
  }
}

void FloatEditor::valueChanged(int v) {
  float tmp = max - min;
  if (tmp > 0.000000001f) {
    tmp = min + tmp * v / 1000.0f;
  }

  UniformVar::List list = pass->uniformList();
  for (int i = 0; i < list.size(); ++i) {
    UniformVar& var = list[i];
    if (var.name() == name) {
      var.set(tmp);
      pass->setUniformList(list);
      break;
    }
  }
}


ShaderProperties* ShaderProperties::s_instance = 0;
FileList* FileList::s_instance = 0;

ShaderProperties &ShaderProperties::instance() {
  if (s_instance) return *s_instance;
  return *(new ShaderProperties);
}

FileList &FileList::instance() {
  if (s_instance) return *s_instance;
  return *(new FileList);
}

Properties::Properties(QWidget* parent)
  : QTreeWidget(parent) {
//  setResizeMode(Interactive);
}

ShaderProperties::ShaderProperties(QWidget* parent)
  : Properties(parent) {
  if (!s_instance) s_instance = this;
  setColumnCount(3);
  setColumnWidth(0, 90);
  setColumnWidth(1, 45);
  setSelectionMode(Properties::NoSelection);
  setHeaderLabels(QStringList() << "Uniform" << "Value" << "Editor");
  setAllColumnsShowFocus(true);
  setAnimated(true);
  setIndentation(10);
  setRootIsDecorated(false);
  setItemsExpandable(false);
  setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
  setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
  //connect(m_manager, SIGNAL(valueChanged(QtProperty*, const QVariant&)),
  //        this, SLOT(valueChanged(QtProperty*, const QVariant&)));
}

ShaderProperties::~ShaderProperties() {
  if (s_instance == this) s_instance = 0;
}

void ShaderProperties::update(RenderPassPtr pass) {
  UniformVar::List list = pass->uniformList();

  Sub& sub = m_passes[pass];
  if (!sub.item) {
    sub.item = new QTreeWidgetItem(this);
    expandItem(sub.item);
    sub.item->setFirstColumnSpanned(true);
    /// @todo The last thing we want to use is hard coded color values when
    ///       everything else is controlled by qt style. fix this.
    sub.item->setBackgroundColor(0, QColor(240, 240, 240));
    QFont font = sub.item->font(0);
    font.setBold(true);
    sub.item->setFont(0, font);
    sub.item->setText(0, pass->name());
  }

  foreach (UniformVar var, list) {
    const ShaderTypeInfo& type = var.typeinfo();

    UEditor* editor = sub.editors[var.name()];
    if (!editor)
      sub.editors[var.name()] = editor = createEditor(pass, var, type, sub.item);

    if (editor)
      editor->updateUI(var);
  }

  /// @todo find removed uniforms

    /*
  QtVariantProperty* obj = m_shaders[shader];

  // Ensure the existence of the shader group property
  if (!obj) {
    m_shaders[shader] = obj = m_manager->addProperty(QtVariantPropertyManager::groupTypeId(), shader->name());
    addProperty(obj);
  }

  // Remove all old subproperties
  foreach (QtProperty* sub, obj->subProperties()) {
    obj->removeSubProperty(sub);
    m_properties.remove(sub);
  }

  UniformVar::List list = shader->getUniformList();
  for (UniformVar::List::iterator it = list.begin(); it != list.end(); ++it) {
    const ShaderTypeInfo& type = it->typeinfo();

    if (it->arraySize() == 1) {
      QString name = it->name();
      QtVariantProperty* item = m_manager->addProperty(type.variant(), name);
      if (item) {
        m_properties[item] = *it;
        item->setValue(it->get());
        obj->addSubProperty(item);
      }
    } else {
      /// @todo implement
    }
  }*/
}

void ShaderProperties::remove(RenderPassPtr pass) {
  /*
  QtVariantProperty* obj = m_shaders[shader];
  if (obj) {
    m_shaders.remove(shader);

    // Remove all old subproperties
    foreach (QtProperty* sub, obj->subProperties()) {
      obj->removeSubProperty(sub);
      m_properties.remove(sub);
    }

    removeProperty(obj);
    delete obj;
  }*/
}

/*void ShaderProperties::valueChanged(QtProperty* property, const QVariant& variant) {
  PropertyMap::iterator it = m_properties.find(property);
  if (it == m_properties.end()) return;
  it->set(variant);
}*/

UEditor* ShaderProperties::createEditor(RenderPassPtr pass, UniformVar& var,
                                        const ShaderTypeInfo& type, QTreeWidgetItem* p) {
  if (var.arraySize() == 1) {
    if (type.type == GL_FLOAT) {
      FloatEditor* editor = new FloatEditor(pass, var);
      QTreeWidgetItem* item = new QTreeWidgetItem(p);
      item->setText(0, var.name());
      setItemWidget(item, 1, editor->edit);
      setItemWidget(item, 2, editor->slider);
      return editor;
    } else {
      /// @todo implement
    }
  } else {
    /// @todo implement
  }
  return 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FileList::FileList(QWidget* parent)
  : QTreeWidget(parent),
    m_src(new QTreeWidgetItem(this)) {
  if (!s_instance) s_instance = this;

  m_src->setText(0, "Sources");
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
    item->setText(0, shader->filename());

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
