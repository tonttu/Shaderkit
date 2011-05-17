/**
 * Copyright 2010,2011 Riku Palomäki.
 * This file is part of GLSL Lab.
 *
 * GLSL Lab is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License Version 3 as
 * published by the Free Software Foundation.
 *
 * GLSL Lab is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GLSL Lab.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include "forward.hpp"
#include "fbo.hpp"

#include <QVariantMap>

class Texture : public FBOImage {
public:
  enum ParamType { UNKNOWN, ENUM, INT, FLOAT };
  struct Param {
    Param(int v = 0) : is_float(false), i(v) {}
    Param(float v) : is_float(true), f(v) {}
    bool is_float;
    union {
      float f;
      int i;
    };
  };

  Texture(QString name);
  virtual ~Texture();

  void setup(unsigned int fbo, int width, int height);

  virtual void bind(int texture = 0);
  virtual void unbind();

  void setParam(unsigned int pname, int param);
  void setParam(unsigned int pname, float param);
  bool setParam(QString pname, Param param);
  bool setParam(QString pname, QString param);

  QMap<QString, Param> paramStrings() const;
  Param param(QString name) const;

  static QStringList allParams();
  static ParamType paramType(QString name);
  static QStringList paramChoices(QString name);
  static QString enumToString(unsigned int value);

  void setBlend(float value);
  void setUV(int idx);

  QString imageClass() const { return "texture"; }

  virtual TexturePtr clone() const;
  virtual QVariantMap save() const;
  virtual void load(QVariantMap map);

protected:
  void applyParams();

  QMap<unsigned int, Param> m_params;
  unsigned int m_bindedTexture;
  float m_blend;
  int m_uv;
  bool m_paramsDirty;
};

class TextureFile : public Texture {
public:
  TextureFile(QString name);
  virtual ~TextureFile() {}

  void setFile(QString file);
  QString file() const { return m_file; }

  virtual void bind(int texture = 0);

  virtual TexturePtr clone() const;
  virtual QVariantMap save() const;
  virtual void load(QVariantMap map);

private:
  QString m_file, m_loadedFile;
};

#endif // TEXTURE_HPP
