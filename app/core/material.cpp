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
#include "core/template_builder.hpp"
#include "gl/program.hpp"
#include "gl/state.hpp"
#include "gl/texture.hpp"
#include "gl/camera.hpp"
#include "gui/properties.hpp"

namespace Shaderkit
{
  static QString normalizeName(QString name)
  {
    QRegExp camelCase("([a-z])([A-Z])");
    return name.replace(camelCase, "\\1_\\2").toLower();
  }

  static bool typesMatch(const UniformVar& var, const QString& type)
  {
    const int varNumElements = var.arraySize() * var.typeinfo().size;
    GLenum varBaseType = var.typeinfo().base_type;

    int typeNumElements = 1;
    GLenum typeBaseType = GL_FLOAT;
    if (type == "mat4") {
      typeNumElements = 16;
    } else if (type == "vec4") {
      typeNumElements = 4;
    } else if (type == "vec3") {
      typeNumElements = 3;
    } else if (type == "vec2") {
      typeNumElements = 2;
    } else if (type == "bool" || type == "int") {
      typeBaseType = GL_INT;
    }

    return varBaseType == typeBaseType && varNumElements == typeNumElements;
  }

  static float matchScore(QString a, QString b)
  {
    if (a.isEmpty() || b.isEmpty()) return 0.f;

    if (a == b)
      return 1.f;

    if (a.contains(b) || b.contains(a))
      return 0.75f;

    QRegExp invalid("[^a-z0-9_]");
    a.replace(".", "_");
    b.replace(".", "_");
    a.replace(invalid, "");
    b.replace(invalid, "");
    a.replace("modelview", "model_view");
    b.replace("modelview", "model_view");

    if (a.isEmpty() || b.isEmpty()) return 0.f;

    if (a == b)
      return 0.5;

    if (a.contains(b) || b.contains(a))
      return 0.25f;

    QSet<QString> aParts = a.split("_").toSet();
    QSet<QString> bParts = b.split("_").toSet();
    if ((aParts & bParts).size()*2 >= std::min(aParts.size(), bParts.size()))
      return 0.125f;

    return 0.f;
  }

  // Finds the best mapping by optimizing sum of individual scores from string matches
  static void solveMapping(QMap<QString, QStringList> choices,
                           QMap<QString, QString>& bestMapping, float bestScore = 0.f,
                           const QMap<QString, QString>& mapping = QMap<QString, QString>(),
                           float score = 0.f)
  {
    if (score > bestScore) {
      bestMapping = mapping;
      bestScore = score;
    }
    for (auto it = choices.begin(); it != choices.end(); ++it) {
      const QString& normalized = normalizeName(it.key());
      for (auto p: *it) {
        float match = matchScore(normalized, p);
        if (match > 0.f) {
          QMap<QString, QString> newMapping = mapping;
          newMapping[it.key()] = p;
          QMap<QString, QStringList> newChoices = choices;
          newChoices.remove(it.key());
          for (QStringList& lst: newChoices)
            lst.removeOne(p);
          float newScore = score + match;
          solveMapping(newChoices, bestMapping, bestScore, newMapping, newScore);
        }
      }
    }
  }

