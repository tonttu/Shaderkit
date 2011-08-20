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
#include "mesh_manager.hpp"

#include "ext/glut_teapot.hpp"

#include "Eigen/OpenGLSupport"

// #include <GL/glut.h>

namespace ObjectRenderer {
  /// @todo delete
  void drawBox(float x, float y, float z) {
#define N(a, b, c) for (int j=0;j<4;++j) normals[i+j] = Eigen::Vector3f(a, b, c)
#define V(a, b, c) vertices[i] = Eigen::Vector3f((2*a-1)*x, (2*b-1)*y, (2*c-1)*z), \
  uv3d[i] = Eigen::Vector3f(a,b,c), \
  uv2d[i] = Eigen::Vector2f(((i + 3) / 2) % 2, (i / 2) % 2), ++i

    using std::abs;
    Eigen::Vector3f normals[4*6], vertices[4*6], uv3d[4*6];
    Eigen::Vector2f uv2d[4*6];

    int i = 0;
    N(1,0,0); V(1,0,1), V(1,0,0), V(1,1,0), V(1,1,1);

    N(0,0,-1); V(1,0,0), V(0,0,0), V(0,1,0), V(1,1,0);

    N(-1,0,0); V(0,0,0), V(0,0,1), V(0,1,1); V(0,1,0);

    N(0,0,1); V(0,0,1), V(1,0,1), V(1,1,1), V(0,1,1);

    N(0,1,0); V(0,1,1), V(1,1,1), V(1,1,0), V(0,1,0);

    N(0,-1,0); V(0,0,0), V(1,0,0), V(1,0,1), V(0,0,1);

    glRun(glEnableClientState(GL_NORMAL_ARRAY));
    glRun(glEnableClientState(GL_TEXTURE_COORD_ARRAY));
    glRun(glEnableClientState(GL_VERTEX_ARRAY));

    glNormalPointer(GL_FLOAT, (unsigned char*)(normals+1)-(unsigned char*)(normals), normals);
    //glTexCoordPointer(3, GL_FLOAT, (unsigned char*)(uv3d+1)-(unsigned char*)(uv3d), uv3d);
    glTexCoordPointer(2, GL_FLOAT, (unsigned char*)(uv2d+1)-(unsigned char*)(uv2d), uv2d);
    glVertexPointer(3, GL_FLOAT, (unsigned char*)(vertices+1)-(unsigned char*)(vertices), vertices);

    glDrawArrays(GL_QUADS, 0, 4*6);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);

#undef V
#undef N
  }

  void drawSphere(float radius, int segments, int rings) {
    float ring_angle = M_PI / rings;
    float segment_angle = 2.0f * M_PI / segments;

    // two layers of vertices per triangle strip (ring)
    int ring_size = 2 * (segments + 1);
    int count = rings * ring_size;

    std::vector<Eigen::Vector3f> normals(count), vertices(count), uv3d(count);
    std::vector<Eigen::Vector2f> uv2d(count);

    int i = 0;
    float r2 = 0.5f / radius;
    for (int layer = 0; layer <= rings; ++layer) {
      float segment_radius = radius * sin(ring_angle * layer);
      float y = radius * cos(ring_angle * layer);

      if (layer == 1) i = 1;

      for (int segment = segments; segment >= 0; --segment) {
        float z = segment_radius * sin(segment_angle * segment);
        float x = segment_radius * cos(segment_angle * segment);

        Eigen::Vector3f point(x, y, z);
        Eigen::Vector3f normal = point.normalized();
        Eigen::Vector3f uv3(0.5f + x * r2, 0.5f + y * r2, 0.5f + z * r2);
        Eigen::Vector2f uv2(float(segment) / segments, float(layer) / rings);

        normals[i] = normal;
        vertices[i] = point;
        uv3d[i] = uv3;
        uv2d[i] = uv2;
        if (layer > 0 && layer < rings) {
          normals[i + ring_size - 1] = normal;
          vertices[i + ring_size - 1] = point;
          uv3d[i + ring_size - 1] = uv3;
          uv2d[i + ring_size - 1] = uv2;
        }
        i += 2;
      }
    }

    glRun(glVertexAttribPointer(0, 3, GL_FLOAT, false, (char*)&vertices[1] - (char*)&vertices[0], &vertices[0]));
/*    glRun(glEnableClientState(GL_NORMAL_ARRAY));
    glRun(glEnableClientState(GL_TEXTURE_COORD_ARRAY));
    glRun(glEnableClientState(GL_VERTEX_ARRAY));

    glNormalPointer(GL_FLOAT, 0, &normals[0]);
    glTexCoordPointer(3, GL_FLOAT, 0, &uv3d[0]);
    //glTexCoordPointer(2, GL_FLOAT, 0, &uv2d[0]);
    glVertexPointer(3, GL_FLOAT, 0, &vertices[0]);*/

    for (int ring = 0; ring < rings; ++ring) {
      glRun(glDrawArrays(GL_TRIANGLE_STRIP, ring_size*ring, ring_size));
    }
  }
}

Node::Node() : transform(Eigen::Affine3f::Identity()) {
}

