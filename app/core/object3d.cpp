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

namespace Shaderkit
{

  QVariantMap saveTransform(const glm::mat4& m)
  {
    QStringList tmp;
    const float* v = &m[0].x;
    for (int i = 0; i < 16; ++i)
      tmp << QString::number(v[i]);
    QVariantMap ret;
    ret["matrix"] = tmp.join(" ");
    return ret;
  }

  glm::mat4 loadTransform(const QVariant& var)
  {
    QVariantMap map = var.toMap();
    if (map.contains("matrix")) {
      QStringList tmp;
      tmp = map["matrix"].toString().split(QRegExp("\\s+"));
      if (tmp.size() == 16) {
        glm::mat4 m(glm::mat4::_null);
        float* v = &m[0].x;
        for (int i = 0; i < 16; ++i)
          v[i] = tmp[i].toFloat();
        return m;
      }
    }
    return glm::mat4();
  }

  Object3D::Object3D(const QString& name, const ModelPtr& model)
    : SceneObject(name),
      m_transform(*this, glm::mat4()),
      m_model(model)
  {
  }

  Object3D::Object3D(const Object3D& o)
    : QObject(),
      std::enable_shared_from_this<Object3D>(),
      SceneObject(o),
      m_transform(*this, o.m_transform),
      // shallow copy
      m_model(o.m_model),
      m_materials(o.m_materials),
      m_default_material(o.m_default_material)
  {}

  Object3D::~Object3D() {}

  void Object3D::render(State& state)
  {
    if (m_model) {
      state.pushModel(m_transform);
      m_model->render(shared_from_this(), state);
      state.popModel();
    }
  }

  bool Object3D::builtin() const
  {
    if (!m_model) return false;
    return m_model->builtin();
  }

  MaterialPtr Object3D::materialForMesh(const QString& meshname) const
  {
    if (m_materials.contains(meshname))
      return m_materials.value(meshname);
    return m_default_material;
  }

  void Object3D::setMaterialForMesh(const QString& meshname, const MaterialPtr& mat)
  {
    if (m_materials[meshname] == mat) return;
    m_materials[meshname] = mat;
    emit changed(shared_from_this());
  }

  void Object3D::setDefaultMaterial(const MaterialPtr& mat)
  {
    if (m_default_material == mat) return;
    m_default_material = mat;
    emit changed(shared_from_this());
  }

  void Object3D::remove(const MaterialPtr& mat)
  {
    bool diff = false;
    if (mat == m_default_material) {
      diff = true;
      m_default_material = MaterialPtr();
    }

    for (auto it = m_materials.begin(); it != m_materials.end();) {
      if (*it == mat) {
        it = m_materials.erase(it);
        diff = true;
      } else ++it;
    }
    if (diff) emit changed(shared_from_this());
  }

  QVariantMap Object3D::toMap() const
  {
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

  void Object3D::load(QVariantMap map)
  {
    SceneObject::load(map);
    m_transform = loadTransform(map["transform"]);
  }

  ObjectPtr Object3D::clone()
  {
    return ObjectPtr(new Object3D(*this));
  }

  void Object3D::setModel(const ModelPtr& model)
  {
    if(m_model == model) return;
    m_model = model;
    emit changed(shared_from_this());
  }

  void Object3D::setTransform(const glm::mat4& transform)
  {
    m_transform = transform;
  }

  void Object3D::attributeChanged()
  {
    emit changed(shared_from_this());
  }

} // namespace Shaderkit
