/**
 * Copyright 2010 Riku Palomäki.
 * This file is part of GLSL Lab.
 *
 * GLSL Lab is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GLSL Lab is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GLSL Lab.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "scene.hpp"
#include "state.hpp"
#include "renderpass.hpp"
#include "object3d.hpp"
#include "json_value.hpp"
#include "light.hpp"
#include "camera.hpp"
#include "shader/program.hpp"
#include "shader/shader.hpp"

#include <QtOpenGL>

/// @todo move to Object3D
static ObjectPtr createObject(const QString& name) {
  if (name == "teapot") {
    return ObjectPtr(new Teapot);
  } else if (name == "box") {
    return ObjectPtr(new Box);
  }
  return ObjectPtr();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

Scene::Scene() : m_width(-1), m_height(-1) {}

void Scene::resize(int width, int height) {
  if (width == m_width && height == m_height) return;

  m_width = width;
  m_height = height;
}

void Scene::render() {
  State state;
  for (RenderPasses::iterator it = m_render_passes.begin(); it != m_render_passes.end(); ++it) {
    (*it)->render(state);
  }
}

ShaderPtr Scene::shaderByFilename(const QString& filename) {
  for (std::map<QString, ProgramPtr>::iterator it = m_shaders.begin(); it != m_shaders.end(); ++it) {
    GLProgram::Shaders shaders = it->second->shaders();
    for (GLProgram::Shaders::iterator it2 = shaders.begin(); it2 != shaders.end(); ++it2) {
      if ((*it2)->filename() == filename) return *it2;
    }
  }
  return ShaderPtr();
}

void Scene::load(const Value& value) {
  Value::Map vm = value.map("objects");
  for (Value::Map::iterator it = vm.begin(); it != vm.end(); ++it) {
    ObjectPtr object;
    QString type = it->second.str("model.0");
    QString name = it->second.str("model.1");
    if (type == "built-in") {
      object = createObject(name);
    } else if (type == "file") {
      //object = loadModel(name);
    }

    if (object) m_objects[it->first] = object;
  }

  vm = value.map("lights");
  for (Value::Map::iterator it = vm.begin(); it != vm.end(); ++it) {
    LightPtr light(new Light(it->first));
    light->load(it->second);
    m_lights[it->first] = light;
  }


  vm = value.map("cameras");
  for (Value::Map::iterator it = vm.begin(); it != vm.end(); ++it) {
    CameraPtr camera(new Camera(it->first));
    camera->load(it->second);
    m_cameras[it->first] = camera;
  }

  vm = value.map("shaders");
  for (Value::Map::iterator it = vm.begin(); it != vm.end(); ++it) {
    ProgramPtr shader(new GLProgram(it->first));
    Value& v = it->second;
    /// @todo change the format so that there can be many shaders of same type
    if (v.have("fragment")) shader->addShader(v.str("fragment"), QGLShader::Fragment);
    if (v.have("vertex")) shader->addShader(v.str("vertex"), QGLShader::Vertex);
    if (v.have("geometry")) shader->addShader(v.str("geometry"), QGLShader::Geometry);
    m_shaders[it->first] = shader;
  }

  vm = value.map("render passes");
  for (Value::Map::iterator it = vm.begin(); it != vm.end(); ++it) {
    RenderPassPtr pass(new RenderPass(shared_from_this()));
    pass->load(it->second);
    m_render_passes.push_back(pass);
  }
}
