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

#include "forward.hpp"
#include "gl/state.hpp"
#include "gl/camera.hpp"
#include "gl/program.hpp"
#include "gl/light.hpp"
#include "core/material.hpp"
#include "core/scene.hpp"

namespace
{
  using namespace Shaderkit;

  void setUniformSel(State* state, GLProgram& prog, const QString& name,
                     const Eigen::Vector4f& v, const std::vector<int>& sel)
  {
    if (sel.empty()) {
      prog.setUniform(state, name, v);
    } else if (sel.size() == 1) {
      prog.setUniform(state, name, v[sel[0]]);
    } else if (sel.size() == 2) {
      prog.setUniform(state, name, Eigen::Vector2f(v[sel[0]], v[sel[1]]));
    } else if (sel.size() == 3) {
      prog.setUniform(state, name, Eigen::Vector3f(v[sel[0]], v[sel[1]], v[sel[2]]));
    } else if (sel.size() == 4) {
      prog.setUniform(state, name, Eigen::Vector4f(v[sel[0]], v[sel[1]], v[sel[2]], v[sel[3]]));
    } else assert(false);
  }

  void setUniformSel(State* state, GLProgram& prog, const QString& name,
                     const Eigen::Vector3f& v, const std::vector<int>& sel)
  {
    if (sel.empty())
      prog.setUniform(state, name, v);
    else
      setUniformSel(state, prog, name, Eigen::Vector4f(v[0], v[1], v[2], 1.f), sel);
  }

  void setUniformSel(State* state, GLProgram& prog, const QString& name,
                     float v, const std::vector<int>&)
  {
    prog.setUniform(state, name, v);
  }

  void assignValues(std::vector<float>::iterator& output,
                    float v, const std::vector<int>& sel)
  {
    *output++ = v;
  }

  void assignValues(std::vector<float>::iterator& output,
                    const Eigen::Vector4f& v, const std::vector<int>& sel)
  {
    if (sel.empty()) {
      std::copy(v.data(), v.data() + 4, output);
      output += 4;
    } else if (sel.size() <= 4) {
      for (int i = 0; i < sel.size(); ++i)
        *output++ = v[sel[i]];
    } else assert(false);
  }

  void assignValues(std::vector<float>::iterator& output,
                    const Eigen::Vector3f& v, const std::vector<int>& sel)
  {
    if (sel.empty()) {
      std::copy(v.data(), v.data() + 3, output);
      output += 3;
    } else assignValues(output, Eigen::Vector4f(v[0], v[1], v[2], 1.0f), sel);
  }

  template <typename T>
  void assignVector(State* state, GLProgram& prog, const QString& name,
                    const MappableValue& map, const QMap<int, Light*>& lights, T f)
  {
    if (map.srcindex() < 0) {
      int vector_dimension = map.select().size() ? map.select().size() : 4;
      std::vector<float> values(vector_dimension * lights.size());
      auto output = values.begin();
      for (auto lit = lights.begin(); lit != lights.end(); ++lit) {
        const Light& l = **lit;
        const auto& v = (l.*f)();
        assignValues(output, v, map.select());
      }
      prog.setUniform(state, name, vector_dimension, values);
    } else if (lights.contains(map.srcindex())) {
      const Light& l = *lights[map.srcindex()];
      const auto& v = (l.*f)();
      setUniformSel(state, prog, name, v, map.select());
    }
  }

}


/// @todo maybe this should be in core, because of this uses material

State::State(Scene& scene, float time, float dt)
  : m_scene(scene), m_time(time), m_dt(dt), m_picking(false)
{
  m_data.push_back(Data());
  m_transforms.push_back(Eigen::Affine3f::Identity());
}

int State::nextFreeLight() const
{
  return nextFree(m_data.back().m_lights.keys().toSet());
}

void State::setLight(int light_id, Light* light)
{
  if (light) {
    m_data.back().m_lights[light_id] = light;
    enable(GL_LIGHTING);
    enable(GL_LIGHT0 + light_id);
  } else {
    m_data.back().m_lights.remove(light_id);
    disable(GL_LIGHT0 + light_id);
  }
}

namespace Shaderkit
{

  void State::enable(GLenum cap)
  {
    glEnable(cap);
  }

