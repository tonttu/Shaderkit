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

#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include "forward.hpp"
#include "color.hpp"

#include "core/attribute.hpp"

#include "gl/error.hpp"
#include "gl/fbo.hpp"
#include "gl/state.hpp"
#include "gl/uniform.hpp"
#include "gl/attribute_var.hpp"
#include "gl/scene_object.hpp"

#include <QString>

namespace Shaderkit
{

  /**
   * This is used to make a mapping between uniform variables / attributes
   * and Shaderkit variables. This class doesn't hold the uniform / attribute name,
   * only the part that exactly defines one Shaderkit variable.
   *
   * Format:
   * src[srcindex].var[varindex].select
   *
   * Examples:
   * lights[2].location.xy
   * light.location.xyz
   * light.location
   * mesh.color[2].r
   *
   * Configuration file example:
   * "uniforms" : {
   *   "color" : {
   *     "map" : "material.diffuse"
   *   }
   * }
   * that would mean that the uniform variable named color is automatically
   * set to active Material::Colors::diffuse.
   */
  class MappableValue
  {
  public:
    MappableValue(const QString& src, const QString& var, int srcindex, int varindex,
                  const QString& selection, const QString& orig);
    MappableValue() : m_srcindex(-1), m_varindex(-1) {}

    /// Variable category, for example "camera" or "material"
    /// This is the name before .
    const QString& src() const { return m_src; }

    /// Variable name, for example "diffuse"
    /// This is the name after .
    const QString& var() const { return m_var; }

    /// When using syntax "light[2].location", this is the index of the category
    /// (2 in the example). Normally this is just -1
    int srcindex() const { return m_srcindex; }

    /// 2 in "mesh.color[2].r"
    int varindex() const { return m_varindex; }

    /// It's possible to use GLSL-like one to four component selection syntax
    /// For example "material.emissive.rr" (vec2) or "material.ambient.argb" (vec4)
    /// The supported names are xyzw / rgba / stpq
    /// @return The list of indices or empty vector if no selection syntax was used
    const std::vector<int>& select() const { return m_select; }

    static MappableValue parse(const QString& input);

    QString toString() const { return m_orig; }

    bool operator==(const MappableValue& o) const {
      return m_orig == o.m_orig &&
             m_src == o.m_src &&
             m_var == o.m_var &&
             m_srcindex == o.m_srcindex &&
             m_varindex == o.m_varindex &&
             m_select == o.m_select;
    }

    bool operator!=(const MappableValue& o) const {
      return !(*this == o);
    }

  private:
    QString m_orig;
    QString m_src, m_var;
    int m_srcindex;
    int m_varindex;
    std::vector<int> m_select;
  };

  class Material : public QObject, public std::enable_shared_from_this<Material>,
    public SceneObject
  {
    Q_OBJECT

  public:
    Material(QString name);

    void addTexture(QString name, TexturePtr tex);
    void setTexture(QString name, TexturePtr tex);
    void removeTexture(TexturePtr tex);

    void setAttributeMapping(const QString& name, const QString& attr);
    void setUniformMapping(const QString& name, const QString& attr);

    struct Colors {
      Colors(Material& material);
      Colors(Material& material, const Colors& copy);

      Attribute<Color> diffuse;
      Attribute<Color> specular;
      Attribute<Color> ambient;
      Attribute<Color> emissive;
      Attribute<Color> transparent;
    } colors;

    struct Style {
      Style(Material& material);
      Style(Material& material, const Style& copy);

      Attribute<bool> wireframe;
      Attribute<bool> twosided;

      Attribute<QString> shading_model;
      Attribute<QString> blend_mode;

      Attribute<float> opacity;
      Attribute<float> shininess;
      Attribute<float> shininess_strength;
      Attribute<float> refracti;
    } style;

    UniformVar::List& uniformList() { return m_uniform_list; }
    AttributeVar::List& attributeList() { return m_attribute_list; }

    ProgramPtr prog() const { return m_program; }
    ProgramPtr prog(bool create_if_not_found);

    void setScene(ScenePtr scene);
    ScenePtr scene() const { return m_scene; }

    QVariantMap toMap() const;
    void load(Scene& scene, QVariantMap map);
    /// Does not clone textures!
    MaterialPtr clone() const;
    MaterialPtr clone(bool clone_textures) const;

    QStringList textureNames() const { return m_textures.keys(); }
    TexturePtr texture(QString key) { return m_textures.value(key); }
//  QMap<QString, TexturePtr> in() const { return m_in; }

    const QMap<QString, MappableValue>& attributeMap() const { return m_attributeMap; }
    const QMap<QString, MappableValue>& uniformMap() const { return m_uniformMap; }

    virtual void attributeChanged();

  signals:
    void changed(MaterialPtr);
    void shaderChanged(ShaderPtr);
    void progLinked(ShaderErrorList);
    void progCompiled(ShaderErrorList);

  private slots:
    void progChanged();

  protected:
    explicit Material(const Material& m);

  private:
    friend void State::pushMaterial(MaterialPtr);
    friend void State::popMaterial();
    void bind(State& state);
    void unbind();

    /// The shader used to render this material
    /// @see m_uniform_list
    ProgramPtr m_program;
    UniformVar::List m_uniform_list, m_uniform_list_prev;
    AttributeVar::List m_attribute_list;

    /// Uniform name => texture
    QMap<QString, TexturePtr> m_textures;
    bool m_prog_binded;

    QMap<QString, MappableValue> m_attributeMap;
    QMap<QString, MappableValue> m_uniformMap;

    ScenePtr m_scene;
  };

} // namespace Shaderkit

#endif // MATERIAL_HPP
