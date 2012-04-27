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

#include "core/model.hpp"
#include "core/object3d.hpp"
#include "core/mesh_manager.hpp"
#include "core/scene.hpp"
#include "core/utils.hpp"
#include "core/material.hpp"

#include "gl/opengl.hpp"
#include "gl/state.hpp"

#include "ext/glut_teapot.hpp"

#include "Eigen/OpenGLSupport"

// #include <GL/glut.h>

namespace Shaderkit {

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

  struct Vertex {
    Eigen::Vector3f point;
    Eigen::Vector2f uv2d;
    Eigen::Vector3f uv3d;
    Eigen::Vector3f normal;
  };

  /// Renders a rectangular box.
  void drawBox(float x, float y, float z, State& state) {
#define N(a, b, c) for (int j=0;j<4;++j) data[j].normal = Eigen::Vector3f(a, b, c)
#define V(a, b, c) data->point = Eigen::Vector3f((2*a-1)*x, (2*b-1)*y, (2*c-1)*z), \
  data->uv3d = Eigen::Vector3f(a,b,c), \
  data->uv2d = Eigen::Vector2f(((data.pos() + 3) / 2) % 2, (data.pos() / 2) % 2), ++data

    BufferObject2 & bo = MeshManager::fetch("drawBox", x, y, z);

    if (bo.size() == 0) {
      bo.link<GL_FLOAT>(&Vertex::point, VertexAttrib::Vertex0);
      bo.link<GL_FLOAT>(&Vertex::uv2d, VertexAttrib::UV0);
      bo.link<GL_FLOAT>(&Vertex::uv3d, VertexAttrib::UV1);
      bo.link<GL_FLOAT>(&Vertex::normal, VertexAttrib::Normal);

      using std::abs;

      BufferObject2::Array<Vertex> data = bo.mapWrite<Vertex>(0, 4*6);

      N(1,0,0); V(1,0,1), V(1,0,0), V(1,1,0), V(1,1,1);

      N(0,0,-1); V(1,0,0), V(0,0,0), V(0,1,0), V(1,1,0);

      N(-1,0,0); V(0,0,0), V(0,0,1), V(0,1,1); V(0,1,0);

      N(0,0,1); V(0,0,1), V(1,0,1), V(1,1,1), V(0,1,1);

      N(0,1,0); V(0,1,1), V(1,1,1), V(1,1,0), V(0,1,0);

      N(0,-1,0); V(0,0,0), V(1,0,0), V(1,0,1), V(0,0,1);
    }

    BufferObject2::BindHolder b = bo.bind(state);

    glDrawArrays(GL_QUADS, 0, 4*6);

#undef V
#undef N
  }

