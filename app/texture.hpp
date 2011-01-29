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

class Texture : public FBOImage {
public:
  Texture(QString name);
  virtual ~Texture();

  void setup(unsigned int fbo, int width, int height);

  void bind(int texture = 0);
  void unbind();

  void setParam(unsigned int pname, int param);
  void setParam(unsigned int pname, float param);

  QString imageClass() const { return "texture"; }

private:
  struct Param {
    Param(int v = 0) : is_float(false), i(v) {}
    Param(float v) : is_float(true), f(v) {}
    bool is_float;
    union {
      float f;
      int i;
    };
  };

  QMap<unsigned int, Param> m_params;
  unsigned int m_id, m_bindedTexture;
};

#endif // TEXTURE_HPP
