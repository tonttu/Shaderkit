/**
 * Copyright 2010 Riku Palomäki.
 * This file is part of GLSL Lab.
 *
 * GLSL Lab is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
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

#include "qtpropertymanager.h"
#include "qtvariantproperty.h"

ShaderProperties* ShaderProperties::s_instance = 0;
RenderPassProperties* RenderPassProperties::s_instance = 0;
FileList* FileList::s_instance = 0;

ShaderProperties &ShaderProperties::instance() {
  if (s_instance) return *s_instance;
  return *(new ShaderProperties);
}

RenderPassProperties &RenderPassProperties::instance() {
  if (s_instance) return *s_instance;
  return *(new RenderPassProperties);
}

FileList &FileList::instance() {
  if (s_instance) return *s_instance;
  return *(new FileList);
}

Properties::Properties(QWidget* parent)
  : QtTreePropertyBrowser(parent),
    m_factory(new QtVariantEditorFactory()),
    m_manager(new QtVariantPropertyManager()) {

  connect(m_manager, SIGNAL(valueChanged(QtProperty*, const QVariant&)),
          this, SLOT(valueChanged(QtProperty*, const QVariant&)));

  setFactoryForManager(m_manager, m_factory);
  setPropertiesWithoutValueMarked(true);
  setResizeMode(Interactive);
}

void Properties::valueChanged(QtProperty* property, const QVariant& variant) {
  PropertyMap::iterator it = m_properties.find(property);
  if (it == m_properties.end()) return;
  it->set(variant);
}

ShaderProperties::ShaderProperties(QWidget* parent)
  : Properties(parent) {
  if (!s_instance) s_instance = this;
}

ShaderProperties::~ShaderProperties() {
  if (s_instance == this) s_instance = 0;
}

void ShaderProperties::update(ProgramPtr shader) {
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
  }
}

void ShaderProperties::remove(ProgramPtr shader) {
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
  }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

RenderPassProperties::RenderPassProperties(QWidget* parent)
  : Properties(parent) {
  if (!s_instance) s_instance = this;
}

RenderPassProperties::~RenderPassProperties() {
  if (s_instance == this) s_instance = 0;
}

void RenderPassProperties::update(RenderPassPtr pass) {
  QtVariantProperty* obj = m_renderpasses[pass];

  // Ensure the existence of the render pass group property
  if (!obj) {
    m_renderpasses[pass] = obj = m_manager->addProperty(QtVariantPropertyManager::groupTypeId(), pass->name());
    addProperty(obj);
  }

  // Remove all old subproperties
  foreach (QtProperty* sub, obj->subProperties()) {
    obj->removeSubProperty(sub);
    m_properties.remove(sub);
  }

  QtVariantProperty* item = m_manager->addProperty(QVariant::Int, "width");
  item->setValue(pass->width());
  obj->addSubProperty(item);

  item = m_manager->addProperty(QVariant::Int, "height");
  item->setValue(pass->height());
  obj->addSubProperty(item);

  QtVariantProperty* in = m_manager->addProperty(QtVariantPropertyManager::groupTypeId(), "in");
  obj->addSubProperty(in);
  foreach (QString name, pass->in()) {
    item = m_manager->addProperty(QVariant::String, name);
    item->setValue(pass->in(name)->name());
    in->addSubProperty(item);
  }

  QtVariantProperty* out = m_manager->addProperty(QtVariantPropertyManager::groupTypeId(), "out");
  obj->addSubProperty(out);
  foreach (QString name, pass->out()) {
    item = m_manager->addProperty(QVariant::String, name);
    item->setValue(pass->out(name)->name());
    in->addSubProperty(item);
  }
}

void RenderPassProperties::remove(RenderPassPtr pass) {
  QtVariantProperty* obj = m_renderpasses[pass];

  if (obj) {
    m_renderpasses.remove(pass);

    // Remove all old subproperties
    foreach (QtProperty* sub, obj->subProperties()) {
      obj->removeSubProperty(sub);
      m_properties.remove(sub);
      /// @todo should we delete sub, also many other places in this file
    }

    removeProperty(obj);
    delete obj;
  }
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
