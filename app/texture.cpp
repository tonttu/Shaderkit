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

#include "texture.hpp"
#include "opengl.hpp"

#include <cassert>

#define D_(pname, param, spname, sparam) \
  s_names[pname] = spname, s_names[param] = sparam, \
  s_enums[spname] = pname, s_enums[sparam] = param, \
  s_choices[spname] << sparam
#define D(pname, param) D_(GL_##pname, GL_##param, \
  QString::fromAscii(#pname), QString::fromAscii(#param))

#define D2_(pname, spname, target) s_names[pname] = spname, \
  s_enums[spname] = pname, target << spname
#define D2(pname, target) D2_(GL_##pname, QString::fromAscii(#pname), target)

namespace {
  QMap<GLenum, QString> s_names;
  QMap<QString, GLenum> s_enums;
  QMap<QString, QStringList> s_choices;
  QSet<QString> s_ints;
  QSet<QString> s_floats;

  void s_init() {
    if (s_names.isEmpty()) {
      D(TEXTURE_WRAP_S, CLAMP);
      D(TEXTURE_WRAP_S, CLAMP_TO_EDGE);
      D(TEXTURE_WRAP_S, REPEAT);
      D(TEXTURE_WRAP_S, CLAMP_TO_BORDER);
      D(TEXTURE_WRAP_S, MIRRORED_REPEAT);

      D(TEXTURE_WRAP_T, CLAMP);
      D(TEXTURE_WRAP_T, CLAMP_TO_EDGE);
      D(TEXTURE_WRAP_T, REPEAT);
      D(TEXTURE_WRAP_T, CLAMP_TO_BORDER);
      D(TEXTURE_WRAP_T, MIRRORED_REPEAT);

      D(TEXTURE_WRAP_R, CLAMP);
      D(TEXTURE_WRAP_R, CLAMP_TO_EDGE);
      D(TEXTURE_WRAP_R, REPEAT);
      D(TEXTURE_WRAP_R, CLAMP_TO_BORDER);
      D(TEXTURE_WRAP_R, MIRRORED_REPEAT);

      D(TEXTURE_MIN_FILTER, NEAREST);
      D(TEXTURE_MIN_FILTER, LINEAR);
      D(TEXTURE_MIN_FILTER, NEAREST_MIPMAP_NEAREST);
      D(TEXTURE_MIN_FILTER, NEAREST_MIPMAP_LINEAR);
      D(TEXTURE_MIN_FILTER, LINEAR_MIPMAP_NEAREST);
      D(TEXTURE_MIN_FILTER, LINEAR_MIPMAP_LINEAR);

      D(TEXTURE_MAG_FILTER, NEAREST);
      D(TEXTURE_MAG_FILTER, LINEAR);

      D(DEPTH_TEXTURE_MODE, RED);
      D(DEPTH_TEXTURE_MODE, LUMINANCE);
      D(DEPTH_TEXTURE_MODE, INTENSITY);
      D(DEPTH_TEXTURE_MODE, ALPHA);

      D(TEXTURE_COMPARE_MODE, NONE);
      D(TEXTURE_COMPARE_MODE, COMPARE_REF_TO_TEXTURE);

      D(TEXTURE_COMPARE_FUNC, LEQUAL);
      D(TEXTURE_COMPARE_FUNC, GEQUAL);
      D(TEXTURE_COMPARE_FUNC, LESS);
      D(TEXTURE_COMPARE_FUNC, GREATER);
      D(TEXTURE_COMPARE_FUNC, EQUAL);
      D(TEXTURE_COMPARE_FUNC, NOTEQUAL);
      D(TEXTURE_COMPARE_FUNC, ALWAYS);
      D(TEXTURE_COMPARE_FUNC, NEVER);

      // TEXTURE_BORDER_COLOR? (vector)

      D2(TEXTURE_PRIORITY, s_floats);
      D2(TEXTURE_MIN_LOD, s_floats);
      D2(TEXTURE_MAX_LOD, s_floats);
      D2(TEXTURE_LOD_BIAS, s_floats);
      D2(TEXTURE_BASE_LEVEL, s_ints);
      D2(TEXTURE_MAX_LEVEL, s_ints);
      D2(GENERATE_MIPMAP, s_ints); // actually a bool

      assert(s_names.size() == s_enums.size());
    }
  }
}
#undef D2_
#undef D2
#undef D_
#undef D

Texture::Texture(QString name)
  : FBOImage(name), m_bindedTexture(0),
    m_blend(1.0), m_uv(1), m_paramsDirty(false) {
  s_init();
  setParam(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  setParam(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  setParam(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  setParam(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

Texture::~Texture() {
  if (m_id)
    glDeleteTextures(1, &m_id);
  m_id = 0;
}

void Texture::bind(int texture) {
  m_bindedTexture = texture;
  if (m_id == 0) glRun(glGenTextures(1, &m_id));
  glRun(glActiveTexture(GL_TEXTURE0 + m_bindedTexture));
  glRun(glBindTexture(GL_TEXTURE_2D, m_id));
  if (m_paramsDirty) applyParams();
}

void Texture::unbind() {
  glRun(glActiveTexture(GL_TEXTURE0 + m_bindedTexture));
  glRun(glBindTexture(GL_TEXTURE_2D, 0));
}

void Texture::setParam(unsigned int pname, int param) {
  m_params[pname] = Param(param);
  m_paramsDirty = true;
}

void Texture::setParam(unsigned int pname, float param) {
  m_params[pname] = Param(param);
  m_paramsDirty = true;
}

bool Texture::setParam(QString pname, Param param) {
  if (!s_enums.contains(pname)) {
    Log::error("Invalid enum: %s", pname.toUtf8().data());
    return false;
  }
  GLenum e = s_enums.value(pname);
  if (param.is_float)
    setParam(e, param.f);
  else
    setParam(e, param.i);
  return true;
}

bool Texture::setParam(QString pname, QString param) {
  if (!s_enums.contains(pname)) {
    Log::error("Invalid enum: %s = %s", pname.toUtf8().data(), param.toUtf8().data());
    return false;
  }
  GLenum e = s_enums.value(pname);

  bool ok;
  if (s_choices.contains(pname)) {
    int i = param.toInt(&ok);
    if (ok) {
      setParam(e, i);
      return true;
    } else if (!s_choices.value(pname).contains(param)) {
      Log::error("Invalid value: %s = %s", pname.toUtf8().data(), param.toUtf8().data());
      return false;
    }
    assert(s_enums.contains(param));
    setParam(e, int(s_enums.value(param)));
    return true;
  } else if (s_ints.contains(pname)) {
    int i = param.toInt(&ok);
    if (!ok) {
      Log::error("Invalid value: %s = %s (should be int)", pname.toUtf8().data(), param.toUtf8().data());
      return false;
    }
    setParam(e, i);
    return true;
  } else if (s_floats.contains(pname)) {
    float f = param.toFloat(&ok);
    if (!ok) {
      Log::error("Invalid value: %s = %s (should be float)", pname.toUtf8().data(), param.toUtf8().data());
      return false;
    }
    setParam(e, f);
    return true;
  }
  Log::error("Invalid enum: %s = %s", pname.toUtf8().data(), param.toUtf8().data());
  return false;
}

QMap<QString, Texture::Param> Texture::paramStrings() const {
  QMap<QString, Texture::Param> ret;
  for (QMap<unsigned int, Param>::const_iterator it = m_params.begin(); it != m_params.end(); ++it) {
    ret[s_names.value(it.key())] = it.value();
  }
  return ret;
}

Texture::Param Texture::param(QString name) const {
  if (!s_enums.contains(name)) {
    Log::error("Unknown param %s", name.toUtf8().data());
    return Param();
  }
  GLenum e = s_enums.value(name);
  if (m_params.contains(e)) return m_params.value(e);

  ParamType type = paramType(name);

  if (type == ENUM || type == INT) {
    GLint p;
    glRun(glGetTexParameteriv(GL_TEXTURE_2D, e, &p));
    return Param(p);
  } else if (type == FLOAT) {
    GLfloat p;
    glRun(glGetTexParameterfv(GL_TEXTURE_2D, e, &p));
    return Param(p);
  }
  assert(false && "Shouldn't happen");
  return Param();
}

QStringList Texture::allParams() {
  return (s_choices.keys().toSet() | s_ints | s_floats).toList();
}

Texture::ParamType Texture::paramType(QString name) {
  if (s_choices.contains(name)) {
    return ENUM;
  } else if (s_ints.contains(name)) {
    return INT;
  } else if (s_floats.contains(name)) {
    return FLOAT;
  }
  return ParamType::UNKNOWN;
}

QStringList Texture::paramChoices(QString name) {
  return s_choices.value(name);
}

QString Texture::enumToString(unsigned int value) {
  return s_names.value(value);
}

void Texture::setBlend(float value) {
  m_blend = value;
}

void Texture::setUV(int idx) {
  m_uv = idx;
}

void Texture::setup(unsigned int fbo, int width, int height) {
  if (m_id == 0) glRun(glGenTextures(1, &m_id));

  bool type_changed = m_type != m_active_type,
       size_changed = m_width != width || m_height != height,
       fbo_changed = !m_fbos.contains(fbo);

  if (type_changed || size_changed) {
    bind();
    if (m_type == GL_DEPTH_ATTACHMENT) {
      glRun(glTexImage2D(GL_TEXTURE_2D, 0 /* level */, GL_DEPTH_COMPONENT24, width, height,
                         0 /* border */, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL /* data */));
    } else if (m_type == GL_STENCIL_ATTACHMENT) {
      /// @todo implement
    } else {
      glRun(glTexImage2D(GL_TEXTURE_2D, 0 /* level */, GL_RGBA8, width, height,
                         0 /* border */, GL_RGBA, GL_UNSIGNED_BYTE, NULL /* data */));
    }

    applyParams();
    unbind();
  }

  /// @todo Should we run this if only size was changed?
  if (type_changed || size_changed || fbo_changed)
    glRun(glFramebufferTexture2D(GL_FRAMEBUFFER, m_type, GL_TEXTURE_2D, m_id, 0 /* level */));

  m_width = width;
  m_height = height;
  m_active_type = m_type;
  if (fbo_changed) m_fbos.insert(fbo);
}

void Texture::applyParams() {
  for (QMap<unsigned int, Param>::const_iterator it = m_params.begin(); it != m_params.end(); ++it) {
    if (it->is_float)
      glRun(glTexParameterf(GL_TEXTURE_2D, it.key(), it->f));
    else
      glRun(glTexParameteri(GL_TEXTURE_2D, it.key(), it->i));
  }
  m_paramsDirty = false;
}

TextureFile::TextureFile(QString name) : Texture(name) {}

void TextureFile::setFile(QString file) {
  m_file = file;
}

TexturePtr Texture::clone() const {
  TexturePtr t(new Texture(*this));
  t->m_id = 0;
  t->m_fbos.clear();
  t->m_bindedTexture = 0;
  return t;
}

QVariantMap Texture::save() const {
  QVariantMap map = FBOImage::save();
  map["blend"] = m_blend;
  map["uv"] = m_uv;

  for (QMap<unsigned int, Param>::const_iterator it = m_params.begin(); it != m_params.end(); ++it) {
    QString pname = s_names.value(it.key());
    if (pname.isEmpty()) {
      Log::warn("Unknown texture parameter name %d", it.key());
      continue;
    }

    if (s_ints.contains(pname)) {
      if (it->is_float) {
        Log::warn("Texture parameter %s should be integer", pname.toUtf8().data());
      } else {
        map[pname] = it->i;
      }
    } else if (s_floats.contains(pname)) {
      if (!it->is_float) {
        Log::warn("Texture parameter %s should be float", pname.toUtf8().data());
      } else {
        map[pname] = it->f;
      }
    } else {
      if (it->is_float) {
        Log::warn("Texture parameter %s should be enum", pname.toUtf8().data());
      } else {
        QString param = s_names.value(it->i);
        if (param.isEmpty()) {
          Log::warn("Unknown texture parameter %d", it->i);
        } else {
          if (s_choices.value(pname).contains(param)) {
            map[pname] = param;
          } else {
            Log::warn("Invalid texture parameter setting %s = %s",
                      pname.toUtf8().data(), param.toUtf8().data());
          }
        }
      }
    }
  }

  return map;
}

/// @todo Instead of QVariantMap we should have something that removes used
///       attributes and warns about extra ones
void Texture::load(QVariantMap map) {
  FBOImage::load(map);

  if (map.contains("blend")) m_blend = map["blend"].toFloat();
  if (map.contains("uv")) m_uv = map["uv"].toInt();

  /// @todo allow GL_ -prefix and make case-insensitive search
  foreach (QString pname, s_choices.keys()) {
    if (map.contains(pname)) {
      QString param = map[pname].toString();
      if (s_choices[pname].contains(param)) {
        setParam(s_enums[pname], int(s_enums[param]));
      } else {
        Log::warn("Invalid texture parameter setting %s = %s",
                  pname.toUtf8().data(), param.toUtf8().data());
      }
    }
  }

  foreach (QString pname, s_ints)
    if (map.contains(pname)) setParam(s_enums[pname], map[pname].toInt());

  foreach (QString pname, s_floats)
    if (map.contains(pname)) setParam(s_enums[pname], map[pname].toFloat());
}

TexturePtr TextureFile::clone() const {
  TextureFile* t = new TextureFile(*this);
  t->m_id = 0;
  t->m_fbos.clear();
  t->m_bindedTexture = 0;
  t->m_loadedFile.clear();
  return TexturePtr(t);
}

QVariantMap TextureFile::save() const {
  QVariantMap map = Texture::save();
  /// @todo if this a reference and the file name is unchanged, we should forget
  ///       this. We need a wrapper class for all of these variables that
  ///       remembers their original values from the original source.
  if (!m_file.isEmpty()) map["file"] = m_file;
  return map;
}

void TextureFile::load(QVariantMap map) {
  Texture::load(map);
  if (map.contains("file")) m_file = map["file"].toString();
}

void TextureFile::bind(int texture) {
  bool reload = m_loadedFile != m_file;
  if (reload && m_id > 0) {
    QGLContext* cx = const_cast<QGLContext*>(QGLContext::currentContext());
    cx->deleteTexture(m_id);
    m_id = 0;
    m_width = m_height = 0;
    if (!m_file.isEmpty()) {
      QImage image(m_file);
      if (image.isNull()) {
        Log::error("Failed to load image: %s", m_file.toUtf8().data());
      } else {
        m_id = cx->bindTexture(image, GL_TEXTURE_2D,
                 QGLContext::InvertedYBindOption | QGLContext::MipmapBindOption);
        m_width = image.width();
        m_height = image.height();
        applyParams();
        Log::info("Image %s, #%d %dx%d", m_file.toUtf8().data(), m_id, m_width, m_height);
      }
    }
    m_loadedFile = m_file;
  }
  glRun(glEnable(GL_TEXTURE_2D));
  Texture::bind(texture);
}


