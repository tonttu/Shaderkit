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

#define SINGLETON()

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
  QtVariantProperty* objProperty = m_shaders[shader];

  // Ensure the existence of the shader group property
  if (!objProperty) {
    m_shaders[shader] = objProperty = m_manager->addProperty(QtVariantPropertyManager::groupTypeId(), shader->name());
    addProperty(objProperty);
  }

  // Remove all old subproperties
  QList<QtProperty*> sub = objProperty->subProperties();
  for (QList<QtProperty*>::iterator it = sub.begin(); it != sub.end(); ++it) {
    objProperty->removeSubProperty(*it);
    m_properties.remove(*it);
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
        objProperty->addSubProperty(item);
      }
    } else {
      /// @todo implement
    }
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
  QtVariantProperty* objProperty = m_renderpasses[pass];

  // Ensure the existence of the render pass group property
  if (!objProperty) {
    m_renderpasses[pass] = objProperty = m_manager->addProperty(QtVariantPropertyManager::groupTypeId(), pass->name());
    addProperty(objProperty);
  }

  // Remove all old subproperties
  QList<QtProperty*> sub = objProperty->subProperties();
  for (QList<QtProperty*>::iterator it = sub.begin(); it != sub.end(); ++it) {
    objProperty->removeSubProperty(*it);
    m_properties.remove(*it);
  }

  QtVariantProperty* item = m_manager->addProperty(QVariant::Int, "width");
  item->setValue(pass->width());
  objProperty->addSubProperty(item);

  item = m_manager->addProperty(QVariant::Int, "height");
  item->setValue(pass->height());
  objProperty->addSubProperty(item);

  QtVariantProperty* in = m_manager->addProperty(QtVariantPropertyManager::groupTypeId(), "in");
  objProperty->addSubProperty(in);
  foreach (QString name, pass->in()) {
    item = m_manager->addProperty(QVariant::String, name);
    item->setValue(pass->in(name)->name());
    in->addSubProperty(item);
  }

  QtVariantProperty* out = m_manager->addProperty(QtVariantPropertyManager::groupTypeId(), "out");
  objProperty->addSubProperty(out);
  foreach (QString name, pass->out()) {
    item = m_manager->addProperty(QVariant::String, name);
    item->setValue(pass->out(name)->name());
    in->addSubProperty(item);
  }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FileList::FileList(QWidget* parent)
  : QTableWidget(parent) {
  if (!s_instance) s_instance = this;

  connect(this, SIGNAL(itemDoubleClicked(QTableWidgetItem*)),
          this, SLOT(activateFile(QTableWidgetItem*)));
}

FileList::~FileList() {
  if (s_instance == this) s_instance = 0;
}

void FileList::update(const QString& filename) {
  QTableWidgetItem* item = m_files[filename];

  if (!item) {
    m_files[filename] = item = new QTableWidgetItem(filename);
    setRowCount(rowCount()+1);
    setItem(rowCount()-1, 0, item);

    resizeColumnsToContents();
    QTableWidget::update();
  }
}

void FileList::activateFile(QTableWidgetItem* item) {
  QString filename = m_files.key(item);
  if(!filename.isEmpty()) {
    emit openFile(filename);
  }
}
