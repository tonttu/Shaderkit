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

#include "core/material.hpp"
#include "core/utils.hpp"
#include "core/scene.hpp"
#include "gl/program.hpp"
#include "gl/state.hpp"
#include "gl/texture.hpp"
#include "gl/camera.hpp"
#include "gui/properties.hpp"

namespace Shaderkit
{

  MappableValue::MappableValue(const QString& src, const QString& var, int srcindex,
                               int varindex, const QString& selection, const QString& orig)
    : m_orig(orig), m_src(src), m_var(var), m_srcindex(srcindex), m_varindex(varindex),
      m_select(selection.size())
  {
    const QString tst = "xrsygtzbpwaq";
    for (int i = 0; i < selection.size(); ++i) {
      m_select[i] = tst.indexOf(selection[i].toAscii()) / 3;
    }
  }

  MappableValue MappableValue::parse(const QString& input)
  {
    QRegExp re("([a-z]+)"                     // src ("scene")
               "(?:\\[(\\d+)\\])?"            // optional: [4]
               "\\."                          // .
               "([a-z]+)"                     // variable ("width")
               "(?:\\[(\\d+)\\])?"            // optional: [2]
               "(?:\\.([xyzwrgbastpq]{1,4}))?"); // optional .abgr
    if (re.exactMatch(input)) {
      return MappableValue(re.cap(1), re.cap(3),
                           re.cap(2).isEmpty() ? -1 : re.cap(2).toInt(),
                           re.cap(4).isEmpty() ? -1 : re.cap(4).toInt(),
                           re.cap(5), input);
    } else {
      Log::error("Failed to parse %s", input.toUtf8().data());
    }
    return MappableValue();
  }

  Material::Material(QString name) : SceneObject(name), m_prog_binded(false)
  {
  }

  void Material::addTexture(QString name, TexturePtr tex)
  {
    m_textures[Utils::uniqueName(name, m_textures.keys())] = tex;
    emit changed(shared_from_this());
  }

  void Material::setTexture(QString name, TexturePtr tex)
  {
    m_textures[name] = tex;
    emit changed(shared_from_this());
  }

  void Material::removeTexture(TexturePtr tex)
  {
    for (auto it = m_textures.begin(); it != m_textures.end();) {
      if (*it == tex) it = m_textures.erase(it);
      else ++it;
    }
    emit changed(shared_from_this());
  }

  void Material::setAttributeMapping(const QString& name, const QString& attr)
  {
    bool signalChanged = false;
    if (attr.isEmpty()) {
      signalChanged = m_attributeMap.remove(name) > 0;
    } else {
      MappableValue var = MappableValue::parse(attr);
      if (!var.src().isEmpty()) {
        signalChanged = m_attributeMap[name] != var;
        m_attributeMap[name] = var;
      }
    }
    if (signalChanged)
      emit changed(shared_from_this());
  }

  void Material::setUniformMapping(const QString& name, const QString& attr)
  {
    bool signalChanged = false;
    if (attr.isEmpty()) {
      signalChanged = m_uniformMap.remove(name) > 0;
    } else {
      MappableValue var = MappableValue::parse(attr);
      if (!var.src().isEmpty()) {
        signalChanged = m_uniformMap[name] != var;
        m_uniformMap[name] = var;
      }
    }
    if (signalChanged)
      emit changed(shared_from_this());
  }

  Material::Colors::Colors()
    : diffuse(0.8f, 0.8f, 0.8f, 1.0f),
      specular(1.0f, 1.0f, 1.0f, 1.0f),
      ambient(1.0f, 1.0f, 1.0f, 1.0f),
      emissive(0.0f, 0.0f, 0.0f, 0.0f),
      transparent(0.0f, 0.0f, 0.0f, 0.0f) {}

  Material::Style::Style()
    : wireframe(false),
      twosided(false),
      shading_model("gouraud"),
      blend_mode("default"),
      opacity(1.0f),
      shininess(0.0f),
      shininess_strength(1.0f),
      refracti(1.0f) {}

  void Material::progChanged()
  {
    emit changed(shared_from_this());
  }

  void Material::bind(State& state)
  {
    m_prog_binded = m_program ? m_program->bind(&state) : false;

    if (m_prog_binded)
      m_program->setUniform(m_uniform_list);

    foreach (QString name, m_textures.keys()) {
      int unit = state.reserveTexUnit(this, name);
      m_textures[name]->bind(unit);
      if (m_prog_binded)
        m_program->setUniform(&state, name, unit);
    }
  }

  void Material::unbind()
  {
    foreach (TexturePtr tex, m_textures)
      tex->unbind();

    if (m_prog_binded) {
      UniformVar::List ulist = m_program->getUniformList();
      AttributeVar::List alist = m_program->getAttributeList();
      m_program->unbind();

      bool changed = false;
      if (ulist != m_uniform_list)
        m_uniform_list = ulist;

      if (alist != m_attribute_list) {
        changed = true;
        m_attribute_list = alist;
      }

      /// @todo why do we need three lists? I bet there was a good reason originally..
      if (ulist != m_uniform_list_prev) {
        m_uniform_list_prev = ulist;
        changed = true;
      }

      if (changed)
        MaterialProperties::instance().update(shared_from_this());
    }
  }