  void State::disable(GLenum cap)
  {
    glDisable(cap);
  }

  int State::reserveTexUnit(void* keyptr, QString keyname)
  {
    typedef QPair<void*, QString> P;
    QMap<P, int>& map = m_data.back().m_texunits;
    P key(keyptr, keyname);

    if (map.contains(key)) return map[key];

    int unit = nextFree(map.values().toSet());
    map[key] = unit;

    return unit;
  }

  void State::push()
  {
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
    m_data.push_back(m_data.back());
  }

  void State::pop()
  {
    glRun(glPopClientAttrib());
    glRun(glPopAttrib());
    if (m_data.size() <= 1) {
      Log::error("State push/pop mismatch");
    } else {
      m_data.pop_back();
    }
  }

  void State::pushMaterial(MaterialPtr m)
  {
    if (!m_materials.isEmpty() && m_materials.back()) m_materials.back()->unbind();
    m_materials.push_back(m);
    m_usedMaterials << m;
    push();
    if (m) m->bind(*this);
  }

  void State::setSelection(QList<ObjectPtr> objects)
  {
    m_selection = objects;
  }

  QList<ObjectPtr> State::selection() const
  {
    return m_selection;
  }

  MaterialPtr State::material() const
  {
    if (m_materials.isEmpty()) return MaterialPtr();
    return m_materials.back();
  }

  void State::popMaterial()
  {
    if (m_materials.isEmpty()) {
      Log::error("State material push/pop mismatch");
    } else {
      if (m_materials.back()) m_materials.back()->unbind();
      m_materials.pop_back();
      pop();
      if (!m_materials.isEmpty() && m_materials.back()) m_materials.back()->bind(*this);
    }
  }

  void State::pushModel(const Eigen::Affine3f& transform)
  {
    m_transforms.push_back(m_transforms.back() * transform);
  }

  void State::popModel()
  {
    if (m_transforms.size() <= 1) {
      Log::error("State transform push/pop mismatch");
    } else {
      m_transforms.pop_back();
    }
  }

  const Eigen::Affine3f& State::model() const
  {
    return m_transforms.back();
  }

  VertexAttrib& State::attr()
  {
    return m_data.back().m_attr;
  }

  Eigen::Projective3f State::transform(bool swap_y) const
  {
    CameraPtr c = camera();
    if (!c) {
      Log::error("No valid camera in State::transform!");
      return Eigen::Projective3f::Identity();
    }

    return c->transform(swap_y) * model();
  }

  void State::setCamera(CameraPtr camera)
  {
    m_data.back().m_camera = camera;
  }

  CameraPtr State::camera() const
  {
    return m_data.back().m_camera;
  }


  int State::nextFree(const QSet<int>& lst, int id) const
  {
    while (lst.contains(id)) ++id;
    return id;
  }

  void State::setPicking(QPoint pos)
  {
    m_picking_point = pos;
    m_picking = true;
    m_picked = QPair<ObjectPtr, MeshPtr>();
  }

  QPoint State::pickingPos() const
  {
    return m_picking_point;
  }

  bool State::picking() const
  {
    return m_picking;
  }

  void State::setPicked(ObjectPtr o, MeshPtr m)
  {
    m_picked = qMakePair(o, m);
  }

  QPair<ObjectPtr, MeshPtr> State::picked()
  {
    return m_picked;
  }

  void State::disablePicking()
  {
    m_picking = false;
  }

  unsigned int State::pickingQuery()
  {
    /// @todo is static so great idea? not
    static unsigned int s_picking_query = 0;
    if (s_picking_query == 0) {
      glRun(glGenQueries(1, &s_picking_query));
    }
    return s_picking_query;
  }

