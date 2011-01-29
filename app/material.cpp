#include "material.hpp"
#include "utils.hpp"

Material::Material(QString name) : m_name(name) {
}

void Material::addTexture(FBOImagePtr tex) {
  m_textures[Utils::uniqueName(tex->name(), m_textures.keys())] = tex;
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
