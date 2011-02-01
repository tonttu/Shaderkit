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

#ifndef OBJECT3D_HPP
#define OBJECT3D_HPP

#include "forward.hpp"

#include <QObject>
#include <QVariantMap>

/**
 * 3D Model class.
 *
 * Every renderable object should inherit from this, including all built-in
 * objects and models loaded from files.
 *
 * An object might have texture coordinates, but actual textures, shaders,
 * vertex colors etc. come from other classes, this is strictly 3D trimesh
 * or similar renderable object. Shaders etc should be ready before calling
 * the render() method.
 */
class Object3D : public QObject, public std::enable_shared_from_this<Object3D> {
  Q_OBJECT

public:
  Object3D(QString name, ModelPtr model = ModelPtr());
  virtual ~Object3D();

  /// Returns the object name
  /// @todo make a separate UI name method
  QString name() const { return m_name; }
  /// Renders the object with given state
  virtual void render(State& state);

  virtual bool builtin() const;

  virtual QVariantMap save() const;

  MaterialPtr material(QString name);
  void setMaterial(QString name, MaterialPtr mat);
  void setDefaultMaterial(MaterialPtr mat);

  void load(QVariantMap map);
  /// Doesn't clone materials or model
  ObjectPtr clone();

  void setModel(ModelPtr model);
  ModelPtr model() { return m_model; }

private:
  QString m_name;
  ModelPtr m_model;
  QMap<QString, MaterialPtr> m_materials;
  MaterialPtr m_default_material;
};

#endif // OBJECT3D_HPP
