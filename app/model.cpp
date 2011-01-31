#include "model.hpp"
#include "opengl.hpp"
#include "object3d.hpp"
#include "state.hpp"

#include "ext/glut_teapot.hpp"

namespace {
  /// Renders a rectangular box.
  void drawBox(float x, float y, float z) {
    glBegin(GL_QUADS);
    glNormal3f(1,0,0);
    glVertex3f(x,-y,z); glVertex3f(x,-y,-z); glVertex3f(x,y,-z); glVertex3f(x,y,z);
    glNormal3f(0,0,-1);
    glVertex3f(x,-y,-z); glVertex3f(-x,-y,-z); glVertex3f(-x,y,-z); glVertex3f(x,y,-z);
    glNormal3f(-1,0,0);
    glVertex3f(-x,-y,-z); glVertex3f(-x,-y,z); glVertex3f(-x,y,z); glVertex3f(-x,y,-z);
    glNormal3f(0,0,1);
    glVertex3f(-x,-y,z); glVertex3f(x,-y,z); glVertex3f(x,y,z); glVertex3f(-x,y,z);
    glNormal3f(0,1,0);
    glVertex3f(-x,y,z); glVertex3f(x,y,z); glVertex3f(x,y,-z); glVertex3f(-x,y,-z);
    glNormal3f(0,-1,0);
    glVertex3f(-x,-y,z); glVertex3f(-x,-y,-z); glVertex3f(x,-y,-z); glVertex3f(x,-y,z);
    glEnd();
  }
}

Node::Node() : matrix() {
  matrix[0] = matrix[5] = matrix[10] = matrix[15] = 1.0f;
}

Model::Model(QString name) : m_builtin(false) {
  m_node.name = name;
}

void Model::render(ObjectPtr o, State& state, const Node& node) {
  /// @todo forget pushmatrix etc, use state
  glPushMatrix();
  glMultMatrixf(node.matrix);

  for (int i = 0; i < node.meshes.size(); ++i) {
    Mesh& m = *node.meshes[i];
    MaterialPtr material = o->materials()[m.name];
    if (material) state.pushMaterial(material);
    node.meshes[i]->render(state);
    if (material) state.popMaterial();
  }

  for (int i = 0; i < node.children.size(); ++i)
    render(o, state, node.children[i]);

  glPopMatrix();
}

void Model::load(QVariantMap /*map*/) {
}

ModelPtr Model::clone() {
  return ModelPtr(new Model(*this));
}

ModelPtr Model::createBuiltin(const QString& name) {
  if (name == "teapot") {
    ModelPtr m(new Model);
    m->m_builtin = true;
    m->node().name = "teapot";
    m->node().meshes << MeshPtr(new Teapot);
    return m;
  } else if (name == "box") {
    ModelPtr m(new Model);
    m->m_builtin = true;
    m->node().name = "box";
    m->node().meshes << MeshPtr(new Box);
    return m;
  }
  Log::error("Unknown builtin '%s'", name.toUtf8().data());
  return ModelPtr();
}

void Mesh::render(State& state) {
  glPushMatrix();
  renderObj(state);
  glPopMatrix();
}

void Teapot::renderObj(State&) {
  teapot(10, 3.7f, GL_FILL);
}

void Box::renderObj(State&) {
  /// @todo remove translatef
  glTranslatef(0, -3.1f, 0);
  drawBox(3.5f, 0.4f, 3.5f);
}

void TriMesh::renderObj(State& state) {
  //if (material) state.push(material);

  /// @todo implement

  //if (material) state.pop(material);
}
