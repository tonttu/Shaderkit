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

Model::Model(QString name) : m_node(new Node), m_builtin(false) {
  m_node->name = name;
}

void Model::render(ObjectPtr o, State& state, const Node& node) {
  /// @todo forget pushmatrix etc, use state
  glPushMatrix();
  glMultMatrixf(node.matrix);

  foreach (MeshPtr m, node.meshes) {
    MaterialPtr material = o->material(m->name);
    if (material) state.pushMaterial(material);
    m->render(state);
    if (material) state.popMaterial();
  }

  for (int i = 0; i < node.children.size(); ++i)
    render(o, state, *node.children[i]);

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
    m->node()->name = "teapot";
    m->node()->meshes << MeshPtr(new Teapot);
    return m;
  } else if (name == "box") {
    ModelPtr m(new Model);
    m->m_builtin = true;
    m->node()->name = "box";
    m->node()->meshes << MeshPtr(new Box);
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
  state.push();

  if (!vertices.empty())
    m_vertices.enableArray(state, GL_VERTEX_ARRAY, 3, vertices);
  if (!normals.empty())
    m_normals.enableArray(state, GL_NORMAL_ARRAY, 3, normals);
  if (!uvs.empty())
    m_uv0.enableArray(state, GL_TEXTURE_COORD_ARRAY, uv_sizes[0], uvs[0]);
  if (!colors.empty())
    m_color0.enableArray(state, GL_COLOR_ARRAY, 3, colors[0]);
  if (!indices.empty()) {
    m_indices.bind(state, GL_ELEMENT_ARRAY_BUFFER, indices);
    glRun(glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0));
  }
  state.pop();
}
