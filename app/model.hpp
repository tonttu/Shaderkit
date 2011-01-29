#ifndef MODEL_HPP
#define MODEL_HPP

#include "forward.hpp"
#include <QString>
#include <QVector>

struct Node {
  QString name;
  float matrix[16];
  QVector<Node> children;
  QVector<MeshPtr> meshes;
};

class Model {
public:
  Model();

  QString name() const { return m_node.name; }
  Node& node() { return m_node; }

private:
  Node m_node;
};

class Mesh {
public:
  /// Puts the vertex data etc to the GPU
  void render(State& state);

  std::vector<float> vertices;
  std::vector<float> normals;
  std::vector<float> tangents;
  std::vector<float> bitangents;
  std::vector<std::vector<float> > colors;
  std::vector<std::vector<float> > uvs;
  std::vector<int> uv_sizes;
  std::vector<unsigned int> indices;

  QString name;
  MaterialPtr material;
};

#endif // MODEL_HPP
