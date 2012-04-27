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

#ifndef MODEL_HPP
#define MODEL_HPP

#include "forward.hpp"
#include "gl/buffer_object.hpp"
#include "gl/scene_object.hpp"

#include <QString>
#include <QVector>
#include <QVariantMap>

#include "Eigen/Geometry"

namespace Shaderkit {

struct Node {
  Node();
  QString name;
  Eigen::Affine3f transform;
  QList<NodePtr> children;
  QList<MeshPtr> meshes;

  Eigen::AlignedBox<float, 3> bbox();
  void calcBbox(Eigen::AlignedBox<float, 3>& bbox,
                const Eigen::Affine3f& transform = Eigen::Affine3f::Identity()) const;

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

namespace ObjectRenderer {
  /// Renders a rectangular box.
  void drawBox(float x, float y, float z);
  void drawBox(float x, float y, float z, State& state);
  void drawSphere(float radius, int segments, int rings, State& state);
}

class Model : public SceneObject {
public:
  Model(QString name);

  NodePtr node() { return m_node; }

  void render(ObjectPtr o, State& state, const Node& node);
  void render(ObjectPtr o, State& state) { render(o, state, *m_node); }

  bool builtin() const { return m_builtin; }

  QVariantMap toMap() const;
  void load(QVariantMap map);
  ModelPtr clone();

  const Eigen::AlignedBox<float, 3>& bbox();

  static ModelPtr createBuiltin(const QString& name, const QString& model,
                                const QVariantMap& map = QVariantMap());

private:

  // bounding box in local coordinates
  Eigen::AlignedBox<float, 3> m_bbox;
  bool m_dirty;

  QVariantMap m_map;

  NodePtr m_node;
  bool m_builtin;
};

class Mesh {
public:
  virtual ~Mesh() {}
  virtual void render(State& state);

  virtual void calcBbox(const Eigen::Affine3f& transform, Eigen::AlignedBox<float, 3>& bbox) const = 0;

  /// @todo accessors and make protected
  QString name;
protected:
  virtual void renderObj(State& state) = 0;

};

class BuiltIn : public Mesh {
public:
  virtual ~BuiltIn() {}
};

/// Built-in Teapot object, that is a tuned version of GLUT teapot.
class Teapot : public BuiltIn {
public:
  virtual ~Teapot() {}
  virtual void calcBbox(const Eigen::Affine3f& transform, Eigen::AlignedBox<float, 3>& bbox) const;

protected:
  void renderObj(State& state);
};

/// Built-in dummy Box object
class Box : public BuiltIn {
public:
  Box(const Eigen::Vector3f size) : m_size(size) {}
  virtual ~Box() {}
  virtual void calcBbox(const Eigen::Affine3f& transform, Eigen::AlignedBox<float, 3>& bbox) const;

protected:
  void renderObj(State& state);

  const Eigen::Vector3f m_size;
};

class Sphere : public BuiltIn {
public:
  Sphere(float size) : m_size(size) {}
  virtual ~Sphere() {}
  virtual void calcBbox(const Eigen::Affine3f& transform,
                        Eigen::AlignedBox<float, 3>& bbox) const;

protected:
  void renderObj(State& state);

  const float m_size;
};

class TriMesh : public Mesh {
public:
  TriMesh();
  virtual ~TriMesh() {}
  virtual void calcBbox(const Eigen::Affine3f& transform,
                        Eigen::AlignedBox<float, 3>& bbox) const;

  std::vector<float> vertices;
  std::vector<float> normals;
  std::vector<float> tangents;
  std::vector<float> bitangents;
  std::vector<std::vector<float>> colors;
  std::vector<std::vector<float>> uvs;
  std::vector<int> uv_components;
  std::vector<unsigned int> indices;

private:
  std::map<VertexAttrib::Target, BufferObject2> m_buffers;
  BufferObject2 m_indices;
  void renderObj(State& state);
};

} // namespace Shaderkit

#endif // MODEL_HPP
