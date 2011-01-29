#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include "forward.hpp"
#include "fbo.hpp"

#include <QVector3D>
#include <QString>

class Material {
public:
  Material(QString name);

  QString name() const { return m_name; }

  void addTexture(FBOImagePtr tex);

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

private:
  QMap<QString, FBOImagePtr> m_textures;
  QString m_name;
};

#endif // MATERIAL_HPP
