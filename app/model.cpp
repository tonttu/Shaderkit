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

#include "model.hpp"
#include "opengl.hpp"
#include "object3d.hpp"
#include "state.hpp"

#include "ext/glut_teapot.hpp"

#include <GL/glut.h>

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

Model::Model(QString name) : SceneObject(name), m_node(new Node), m_builtin(false) {
  m_node->name = name;
}

void Model::render(ObjectPtr o, State& state, const Node& node) {
  /// @todo forget pushmatrix etc, use state
  glPushMatrix();
  glMultMatrixf(node.matrix);

  foreach (MeshPtr m, node.meshes) {
    MaterialPtr material = o->materialForMesh(m->name);
    if (material) state.pushMaterial(material);

    if (state.picking()) {
      /// @todo use state to do the scissor test
      glRun(glScissor(state.pickingPos().x(), state.pickingPos().y(), 1, 1));
      glRun(glEnable(GL_SCISSOR_TEST));
      /// @todo get the query from state, use glGenQueries etc
      glRun(glBeginQuery(GL_SAMPLES_PASSED, state.pickingQuery()));
      m->render(state);
      glRun(glEndQuery(GL_SAMPLES_PASSED));
      glRun(glDisable(GL_SCISSOR_TEST));

      unsigned int res = 0;
      glRun(glGetQueryObjectuiv(state.pickingQuery(), GL_QUERY_RESULT, &res));

      if (res > 0) state.setPicked(o, m);
    }
    m->render(state);

    if (material) state.popMaterial();
  }

  for (int i = 0; i < node.children.size(); ++i)
    render(o, state, *node.children[i]);

  glPopMatrix();
}

QVariantMap Model::save() const {
  QVariantMap map = SceneObject::save();
  if (m_builtin) {
    map["built-in"] = m_node->name;
  }
  return map;
}

void Model::load(QVariantMap map) {
  SceneObject::load(map);
}

ModelPtr Model::clone() {
  return ModelPtr(new Model(*this));
}

ModelPtr Model::createBuiltin(const QString& name, const QString& model_name) {
  /// @todo tidy this up
  if (model_name == "teapot") {
    ModelPtr m(new Model(name));
    m->m_builtin = true;
    m->node()->name = model_name;
    m->node()->meshes << MeshPtr(new Teapot);
    return m;
  } else if (model_name == "box") {
    ModelPtr m(new Model(name));
    m->m_builtin = true;
    m->node()->name = model_name;
    m->node()->meshes << MeshPtr(new Box);
    return m;
  } else if (model_name == "sphere") {
    ModelPtr m(new Model(name));
    m->m_builtin = true;
    m->node()->name = model_name;
    m->node()->meshes << MeshPtr(new Sphere);
    return m;
  }

  Log::error("Unknown builtin '%s'", model_name.toUtf8().data());
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

void Sphere::renderObj(State&) {
  glutSolidSphere(5.0f, 32, 32);
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
