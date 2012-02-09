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

#include "material.hpp"
#include "utils.hpp"
#include "shader/program.hpp"
#include "state.hpp"
#include "texture.hpp"
#include "properties.hpp"
#include "camera.hpp"
#include "scene.hpp"

Material::Material(QString name) : SceneObject(name), m_prog_binded(false) {
}

void Material::addTexture(QString name, TexturePtr tex) {
  m_textures[Utils::uniqueName(name, m_textures.keys())] = tex;
  emit changed(shared_from_this());
}

void Material::setTexture(QString name, TexturePtr tex) {
  m_textures[name] = tex;
  emit changed(shared_from_this());
}

void Material::removeTexture(TexturePtr tex) {
  for (auto it = m_textures.begin(); it != m_textures.end();) {
    if (*it == tex) it = m_textures.erase(it);
    else ++it;
  }
  emit changed(shared_from_this());
}

Material::Colors::Colors()
  : diffuse(0.8f, 0.8f, 0.8f),
    specular(1.0f, 1.0f, 1.0f),
    ambient(1.0f, 1.0f, 1.0f),
    emissive(0.0f, 0.0f, 0.0f),
    transparent(0.0f, 0.0f, 0.0f) {}

Material::Style::Style()
  : wireframe(false),
    twosided(false),
    shading_model("gouraud"),
    blend_mode("default"),
    opacity(1.0f),
    shininess(0.0f),
    shininess_strength(1.0f),
    refracti(1.0f) {}

void Material::progChanged() {
  emit changed(shared_from_this());
}

void Material::bind(State& state) {
  m_prog_binded = m_program ? m_program->bind(&state) : false;

  if (m_prog_binded) {
    m_program->setUniform(m_uniform_list);
    /// @todo handle these magical variables somehow better
    m_program->setUniform(&state, "time", state.time());
    m_program->setUniform(&state, "far", state.camera()->far());
    m_program->setUniform(&state, "near", state.camera()->near());
  }

  foreach (QString name, m_textures.keys()) {
    int unit = state.reserveTexUnit(this, name);
    m_textures[name]->bind(unit);
    if (m_prog_binded)
      m_program->setUniform(&state, name, unit);
  }
}

void Material::unbind() {
  foreach (TexturePtr tex, m_textures)
    tex->unbind();

  if (m_prog_binded) {
    UniformVar::List list = m_program->getUniformList();
    m_program->unbind();

    if(list != m_uniform_list)
      m_uniform_list = list;

    /// @todo why do we need three lists? I bet there was a good reason originally..
    if(list != m_uniform_list_prev) {
      m_uniform_list_prev = list;
      MaterialProperties::instance().update(shared_from_this());
    }
  }
}

ProgramPtr Material::prog(bool create_if_not_found) {
  if (!create_if_not_found || m_program) return m_program;
  m_program.reset(new GLProgram(m_name));
  connect(m_program.get(), SIGNAL(changed()), this, SLOT(progChanged()));
  emit changed(shared_from_this());
  return m_program;
}


void Material::setScene(ScenePtr scene) {
  m_scene = scene;
}

QVariantMap Material::toMap() const {
  QVariantMap map = SceneObject::toMap();

  QVariantMap textures;
  for (auto it = m_textures.begin(); it != m_textures.end(); ++it) {
    if (!*it) continue;
    textures[it.key()] = (*it)->name();
  }
  if (!textures.isEmpty()) map["textures"] = textures;

  map["diffuse"] = toList(colors.diffuse);
  map["specular"] = toList(colors.specular);
  map["ambient"] = toList(colors.ambient);
  map["emissive"] = toList(colors.emissive);
  map["transparent"] = toList(colors.transparent);

  map["wireframe"] = style.wireframe;
  map["twosided"] = style.twosided;

  map["shading_model"] = style.shading_model;
  map["blend_mode"] = style.blend_mode;

  map["opacity"] = style.opacity;
  map["shininess"] = style.shininess;
  map["shininess_strength"] = style.shininess_strength;
  map["refracti"] = style.refracti;

  if (prog()) prog()->toMap(scene(), map);
  return map;
}

void Material::load(Scene& scene, QVariantMap map) {
  SceneObject::load(map);

  if (map.contains("diffuse")) colors.diffuse = toVector(map["diffuse"]);
  if (map.contains("specular")) colors.specular = toVector(map["specular"]);
  if (map.contains("ambient")) colors.ambient = toVector(map["ambient"]);
  if (map.contains("emissive")) colors.emissive = toVector(map["emissive"]);
  if (map.contains("transparent")) colors.transparent = toVector(map["transparent"]);

  if (map.contains("wireframe")) style.wireframe = map["wireframe"].toBool();
  if (map.contains("twosided")) style.twosided = map["twosided"].toBool();

  if (map.contains("shading_model")) style.shading_model = map["shading_model"].toString();
  if (map.contains("blend_mode")) style.blend_mode = map["blend_mode"].toString();

  if (map.contains("opacity")) style.opacity = map["opacity"].toFloat();
  if (map.contains("shininess")) style.shininess = map["shininess"].toFloat();
  if (map.contains("shininess_strength")) style.shininess_strength = map["shininess_strength"].toFloat();
  if (map.contains("refracti")) style.refracti = map["refracti"].toFloat();


  foreach (QString filename, map["fragment"].toStringList())
    prog(true)->addShader(filename, Shader::Fragment);
  foreach (QString filename, map["vertex"].toStringList())
    prog(true)->addShader(filename, Shader::Vertex);
  foreach (QString filename, map["geometry"].toStringList())
    prog(true)->addShader(filename, Shader::Geometry);

  Log::info("Shading model: %s", style.shading_model.toUtf8().data());
  /// @todo add a default shader if the material has shader hint
  ///       and prog is null

  QVariantMap tmp = map["textures"].toMap();
  for (auto it = tmp.begin(); it != tmp.end(); ++it)
    addTexture(it.key(), scene.genTexture(it.value().toString()));
}

MaterialPtr Material::clone() const {
  MaterialPtr m(new Material(m_name));
  m->colors = colors;
  m->style = style;
  m->m_scene = m_scene;

  if (m_program)
    m->m_program = m_program->clone();

  return m;
}

MaterialPtr Material::clone(bool clone_textures) const {
  MaterialPtr m = clone();
  if (clone_textures) {
    for (auto it = m_textures.begin(); it != m_textures.end(); ++it) {
      TexturePtr t = it.value()->clone();
      if (m_scene) m_scene->add(t);
      m->m_textures[it.key()] = t;
    }
  }
  return m;
}