  void drawSphere(float radius, int segments, int stripes, State& state) {
    assert(sizeof(Vertex) == sizeof(float)*(3*3+2));
    BufferObject2 & bo = MeshManager::fetch("drawSphere", radius, segments, stripes);

    // two layers of vertices per triangle strip (stripe)
    int vertices_in_stripe = 2 * (segments + 1);

    if (bo.size() == 0) {
      bo.link<GL_FLOAT>(&Vertex::point, VertexAttrib::Vertex0);
      bo.link<GL_FLOAT>(&Vertex::uv2d, VertexAttrib::UV0);
      bo.link<GL_FLOAT>(&Vertex::uv3d, VertexAttrib::UV1);
      bo.link<GL_FLOAT>(&Vertex::normal, VertexAttrib::Normal);

      float stripe_angle = M_PI / stripes;
      float segment_angle = 2.0f * M_PI / segments;

      int vertex_count = stripes * vertices_in_stripe;

      BufferObject2::Array<Vertex> data = bo.mapWrite<Vertex>(0, vertex_count);

      int i = 0;
      float r2 = 0.5f / radius;
      for (int layer = 0; layer <= stripes; ++layer) {
        float layer_radius = radius * sin(stripe_angle * layer);
        float y = radius * cos(stripe_angle * layer);

        if (layer == 1) data = 1;

        for (int segment = segments; segment >= 0; --segment) {
          float z = layer_radius * sin(segment_angle * segment);
          float x = layer_radius * cos(segment_angle * segment);

          Eigen::Vector3f point(x, y, z);
          Eigen::Vector3f normal = point.normalized();
          Eigen::Vector3f uv3(0.5f + x * r2, 0.5f + y * r2, 0.5f + z * r2);
          Eigen::Vector2f uv2(float(segment) / segments, float(layer) / stripes);

          data->point = point;
          data->uv2d = uv2;
          data->uv3d = uv3;
          data->normal = normal;
          if (layer > 0 && layer < stripes) {
            // waiting for c++11 support..
            // data[vertices_in_stripe - 1] = {point, uv2, uv3, normal};
            Vertex& v = data[vertices_in_stripe - 1];
            v.point = point, v.uv2d = uv2, v.uv3d = uv3, v.normal = normal;
          }
          data += 2;
        }
      }
    }

    BufferObject2::BindHolder b = bo.bind(state);

    for (int stripe = 0; stripe < stripes; ++stripe) {
      glRun(glDrawArrays(GL_TRIANGLE_STRIP, vertices_in_stripe*stripe, vertices_in_stripe));
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

Node::Node() : transform(Eigen::Affine3f::Identity()) {
}

Eigen::AlignedBox<float, 3> Node::bbox() {
  Eigen::AlignedBox<float, 3> bb;
  calcBbox(bb);
  return bb;
}

void Node::calcBbox(Eigen::AlignedBox<float, 3>& bbox, const Eigen::Affine3f& p) const {
  const Eigen::Affine3f& t = transform * p;

  foreach (MeshPtr m, meshes)
    m->calcBbox(t, bbox);

  for (int i = 0; i < children.size(); ++i)
    children[i]->calcBbox(bbox, t);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

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
    /// @todo should use raii
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

    state.applyUniformsMappings();
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

QVariantMap Model::toMap() const {
  QVariantMap map = m_map;
  map.unite(SceneObject::toMap());
  if (m_builtin)
    map["built-in"] = m_node->name;
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
    m_bbox = m_node->bbox();
  }
  return m_bbox;
}

ModelPtr Model::createBuiltin(const QString& name, const QString& model_name, const QVariantMap& map) {
  auto vec3 = map.value("size", QVariantList() << 1.0f << 1.0f << 1.0f);
  const Eigen::Vector3f size = Utils::toVector3(vec3);
  ModelPtr m(new Model(name));
  m->m_map["size"] = vec3;

  if (model_name == "teapot") {
    m->node()->meshes << MeshPtr(new Teapot);
  } else if (model_name == "box") {
    m->node()->meshes << MeshPtr(new Box(size));
  } else if (model_name == "sphere") {
    float s = size[0] + size[1] + size[2];
    m->node()->meshes << MeshPtr(new Sphere(s / 3.0f));
  } else {
    Log::error("Unknown builtin '%s'", model_name.toUtf8().data());
    return ModelPtr();
  }
  m->m_builtin = true;
  m->node()->name = model_name;
  return m;
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
/*  VertexAttrib attrib;
  attrib[VertexAttrib::Vertex0] = 0;
  attrib[VertexAttrib::UV0] = 8;
  attrib[VertexAttrib::Normal] = 2;
  ObjectRenderer::drawSphere(m_size/2, 32, 32, attrib);*/
}

TriMesh::TriMesh()
  : m_indices(GL_ELEMENT_ARRAY_BUFFER) {
}

void TriMesh::calcBbox(const Eigen::Affine3f& transform, Eigen::AlignedBox<float, 3>& bbox) const {
  for (int i = 0, m = vertices.size(); i < m; i += 3)
    bbox.extend(transform * Eigen::Vector3f(vertices[i], vertices[i+1], vertices[i+2]));
}

void TriMesh::renderObj(State& state) {
  state.push();

  /*bool handledVertices = false, handledNormals = false, handledUvs = false,
      handledColors = false;*/

  std::vector<BufferObject2::BindHolder> binds;

  MaterialPtr mat = state.material();
  if (mat && mat->prog()) {
    const QMap<QString, MappableValue>& attrs = mat->attributeMap();
    GLProgram& prog = *mat->prog();
    for (auto it = attrs.begin(); it != attrs.end(); ++it) {
      const MappableValue& map = *it;
      if (map.src() == "mesh" && map.srcindex() == -1 && map.select().empty()) {
        if ((map.var() == "vertex" || map.var() == "vertices") &&
            (map.varindex() == -1 || map.varindex() == 0) &&
            !vertices.empty()) {
          BufferObject2& bo = m_buffers[VertexAttrib::Vertex0];
          bo.set<GL_FLOAT>(VertexAttrib::Vertex0, 3, 0, 0);
          bo.sync(vertices);
          state.attr()[VertexAttrib::Vertex0] = it.key();
          binds.push_back(bo.bind(state));
        }

        else if ((map.var() == "normal" || map.var() == "normals") &&
                 (map.varindex() == -1 || map.varindex() == 0) &&
                 !normals.empty()) {
          BufferObject2& bo = m_buffers[VertexAttrib::Normal];
          bo.set<GL_FLOAT>(VertexAttrib::Normal, 3);
          bo.sync(normals);
          state.attr()[VertexAttrib::Normal] = it.key();
          binds.push_back(bo.bind(state));
        }

        else if ((map.var() == "tangent" || map.var() == "tangents") &&
                 (map.varindex() == -1 || map.varindex() == 0) &&
                 !tangents.empty()) {
          BufferObject2& bo = m_buffers[VertexAttrib::Tangent];
          bo.set<GL_FLOAT>(VertexAttrib::Tangent, 3);
          bo.sync(tangents);
          state.attr()[VertexAttrib::Tangent] = it.key();
          binds.push_back(bo.bind(state));
        }

        else if ((map.var() == "bitangent" || map.var() == "bitangents") &&
                 (map.varindex() == -1 || map.varindex() == 0) &&
                 !bitangents.empty()) {
          BufferObject2& bo = m_buffers[VertexAttrib::Bitangent];
          bo.set<GL_FLOAT>(VertexAttrib::Bitangent, 3);
          bo.sync(bitangents);
          state.attr()[VertexAttrib::Bitangent] = it.key();
          binds.push_back(bo.bind(state));
        }

        else if ((map.var() == "color" || map.var() == "colors") &&
                 (std::max(0, map.varindex()) < colors.size())) {
          int idx = std::max(0, map.varindex());
          assert(idx < 8);
          VertexAttrib::Target target = (VertexAttrib::Target)(VertexAttrib::Color0 + idx);
          BufferObject2& bo = m_buffers[target];
          bo.set<GL_FLOAT>(target, 3);
          bo.sync(colors[idx]);
          state.attr()[target] = it.key();
          binds.push_back(bo.bind(state));
        }

        else if ((map.var() == "uv" || map.var() == "uvs") &&
                 (std::max(0, map.varindex()) < uvs.size())) {
          int idx = std::max(0, map.varindex());
          assert(idx < 8);
          VertexAttrib::Target target = (VertexAttrib::Target)(VertexAttrib::UV0 + idx);
          BufferObject2& bo = m_buffers[target];
          bo.set<GL_FLOAT>(target, uv_components[idx]);
          bo.sync(uvs[idx]);
          state.attr()[target] = it.key();
          binds.push_back(bo.bind(state));
        }

        else assert(false);
      } else assert(false);
    }
  }

/*  if (!vertices.empty())
    m_vertices.enableArray(state, GL_VERTEX_ARRAY, 3, vertices);
  if (!normals.empty())
    m_normals.enableArray(state, GL_NORMAL_ARRAY, 3, normals);
  if (!uvs.empty())
    m_uv0.enableArray(state, GL_TEXTURE_COORD_ARRAY, uv_sizes[0], uvs[0]);
  if (!colors.empty())
    m_color0.enableArray(state, GL_COLOR_ARRAY, 3, colors[0]);
    */
  if (!indices.empty()) {
    m_indices.set<GL_UNSIGNED_INT>(VertexAttrib::Index, 1);
    m_indices.sync(indices);
    BufferObject2::BindHolder b = m_indices.bind(state);

    glRun(glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0));
  }
  state.pop();
}

} // namespace Shaderkit
