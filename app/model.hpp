#ifndef MODEL_HPP
#define MODEL_HPP

#include "forward.hpp"
#include <QString>
#include <QVector>
#include <QVariantMap>

struct Node {
  Node();
  QString name;
  float matrix[16];
  QVector<Node> children;
  QVector<MeshPtr> meshes;
};

class Model {
public:
  Model(QString name = "");

  QString name() const { return m_node.name; }
  Node& node() { return m_node; }

  void render(ObjectPtr o, State& state, const Node& node);
  void render(ObjectPtr o, State& state) { render(o, state, m_node); }

  bool builtin() const { return m_builtin; }

  void load(QVariantMap map);
  ModelPtr clone();

  static ModelPtr createBuiltin(const QString& name);

private:
  Node m_node;
  bool m_builtin;
};

class Mesh {
public:
  virtual ~Mesh() {}
  virtual void render(State& state);

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

protected:
  void renderObj(State& state);
};

/// Built-in dummy Box object
class Box : public BuiltIn {
public:
  virtual ~Box() {}

protected:
  void renderObj(State& state);
};

class TriMesh : public Mesh {
public:
  virtual ~TriMesh() {}

  std::vector<float> vertices;
  std::vector<float> normals;
  std::vector<float> tangents;
  std::vector<float> bitangents;
  std::vector<std::vector<float> > colors;
  std::vector<std::vector<float> > uvs;
  std::vector<int> uv_sizes;
  std::vector<unsigned int> indices;

protected:
  void renderObj(State& state);
};



#endif // MODEL_HPP