  ProgramPtr Material::prog(bool create_if_not_found)
  {
    if (!create_if_not_found || m_program) return m_program;
    m_program.reset(new GLProgram(m_name));
    connect(m_program.get(), SIGNAL(changed()), this, SLOT(progChanged()));
    connect(m_program.get(), SIGNAL(linked(ShaderErrorList)),
            this, SIGNAL(progLinked(ShaderErrorList)));
    connect(m_program.get(), SIGNAL(compiled(ShaderErrorList)),
            this, SIGNAL(progCompiled(ShaderErrorList)));
    connect(m_program.get(), SIGNAL(shaderChanged(ShaderPtr)),
            this, SIGNAL(shaderChanged(ShaderPtr)));
    emit changed(shared_from_this());
    return m_program;
  }

  void Material::setScene(ScenePtr scene)
  {
    m_scene = scene;
  }

  QVariantMap Material::toMap() const
  {
    QVariantMap map = SceneObject::toMap();

    QVariantMap tmp;
    for (auto it = m_textures.begin(); it != m_textures.end(); ++it) {
      if (!*it) continue;
      tmp[it.key()] = (*it)->name();
    }
    if (!tmp.isEmpty()) map["textures"] = tmp;
    tmp.clear();

    for (auto it = m_attributeMap.begin(); it != m_attributeMap.end(); ++it) {
      QVariantMap attrMap;
      attrMap["map"] = it.value().toString();
      tmp[it.key()] = attrMap;
    }
    if (!tmp.isEmpty()) map["attributes"] = tmp;
    tmp.clear();

    for (auto it = m_uniformMap.begin(); it != m_uniformMap.end(); ++it) {
      QVariantMap uniformMap;
      uniformMap["map"] = it.value().toString();
      tmp[it.key()] = uniformMap;
    }
    if (!tmp.isEmpty()) map["uniforms"] = tmp;
    tmp.clear();

    map["diffuse"] = colors.diffuse.toQVariant();
    map["specular"] = colors.specular.toQVariant();
    map["ambient"] = colors.ambient.toQVariant();
    map["emissive"] = colors.emissive.toQVariant();
    map["transparent"] = colors.transparent.toQVariant();

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

  void Material::load(Scene& scene, QVariantMap map)
  {
    SceneObject::load(map);

    if (map.contains("diffuse")) colors.diffuse = Color::fromQVariant(map["diffuse"]);
    if (map.contains("specular")) colors.specular = Color::fromQVariant(map["specular"]);
    if (map.contains("ambient")) colors.ambient = Color::fromQVariant(map["ambient"]);
    if (map.contains("emissive")) colors.emissive = Color::fromQVariant(map["emissive"]);
    if (map.contains("transparent")) colors.transparent = Color::fromQVariant(map["transparent"]);

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

    QVariantMap tmp = map["attributes"].toMap();
    for (auto it = tmp.begin(); it != tmp.end(); ++it) {
      QVariantMap attrMap = it.value().toMap();
      if (attrMap.contains("map"))
        setAttributeMapping(it.key(), attrMap["map"].toString());
    }

    tmp = map["uniforms"].toMap();
    for (auto it = tmp.begin(); it != tmp.end(); ++it) {
      QVariantMap uniformMap = it.value().toMap();
      if (uniformMap.contains("map"))
        setUniformMapping(it.key(), uniformMap["map"].toString());
    }

    tmp = map["textures"].toMap();
    for (auto it = tmp.begin(); it != tmp.end(); ++it)
      addTexture(it.key(), scene.genTexture(it.value().toString()));
  }

  MaterialPtr Material::clone() const
  {
    MaterialPtr m(new Material(m_name));
    m->colors = colors;
    m->style = style;
    m->m_scene = m_scene;

    if (m_program) {
      m->m_program = m_program->clone();
      connect(m->m_program.get(), SIGNAL(changed()), m.get(), SLOT(progChanged()));
      connect(m->m_program.get(), SIGNAL(linked(ShaderErrorList)),
              m.get(), SIGNAL(progLinked(ShaderErrorList)));
      connect(m->m_program.get(), SIGNAL(compiled(ShaderErrorList)),
              m.get(), SIGNAL(progCompiled(ShaderErrorList)));
      connect(m->m_program.get(), SIGNAL(shaderChanged(ShaderPtr)),
              m.get(), SIGNAL(shaderChanged(ShaderPtr)));
    }

    return m;
  }

  MaterialPtr Material::clone(bool clone_textures) const
  {
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

} // namespace Shaderkit
