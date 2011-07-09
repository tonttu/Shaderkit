/**
 * Copyright 2010,2011 Riku Palomäki.
 * This file is part of Shaderkit, http://www.shaderkit.org/.
 *
 * Shaderkit is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License Version 3 as
 * published by the Free Software Foundation.
 *
 * Shaderkit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Shaderkit.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "object3d.hpp"
#include "state.hpp"
#include "model.hpp"
#include "material.hpp"

Object3D::Object3D(QString name, ModelPtr model)
  : SceneObject(name),
    m_transform(Eigen::Affine3f::Identity()),
    m_model(model) {}
Object3D::~Object3D() {}

void Object3D::render(State& state) {
  if (m_model) {
    state.pushTransform(m_transform);
    m_model->render(shared_from_this(), state);
    state.popTransform();
  }
}

bool Object3D::builtin() const {
  if (!m_model) return false;
  return m_model->builtin();
}

MaterialPtr Object3D::materialForMesh(QString meshname) {
  if (m_materials.contains(meshname)) return m_materials[meshname];
  return m_default_material;
}

void Object3D::setMaterialForMesh(QString meshname, MaterialPtr mat) {
  m_materials[meshname] = mat;
}

void Object3D::setDefaultMaterial(MaterialPtr mat) {
  m_default_material = mat;
}

void Object3D::remove(MaterialPtr mat) {
  if (mat == m_default_material) m_default_material = MaterialPtr();

  for (auto it = m_materials.begin(); it != m_materials.end();) {
    if (*it == mat) it = m_materials.erase(it);
    else ++it;
  }
}

QVariantMap Object3D::save() const {
  QVariantMap map = SceneObject::save();

  if (m_default_material) map["material"] = m_default_material->name();
  if (m_model) map["model"] = m_model->name();
  QVariantMap materials;
  for (auto it = m_materials.begin(); it != m_materials.end(); ++it) {
    if (*it) materials[it.key()] = (*it)->name();
  }
  if (!materials.isEmpty()) map["materials"] = materials;

  return map;
}

void Object3D::load(QVariantMap map) {
  SceneObject::load(map);
}

ObjectPtr Object3D::clone() {
  return ObjectPtr(new Object3D(m_name));
}

void Object3D::setModel(ModelPtr model) {
  m_model = model;
}