  void State::applyUniformsMappings()
  {
    if (m_materials.isEmpty()) return;
    Material& mat = *m_materials.back();
    /// @todo should work without shaders
    if (!mat.prog()) return;
    GLProgram& prog = *mat.prog();
    CameraPtr c = camera();
    for (auto it = mat.uniformMap().begin(); it != mat.uniformMap().end(); ++it) {
      const MappableValue& map = *it;
      if (map.src() == "model") {
        if (map.var() == "transform")
          setUniform(prog, it.key(), model());
        else if (map.var() == "modelview" && c)
          setUniform(prog, it.key(), c->view() * model());
        else assert(false);
      }

      else if (map.src() == "material") {
        if (map.var() == "diffuse")
          setUniformSel(this, prog, it.key(), mat.colors.diffuse, map.select());
        else if (map.var() == "ambient")
          setUniformSel(this, prog, it.key(), mat.colors.ambient, map.select());
        else if (map.var() == "specular")
          setUniformSel(this, prog, it.key(), mat.colors.specular, map.select());
        else if (map.var() == "transparent")
          setUniformSel(this, prog, it.key(), mat.colors.transparent, map.select());
        else if (map.var() == "emissive")
          setUniformSel(this, prog, it.key(), mat.colors.emissive, map.select());

        else if (map.var() == "wireframe")
          prog.setUniform(this, it.key(), mat.style.wireframe);
        else if (map.var() == "twosided")
          prog.setUniform(this, it.key(), mat.style.twosided);

        else if (map.var() == "opacity")
          prog.setUniform(this, it.key(), mat.style.opacity);
        else if (map.var() == "shininess")
          prog.setUniform(this, it.key(), mat.style.shininess);
        else if (map.var() == "shininess_strength")
          prog.setUniform(this, it.key(), mat.style.shininess_strength);
        else if (map.var() == "refracti")
          prog.setUniform(this, it.key(), mat.style.refracti);
        else assert(false);
      }

      else if (map.src() == "scene") {
        if (map.var() == "width")
          prog.setUniform(this, it.key(), m_scene.width());
        else if (map.var() == "height")
          prog.setUniform(this, it.key(), m_scene.width());
        else if (map.var() == "time")
          prog.setUniform(this, it.key(), m_time);
        else if (map.var() == "dt")
          prog.setUniform(this, it.key(), m_dt);
        else assert(false);
      }

      else if (map.src() == "camera" && c) {
        if (map.var() == "target")
          setUniformSel(this, prog, it.key(), c->target(), map.select());
        else if (map.var() == "up")
          setUniformSel(this, prog, it.key(), c->up(), map.select());
        else if (map.var() == "right")
          setUniformSel(this, prog, it.key(), c->right(), map.select());
        else if (map.var() == "front")
          setUniformSel(this, prog, it.key(), c->front(), map.select());

        else if (map.var() == "view")
          setUniform(prog, it.key(), c->view());
        else if (map.var() == "projection")
          prog.setUniform(this, it.key(), c->projection());

        else if (map.var() == "dist")
          prog.setUniform(this, it.key(), c->dist());
        else if (map.var() == "fov")
          prog.setUniform(this, it.key(), c->fov());
        else if (map.var() == "near")
          prog.setUniform(this, it.key(), c->near());
        else if (map.var() == "far")
          prog.setUniform(this, it.key(), c->far());
        else if (map.var() == "width")
          prog.setUniform(this, it.key(), c->width());
        else if (map.var() == "height")
          prog.setUniform(this, it.key(), c->height());
        else assert(false);
      }

      else if (map.src() == "lights" || map.src() == "light") {
        if (map.var() == "ambient")
          assignVector(this, prog, it.key(), map, m_data.back().m_lights, &Light::ambient);
        else if (map.var() == "diffuse")
          assignVector(this, prog, it.key(), map, m_data.back().m_lights, &Light::diffuse);
        else if (map.var() == "specular")
          assignVector(this, prog, it.key(), map, m_data.back().m_lights, &Light::specular);
        else if (map.var() == "direction")
          assignVector(this, prog, it.key(), map, m_data.back().m_lights, &Light::direction);
        else if (map.var() == "location")
          assignVector(this, prog, it.key(), map, m_data.back().m_lights, &Light::location);
        else if (map.var() == "target")
          assignVector(this, prog, it.key(), map, m_data.back().m_lights, &Light::target);
        else if (map.var() == "spot_cutoff")
          assignVector(this, prog, it.key(), map, m_data.back().m_lights, &Light::spotCutoff);
        else assert(false);

      } else assert(false);
    }
  }

  void State::setUniform(GLProgram& prog, const QString& name, const Eigen::Affine3f& m)
  {
    /// @todo should work with mat4 and mat3
    prog.setUniform(this, name, m);
  }

} // namespace Shaderkit
