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

#ifndef OBJECT3D_HPP
#define OBJECT3D_HPP

#include "forward.hpp"
#include "gl/scene_object.hpp"

#include <core/attribute.hpp>

#include "Eigen/Geometry"

#include <QVariantMap>

namespace Shaderkit
{

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
  class Object3D : public QObject, public std::enable_shared_from_this<Object3D>,
    public SceneObject
  {
    Q_OBJECT

  public:
    Object3D(const QString& name, const ModelPtr& model = ModelPtr());
    virtual ~Object3D();

    /// Renders the object with given state
    virtual void render(State& state);

    virtual bool builtin() const;

    MaterialPtr materialForMesh(const QString& meshname) const;
    void setMaterialForMesh(const QString& meshname, const MaterialPtr& mat);
    void setDefaultMaterial(const MaterialPtr& mat);
    void remove(const MaterialPtr& mat);

    virtual QVariantMap toMap() const;
    virtual void load(QVariantMap map);
    /// @todo maybe different version that also makes a deep copy of model and materials?
    ObjectPtr clone();

    void setModel(const ModelPtr& model);
    ModelPtr model() { return m_model; }

    void setTransform(const Eigen::Affine3f& transform);
    const Eigen::Affine3f& transform() const { return m_transform; }

    virtual void attributeChanged();

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

  signals:
    void changed(ObjectPtr);

  protected:
    explicit Object3D(const Object3D& o);

  private:
    Attribute<Eigen::Affine3f> m_transform;
    ModelPtr m_model;
    QMap<QString, MaterialPtr> m_materials;
    MaterialPtr m_default_material;
  };

} // namespace Shaderkit

#endif // OBJECT3D_HPP
