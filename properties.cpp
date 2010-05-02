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

#include "qtpropertymanager.h"
#include "qtvariantproperty.h"

Properties* Properties::s_instance = 0;

Properties::Properties(QWidget *parent)
  : QtTreePropertyBrowser(parent),
    m_factory(new QtVariantEditorFactory()),
    m_manager(new QtVariantPropertyManager()) {
  if (!s_instance) s_instance = this;

  connect(m_manager, SIGNAL(valueChanged(QtProperty*, const QVariant&)),
          this, SLOT(valueChanged(QtProperty*, const QVariant&)));

  setFactoryForManager(m_manager, m_factory);
  setPropertiesWithoutValueMarked(true);
  setResizeMode(Interactive);
}

Properties::~Properties() {
  if (s_instance == this) s_instance = 0;
}

void Properties::update(ProgramPtr shader) {
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

void Properties::valueChanged(QtProperty* property, const QVariant& variant) {
  PropertyMap::iterator it = m_properties.find(property);
  if (it == m_properties.end()) return;
qDebug() << variant.toFloat();
  it->set(variant);
}

Properties &Properties::instance() {
  if (s_instance) return *s_instance;
  return *(new Properties);
}
