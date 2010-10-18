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
  //connect(m_manager, SIGNAL(valueChanged(QtProperty*, const QVariant&)),
  //        this, SLOT(valueChanged(QtProperty*, const QVariant&)));
}

ShaderProperties::~ShaderProperties() {
  if (s_instance == this) s_instance = 0;
}

void ShaderProperties::update(ProgramPtr shader) {
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

void ShaderProperties::remove(ProgramPtr shader) {
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
