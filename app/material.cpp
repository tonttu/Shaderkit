#include "material.hpp"
#include "utils.hpp"
#include "shader/program.hpp"
#include "state.hpp"
#include "texture.hpp"
#include "properties.hpp"

Material::Material(QString name) : m_name(name), m_prog_binded(false) {
}

void Material::addTexture(QString name, TexturePtr tex) {
  m_textures[Utils::uniqueName(name, m_textures.keys())] = tex;
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

void Material::bind(State& state) {
  m_prog_binded = m_program ? m_program->bind() : false;

  if (m_prog_binded) {
    m_program->setUniform(m_uniform_list);
    /// @todo handle these magical variables somehow better
    m_program->setUniform("time", state.time());
  }

  foreach (QString name, m_textures.keys()) {
    int unit = state.reserveTexUnit(this, name);
    m_textures[name]->bind(unit);
    if (m_prog_binded)
      m_program->setUniform(name, unit);
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

void Material::setProg(ProgramPtr prog) {
  m_program = prog;
}

void Material::load(QVariantMap map) {
  /// @todo
}

MaterialPtr Material::clone() const {
  MaterialPtr m(new Material(m_name));
  m->colors = colors;
  m->style = style;
  m->m_program = m_program;
  return m;
}
