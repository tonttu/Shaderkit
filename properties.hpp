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

#ifndef PROPERTIES_HPP
#define PROPERTIES_HPP

#include "forward.hpp"

#include "qttreepropertybrowser.h"

#include <QMap>

class QtVariantEditorFactory;
class QtVariantPropertyManager;
class QtVariantProperty;

/**
 * Real-time property editor for different properties, including shader
 * uniform variables.
 *
 * This is a singleton class.
 */
class Properties : public QtTreePropertyBrowser {
  Q_OBJECT

public:
  static Properties& instance();
  Properties(QWidget* parent = 0);
  virtual ~Properties();

public slots:
  /// This shader program has changed (usually just relinked)
  void update(ProgramPtr shader);
  /// User changed property value (to variant)
  void valueChanged(QtProperty* property, const QVariant& variant);

protected:
  /// Every shader has one group property whose children are the actual uniform variables
  QMap<ProgramPtr, QtVariantProperty*> m_shaders;

  /// When a property is being edited, this factory is used to create editor widgets.
  /// Custom editors should be implemented by subclassing QtVariantEditorFactory.
  QtVariantEditorFactory* m_factory;

  QtVariantPropertyManager* m_manager;

  static Properties* s_instance;

  typedef QMap<QtProperty*, UniformVar> PropertyMap;

  /// Maps the property to uniform variable
  PropertyMap m_properties;
};

#endif // PROPERTIES_HPP
