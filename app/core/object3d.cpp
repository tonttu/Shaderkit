/**
 * Copyright 2010-2012 Riku Palomäki.
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

#include "core/object3d.hpp"
#include "core/model.hpp"
#include "core/material.hpp"
#include "gl/state.hpp"

namespace Shaderkit {

QVariantMap saveTransform(const Eigen::Affine3f& affine) {
  const Eigen::Matrix4f& m = affine.matrix();
  QStringList tmp;
  for (int i = 0; i < 16; ++i)
    tmp << QString::number(m.data()[i]);
  QVariantMap ret;
  ret["matrix"] = tmp.join(" ");
  return ret;
}

Eigen::Affine3f loadTransform(const QVariant& var) {
  QVariantMap map = var.toMap();
  if (map.contains("matrix")) {
    QStringList tmp;
    tmp = map["matrix"].toString().split(QRegExp("\\s+"));
    if (tmp.size() == 16) {
      Eigen::Affine3f affine;
      Eigen::Matrix4f& m = affine.matrix();
      for (int i = 0; i < 16; ++i)
        m.data()[i] = tmp[i].toFloat();
      return affine;
    }
  }
  return Eigen::Affine3f::Identity();
}

Object3D::Object3D(QString name, ModelPtr model)
  : SceneObject(name),
    m_transform(Eigen::Affine3f::Identity()),
    m_model(model) {
}
Object3D::~Object3D() {}

void Object3D::render(State& state) {
  if (m_model) {
    state.pushModel(m_transform);
    m_model->render(shared_from_this(), state);
    state.popModel();
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

QVariantMap Object3D::toMap() const {
  QVariantMap map = SceneObject::toMap();

  if (m_default_material) map["material"] = m_default_material->name();
  if (m_model) map["model"] = m_model->name();
  QVariantMap materials;
  for (auto it = m_materials.begin(); it != m_materials.end(); ++it) {
    if (*it) materials[it.key()] = (*it)->name();
  }
  if (!materials.isEmpty()) map["materials"] = materials;
  map["transform"] = saveTransform(m_transform);

  return map;
}

void Object3D::load(QVariantMap map) {
  SceneObject::load(map);
  m_transform = loadTransform(map["transform"]);
}

ObjectPtr Object3D::clone() {
  return ObjectPtr(new Object3D(m_name));
}

void Object3D::setModel(ModelPtr model) {
  m_model = model;
}

} // namespace Shaderkit
