#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include "forward.hpp"
#include "fbo.hpp"
#include "state.hpp"
#include "shader/uniform.hpp"
#include "scene_object.hpp"

#include <QVector3D>
#include <QString>

class Material : public QObject, public std::enable_shared_from_this<Material>,
                 public SceneObject {
  Q_OBJECT

public:
  Material(QString name);

  void addTexture(QString name, TexturePtr tex);
  void setTexture(QString name, TexturePtr tex);
  void removeTexture(TexturePtr tex);

  struct Colors {
    Colors();
    QVector3D diffuse;
    QVector3D specular;
    QVector3D ambient;
    QVector3D emissive;
    QVector3D transparent;
  } colors;

  struct Style {
    Style();

    bool wireframe;
    bool twosided;

    QString shading_model;
    QString blend_mode;

    float opacity;
    float shininess;
    float shininess_strength;
    float refracti;
  } style;

  UniformVar::List & uniformList() { return m_uniform_list; }

  ProgramPtr prog() { return m_program; }
  void setProg(ProgramPtr prog);

  void setScene(ScenePtr scene);
  ScenePtr scene() { return m_scene; }

  QVariantMap save() const;
  void load(QVariantMap map);
  /// Does not clone textures!
  MaterialPtr clone() const;
  MaterialPtr clone(bool clone_textures) const;

  QStringList textureNames() const { return m_textures.keys(); }
  TexturePtr texture(QString key) { return m_textures.value(key); }
//  QMap<QString, TexturePtr> in() const { return m_in; }

signals:
  void changed(MaterialPtr);

private:
  friend void State::pushMaterial(MaterialPtr);
  friend void State::popMaterial();
  void bind(State& state);
  void unbind();

  /// The shader used to render this material, this could be shared between different objects
  /// @see m_uniform_list
  ProgramPtr m_program;
  UniformVar::List m_uniform_list, m_uniform_list_prev;

  QMap<QString, TexturePtr> m_textures;
  bool m_prog_binded;

  ScenePtr m_scene;
};

#endif // MATERIAL_HPP