  MappableValue::MappableValue(const QString& src, const QString& var, int srcindex,
                               int varindex, const QString& selection, const QString& orig)
    : m_orig(orig), m_src(src), m_var(var), m_srcindex(srcindex), m_varindex(varindex),
      m_select(selection.size())
  {
    const QByteArray tst = "xrsygtzbpwaq";
    const QByteArray sel = selection.toUtf8();
    for (int i = 0; i < sel.size(); ++i) {
      m_select[i] = tst.indexOf(sel[i]) / 3;
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

  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  Material::Colors::Colors(Material& material)
    : diffuse(material, Color(0.8f, 0.8f, 0.8f, 1.0f)),
      specular(material, Color(1.0f, 1.0f, 1.0f, 1.0f)),
      ambient(material, Color(1.0f, 1.0f, 1.0f, 1.0f)),
      emissive(material, Color(0.0f, 0.0f, 0.0f, 0.0f)),
      transparent(material, Color(0.0f, 0.0f, 0.0f, 0.0f)) {}

  Material::Colors::Colors(Material& material, const Material::Colors& copy)
    : diffuse(material, copy.diffuse),
      specular(material, copy.specular),
      ambient(material, copy.ambient),
      emissive(material, copy.emissive),
      transparent(material, copy.transparent) {}

  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  Material::Style::Style(Material& material)
    : wireframe(material, false),
      twosided(material, false),
      shading_model(material, "gouraud"),
      blend_mode(material, "default"),
      opacity(material, 1.0f),
      shininess(material, 0.0f),
      shininess_strength(material, 1.0f),
      refracti(material, 1.0f) {}

  Material::Style::Style(Material& material, const Style& copy)
    : wireframe(material, copy.wireframe),
      twosided(material, copy.twosided),
      shading_model(material, copy.shading_model),
      blend_mode(material, copy.blend_mode),
      opacity(material, copy.opacity),
      shininess(material, copy.shininess),
      shininess_strength(material, copy.shininess_strength),
      refracti(material, copy.refracti) {}

  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  Material::Material(QString name)
    : SceneObject(name),
      colors(*this),
      style(*this),
      m_prog_binded(false)
  {
  }

  Material::Material(const Material& m)
    : QObject(),
      std::enable_shared_from_this<Material>(),
      SceneObject(m),
      colors(*this, m.colors),
      style(*this, m.style),
      m_uniform_list(m.m_uniform_list),
      m_attribute_list(m.m_attribute_list),
      m_textures(m.m_textures),
      m_prog_binded(false),
      m_attributeMap(m.m_attributeMap),
      m_uniformMap(m.m_uniformMap),
      m_scene(m.m_scene)
  {
    if (m.m_program) {
      m_program = m.m_program->clone();
      connect(m_program.get(), SIGNAL(changed()), this, SLOT(progChanged()));
      connect(m_program.get(), SIGNAL(linked(ShaderErrorList)),
              this, SIGNAL(progLinked(ShaderErrorList)));
      connect(m_program.get(), SIGNAL(compiled(ShaderErrorList)),
              this, SIGNAL(progCompiled(ShaderErrorList)));
      connect(m_program.get(), SIGNAL(shaderChanged(ShaderPtr)),
              this, SIGNAL(shaderChanged(ShaderPtr)));
    }
  }

  void Material::addTexture(QString name, TexturePtr tex)
  {
    m_textures[Utils::uniqueName(name, m_textures.keys())] = tex;
    emit changed(shared_from_this());
  }

  void Material::setTexture(QString name, TexturePtr tex)
  {
    if (m_textures[name] == tex) return;
    m_textures[name] = tex;
    emit changed(shared_from_this());
  }

  void Material::removeTexture(TexturePtr tex)
  {
    bool diff = false;
    for (auto it = m_textures.begin(); it != m_textures.end();) {
      if (*it == tex) {
        it = m_textures.erase(it);
        diff = true;
      } else ++it;
    }
    if (diff) emit changed(shared_from_this());
  }

  void Material::setAttributeMapping(const QString& name, const QString& attr)
  {
    bool diff = false;
    if (attr.isEmpty()) {
      diff = m_attributeMap.remove(name) > 0;
    } else {
      MappableValue var = MappableValue::parse(attr);
      if (!var.src().isEmpty()) {
        diff = m_attributeMap[name] != var;
        m_attributeMap[name] = var;
      }
    }
    if (diff)
      emit changed(shared_from_this());
  }

  void Material::setUniformMapping(const QString& name, const QString& attr)
  {
    bool diff = false;
    if (attr.isEmpty()) {
      diff = m_uniformMap.remove(name) > 0;
    } else {
      MappableValue var = MappableValue::parse(attr);
      if (!var.src().isEmpty()) {
        diff = m_uniformMap[name] != var;
        m_uniformMap[name] = var;
      }
    }
    if (diff)
      emit changed(shared_from_this());
  }

  void Material::progChanged()
  {
    emit changed(shared_from_this());

    performAutomaticMapping();
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

      if (changed) {
        performAutomaticMapping();
        emit this->changed(shared_from_this());
      }
    }
  }

  void Material::performAutomaticMapping()
  {
    performAutomaticUniformMapping();
    performAutomaticAttributeMapping();
  }

  void Material::performAutomaticUniformMapping()
  {
    // name -> type
    QMap<QString, QString> availableUniforms;
    for (VarGroupDescription& g: UniformVar::builtInVars()) {
      for (VarDescription& v: g.vars) {
        availableUniforms[QString("%1.%2").arg(g.prefix, v.name)] = v.type;
        if (g.prefix == "light") {
          availableUniforms[QString("lights.%1").arg(v.name)] = v.type;
          for (int i = 0; i < 8; ++i) {
            availableUniforms[QString("light[%1].%2").arg(i).arg(v.name)] = v.type;
            availableUniforms[QString("lights[%1].%2").arg(i).arg(v.name)] = v.type;
          }
        } else {
          availableUniforms[QString("%1.%2").arg(g.prefix, v.name)] = v.type;
        }
      }
    }

    // actual name -> normalized name
    QMap<QString, QString> uniforms;
    for (auto p: m_uniform_list)
      uniforms[p.name()] = normalizeName(p.name());
    for (auto& p: m_textures.keys()) {
      uniforms.remove(p);
      availableUniforms.remove(p);
    }
    for (auto& p: m_uniformMap.keys()) {
      uniforms.remove(p);
      availableUniforms.remove(p);
    }

    // actual uniform name -> list of available uniforms that have the correct type
    QMap<QString, QStringList> choices;
    for (const UniformVar& var: m_uniform_list) {
      auto it = uniforms.find(var.name());
      assert(it != uniforms.end());

      for (auto it2 = availableUniforms.begin(); it2 != availableUniforms.end(); ++it2)
        if (typesMatch(var, it2.value()))
          choices[it.key()] << it2.key();
    }

    QMap<QString, QString> bestMapping;
    solveMapping(choices, bestMapping);
    for (auto it = bestMapping.begin(); it != bestMapping.end(); ++it)
      m_uniformMap[it.key()] = MappableValue::parse(it.value());
  }

  void Material::performAutomaticAttributeMapping()
  {
  }

  ProgramPtr Material::prog(bool create_if_not_found)
  {
    if (!create_if_not_found || m_program) return m_program;
    m_program.reset(new GLProgram(name()));
    connect(m_program.get(), SIGNAL(changed()), this, SLOT(progChanged()));
    connect(m_program.get(), SIGNAL(linked(ShaderErrorList)),
            this, SIGNAL(progLinked(ShaderErrorList)));
    connect(m_program.get(), SIGNAL(compiled(ShaderErrorList)),
            this, SIGNAL(progCompiled(ShaderErrorList)));
    connect(m_program.get(), SIGNAL(shaderChanged(ShaderPtr)),
            this, SIGNAL(shaderChanged(ShaderPtr)));
    auto ptr = shared_from_this();
    RenderPass::addCallback([ptr] (State& state) { ptr->autoBuild(state); });
    emit changed(ptr);
    return m_program;
  }

  void Material::setScene(ScenePtr scene)
  {
    if (m_scene == scene) return;
    m_scene = scene;
    emit changed(shared_from_this());
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

    map["diffuse"] = colors.diffuse->toQVariant();
    map["specular"] = colors.specular->toQVariant();
    map["ambient"] = colors.ambient->toQVariant();
    map["emissive"] = colors.emissive->toQVariant();
    map["transparent"] = colors.transparent->toQVariant();

    map["wireframe"] = style.wireframe.value() ? 1 : 0;
    map["twosided"] = style.twosided.value() ? 1 : 0;

    map["shading_model"] = style.shading_model.value();
    map["blend_mode"] = style.blend_mode.value();

    map["opacity"] = style.opacity.value();
    map["shininess"] = style.shininess.value();
    map["shininess_strength"] = style.shininess_strength.value();
    map["refracti"] = style.refracti.value();

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

    if (map.contains("wireframe")) style.wireframe = map["wireframe"].toInt();
    if (map.contains("twosided")) style.twosided = map["twosided"].toInt();

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
    foreach (QString filename, map["tess-ctrl"].toStringList())
      prog(true)->addShader(filename, Shader::TessCtrl);
    foreach (QString filename, map["tess-eval"].toStringList())
      prog(true)->addShader(filename, Shader::TessEval);
    foreach (QString filename, map["compute"].toStringList())
      prog(true)->addShader(filename, Shader::Compute);

    loadTemplateShader(scene.glFormat(), scene.templateBuilder());

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
    m_textures.clear();
    for (auto it = tmp.begin(); it != tmp.end(); ++it)
      addTexture(it.key(), scene.genTexture(it.value().toString()));
  }

  MaterialPtr Material::clone() const
  {
    return MaterialPtr(new Material(*this));
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

  void Material::attributeChanged()
  {
    emit changed(shared_from_this());
  }

  void Material::loadTemplateShader(const QGLFormat& format, const TemplateBuilder& builder)
  {
    if (prog())
      return;

    TemplateBuilder::Query q;
    q.format = format;
    q.textures = m_textures.size();
    q.shading_model = style.shading_model;

    auto tpl = builder.findBestTemplate(q);
    if (tpl.id >= 0) {
      m_program = builder.create(tpl.id);
      if (m_program) {
        auto ptr = shared_from_this();
        RenderPass::addCallback([ptr] (State& state) { ptr->autoBuild(state); });
      }
    }
  }

  void Material::autoBuild(State& state)
  {
    bind(state);
    unbind();
  }

} // namespace Shaderkit