Model::Model(QString name) : SceneObject(name), m_dirty(true), m_node(new Node), m_builtin(false) {
  m_node->name = name;
}

void Model::render(ObjectPtr o, State& state, const Node& node) {
  /// @todo forget pushmatrix etc, use state
  state.pushModel(node.transform);
  glPushMatrix();
  glMultMatrix(state.model());

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

  glPopMatrix();

  for (int i = 0; i < node.children.size(); ++i)
    render(o, state, *node.children[i]);

  if (&node == m_node.get() && state.selection().contains(o)) {
    state.push();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    glLineWidth(1.37f);
    glColor4f(1, 1, 1, 0.5f);

    glPushMatrix();
    glMultMatrix(state.model());

    std::vector<float> vertices(8*3*2*3);
    float* v = &vertices[0];
    Eigen::Vector3f mm[2] = {bbox().min(), bbox().max()};
    for (int i = 0; i < 8; ++i) {
      float a[3] = { mm[i&1][0], mm[(i>>1)&1][1], mm[(i>>2)&1][2] };
      float b[3] = { mm[1-(i&1)][0], mm[1-((i>>1)&1)][1], mm[1-((i>>2)&1)][2] };
      for (int j = 0; j < 3; ++j) {
        *v++ = a[0]; *v++ = a[1]; *v++ = a[2];

        *v++ = j == 0 ? a[0] * 0.8f + b[0] * 0.2f : a[0];
        *v++ = j == 1 ? a[1] * 0.8f + b[1] * 0.2f : a[1];
        *v++ = j == 2 ? a[2] * 0.8f + b[2] * 0.2f : a[2];
      }
    }

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, &vertices[0]);
    glDrawArrays(GL_LINES, 0, vertices.size()/3);
    glDisableClientState(GL_VERTEX_ARRAY);

    glRun(glPopMatrix());
    state.pop();
  }
  state.popModel();
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

const Eigen::AlignedBox<float, 3>& Model::bbox() {
  if (m_dirty) {
    m_dirty = false;
    m_bbox.setEmpty();

    calcBbox(Eigen::Affine3f::Identity(), *m_node);
  }
  return m_bbox;
}

void Model::calcBbox(const Eigen::Affine3f& transform, const Node& node) {
  const Eigen::Affine3f& t = node.transform * transform;

  foreach (MeshPtr m, node.meshes)
    m->calcBbox(t, m_bbox);

  for (int i = 0; i < node.children.size(); ++i)
    calcBbox(t, *node.children[i]);
}

ModelPtr Model::createBuiltin(const QString& name, const QString& model_name, const Eigen::Vector3f size) {
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
    m->node()->meshes << MeshPtr(new Box(size));
    return m;
  } else if (model_name == "sphere") {
    ModelPtr m(new Model(name));
    m->m_builtin = true;
    m->node()->name = model_name;
    float s = size[0] + size[1] + size[2];
    m->node()->meshes << MeshPtr(new Sphere(s / 3.0f));
    return m;
  }

  Log::error("Unknown builtin '%s'", model_name.toUtf8().data());
  return ModelPtr();
}

void Mesh::render(State& state) {
  glPushMatrix();
  renderObj(state);
  glRun(glPopMatrix());
}

void Teapot::calcBbox(const Eigen::Affine3f& transform, Eigen::AlignedBox<float, 3>& bbox) const {
  for (int i = 0; i < 8; ++i)
    bbox.extend(transform * Eigen::Vector3f(((i>>2)&1)*7.4-3.7f, ((i>>1)&1)*7.4-3.7f, (i&1)*7.4-3.7f));
}

void Teapot::renderObj(State&) {
  teapot(10, 3.7f, GL_FILL);
}

void Box::calcBbox(const Eigen::Affine3f& transform, Eigen::AlignedBox<float, 3>& bbox) const {
  for (int i = 0; i < 8; ++i)
    bbox.extend(transform * Eigen::Vector3f(((i>>2)&1)*m_size[0]-m_size[0]*0.5f,
                                            ((i>>1)&1)*m_size[1]-m_size[1]*0.5f,
                                            (i&1)*m_size[2]-m_size[2]*0.5f));
}

void Box::renderObj(State&) {
  ObjectRenderer::drawBox(m_size[0]*0.5f, m_size[1]*0.5f, m_size[2]*0.5f);
}

void Sphere::calcBbox(const Eigen::Affine3f& transform, Eigen::AlignedBox<float, 3>& bbox) const {
  for (int i = 0; i < 8; ++i)
    bbox.extend(transform * Eigen::Vector3f(((i>>2)&1)*m_size-m_size*0.5f,
                                            ((i>>1)&1)*m_size-m_size*0.5f,
                                            (i&1)*m_size-m_size*0.5f));
}

void Sphere::renderObj(State&) {
  //ObjectRenderer::drawSphere(m_size/2, 32, 32);
}

void TriMesh::calcBbox(const Eigen::Affine3f& transform, Eigen::AlignedBox<float, 3>& bbox) const {
  for (int i = 0, m = vertices.size(); i < m; i += 3)
    bbox.extend(transform * Eigen::Vector3f(vertices[i], vertices[i+1], vertices[i+2]));
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
