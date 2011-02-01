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

#include "object3d.hpp"
#include "state.hpp"
#include "model.hpp"

Object3D::Object3D(QString name, ModelPtr model)
  : m_name(name),
    m_model(model) {}
Object3D::~Object3D() {}

void Object3D::render(State& state) {
  if (m_model) {
    m_model->render(shared_from_this(), state);
  }
}

bool Object3D::builtin() const {
  if (!m_model) return false;
  return m_model->builtin();
}

QVariantMap Object3D::save() const {
  QVariantMap map;
  if (m_model) {
    map["model"] = m_model->name();
  }

  return map;
}

MaterialPtr Object3D::material(QString name) {
  if (m_materials.contains(name)) return m_materials[name];
  return m_default_material;
}

void Object3D::setMaterial(QString name, MaterialPtr mat) {
  m_materials[name] = mat;
}

void Object3D::setDefaultMaterial(MaterialPtr mat) {
  m_default_material = mat;
}

void Object3D::load(QVariantMap /*map*/) {
}

ObjectPtr Object3D::clone() {
  return ObjectPtr(new Object3D(m_name));
}

void Object3D::setModel(ModelPtr model) {
  m_model = model;
}
