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

#include "gl/texture.hpp"
#include "gl/opengl.hpp"

#include <cassert>

#include <QTimer>
#include <QGLContext>
#include <QIcon>

#define D_(pname, param, spname, sparam) \
  s_names[pname] = spname, s_names[param] = sparam, \
  s_enums[spname] = pname, s_enums[sparam] = param, \
  s_choices[spname] << sparam
#define D(pname, param) D_(GL_##pname, GL_##param, \
  QString::fromUtf8(#pname), QString::fromUtf8(#param))

#define D2_(pname, spname, target) s_names[pname] = spname, \
  s_enums[spname] = pname, target << spname
#define D2(pname, target) D2_(GL_##pname, QString::fromUtf8(#pname), target)

#define I_(e, name) s_internalFormats[e] = #name, s_internalFormatsStr[#name] = e
#define I(name) I_(GL_##name, name)

namespace
{
  QMap<GLenum, QString> s_names;
  QMap<QString, GLenum> s_enums;
  QMap<QString, QStringList> s_choices;
  QSet<QString> s_ints;
  QSet<QString> s_floats;
  QMap<int, QString> s_internalFormats;
  QMap<QString, int> s_internalFormatsStr;

  void s_init()
  {
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

      I(RED);
      I(RG);
      I(RGB);
      I(RGBA);

      I(ALPHA);
      I(ALPHA12);
      I(ALPHA16);
      I(ALPHA4);
      I(ALPHA8);
      I(DEPTH_COMPONENT);
      I(DEPTH_COMPONENT16);
      I(DEPTH_COMPONENT24);
      I(DEPTH_COMPONENT32);
      I(DEPTH_COMPONENT32F);
      I(DEPTH24_STENCIL8);
      I(DEPTH32F_STENCIL8);
      I(INTENSITY);
      I(INTENSITY12);
      I(INTENSITY16);
      I(INTENSITY4);
      I(INTENSITY8);
      I(LUMINANCE);
      I(LUMINANCE12);
      I(LUMINANCE12_ALPHA12);
      I(LUMINANCE12_ALPHA4);
      I(LUMINANCE16);
      I(LUMINANCE16_ALPHA16);
      I(LUMINANCE4);
      I(LUMINANCE4_ALPHA4);
      I(LUMINANCE6_ALPHA2);
      I(LUMINANCE8);
      I(LUMINANCE8_ALPHA8);
      I(LUMINANCE_ALPHA);
      I(R11F_G11F_B10F);
      I(R16);
      I(R16F);
      I(R16I);
      I(R16_SNORM);
      I(R16UI);
      I(R32F);
      I(R32I);
      I(R32UI);
      I(R3_G3_B2);
      I(R8);
      I(R8I);
      I(R8_SNORM);
      I(R8UI);
      I(RG16);
      I(RG16F);
      I(RG16I);
      I(RG16_SNORM);
      I(RG16UI);
      I(RG32F);
      I(RG32I);
      I(RG32UI);
      I(RG8);
      I(RG8I);
      I(RG8_SNORM);
      I(RG8UI);
      I(RGB10);
      I(RGB10_A2);
      I(RGB10_A2UI);
      I(RGB12);
      I(RGB16);
      I(RGB16F);
      I(RGB16I);
      I(RGB16_SNORM);
      I(RGB16UI);
      I(RGB32F);
      I(RGB32I);
      I(RGB32UI);
      I(RGB4);
      I(RGB5);
      I(RGB5_A1);
      I(RGB8);
      I(RGB8I);
      I(RGB8_SNORM);
      I(RGB8UI);
      I(RGB9_E5);
      I(RGBA12);
      I(RGBA16);
      I(RGBA16F);
      I(RGBA16I);
      I(RGBA16_SNORM);
      I(RGBA16UI);
      I(RGBA2);
      I(RGBA32F);
      I(RGBA32I);
      I(RGBA32UI);
      I(RGBA4);
      I(RGBA8);
      I(RGBA8I);
      I(RGBA8_SNORM);
      I(RGBA8UI);
      I(SLUMINANCE);
      I(SLUMINANCE8);
      I(SLUMINANCE8_ALPHA8);
      I(SLUMINANCE_ALPHA);
      I(SRGB);
      I(SRGB8);
      I(SRGB8_ALPHA8);
      I(SRGB_ALPHA);

      I(STENCIL);
      I(DEPTH_STENCIL);
      I(STENCIL_INDEX1);
      I(STENCIL_INDEX4);
      I(STENCIL_INDEX8);
      I(STENCIL_INDEX16);

      I(COMPRESSED_RED);
      I(COMPRESSED_RG);
      I(COMPRESSED_RGB);
      I(COMPRESSED_RGBA);
      I(COMPRESSED_SRGB);
      I(COMPRESSED_SRGB_ALPHA);
      I(COMPRESSED_RED_RGTC1);
      I(COMPRESSED_SIGNED_RED_RGTC1);
      I(COMPRESSED_RG_RGTC2);
      I(COMPRESSED_SIGNED_RG_RGTC2);

      I(COMPRESSED_ALPHA);
      I(COMPRESSED_INTENSITY);
      I(COMPRESSED_LUMINANCE);
      I(COMPRESSED_LUMINANCE_ALPHA);

      I(COMPRESSED_RGB_S3TC_DXT1_EXT);
      I(COMPRESSED_RGBA_S3TC_DXT1_EXT);
      I(COMPRESSED_RGBA_S3TC_DXT3_EXT);
      I(COMPRESSED_RGBA_S3TC_DXT5_EXT);

      assert(s_internalFormats.size() == s_internalFormatsStr.size());

      // these are correct, right?
      s_internalFormats[1] = "LUMINANCE",
                             s_internalFormats[2] = "LUMINANCE_ALPHA";
      s_internalFormats[3] = "RGB";
      s_internalFormats[4] = "RGBA";
    }
  }

  Shaderkit::TextureChangeManager* s_instance = 0;
}
#undef I
#undef D2_
#undef D2
#undef D_
#undef D

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

namespace Shaderkit
{

  TextureChangeManager::TextureChangeManager() : m_timer(new QTimer(this))
  {
    assert(!s_instance);
    m_timer->start(100);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(run()));
  }

  TextureChangeManager::~TextureChangeManager()
  {
  }

  void TextureChangeManager::listen(TexturePtr texture, QObject* listener, std::function<void ()> func, bool data)
  {
    if (!s_instance) s_instance = new TextureChangeManager;
    Listener l;
    l.obj = listener;
    l.data = data;
    l.func = func;
    s_instance->m_callbacks[texture.get()] << l;
    connect(listener, SIGNAL(destroyed(QObject*)), s_instance, SLOT(listenerDeleted(QObject*)));
  }

  void TextureChangeManager::forget(TexturePtr texture, QObject* listener)
  {
    if (!s_instance) return;
    disconnect(listener, SIGNAL(destroyed(QObject*)), s_instance, SLOT(listenerDeleted(QObject*)));
    auto it = s_instance->m_callbacks.find(texture.get());
    for (auto it2 = it->begin(); it2 != it->end(); ) {
      if (it2->obj == listener) it2 = it->erase(it2);
      else ++it2;
    }
    if (it->isEmpty()) it = s_instance->m_callbacks.erase(it);
    if (s_instance->m_callbacks.isEmpty()) {
      s_instance->deleteLater();
      s_instance = 0;
    }
  }

  void TextureChangeManager::changed(Texture* tex, bool data)
  {
    if (!s_instance) return;
    (data ? s_instance->m_queueData : s_instance->m_queue) << tex;
  }

  void TextureChangeManager::removed(Texture* tex)
  {
    if (!s_instance) return;
    auto lst = s_instance->m_callbacks.take(tex);
    foreach (const Listener& l, lst)
      if (l.obj)
        disconnect(l.obj, SIGNAL(destroyed(QObject*)), s_instance, SLOT(listenerDeleted(QObject*)));
    if (s_instance->m_callbacks.isEmpty()) {
      s_instance->deleteLater();
      s_instance = 0;
    }
  }

  void TextureChangeManager::listenerDeleted(QObject* obj)
  {
    assert(s_instance == this);
    for (auto it = m_callbacks.begin(); it != m_callbacks.end();) {
      for (auto it2 = it->begin(); it2 != it->end(); ) {
        if (it2->obj == obj) it2 = it->erase(it2);
        else ++it2;
      }
      if (it->isEmpty()) it = m_callbacks.erase(it);
      else ++it;
    }
    if (m_callbacks.isEmpty()) {
      disconnect(m_timer, SIGNAL(timeout()), this, SLOT(run()));
      deleteLater();
      s_instance = 0;
    }
  }

  void TextureChangeManager::run()
  {
    assert(s_instance == this);
    foreach (Texture* tex, m_queue) {
      auto it = m_callbacks.find(tex);
      if (it != m_callbacks.end()) {
        for (auto it2 = it->begin(); it2 != it->end(); ++it2)
          it2->func();
      }
    }
    foreach (Texture* tex, m_queueData - m_queue) {
      auto it = m_callbacks.find(tex);
      if (it != m_callbacks.end()) {
        for (auto it2 = it->begin(); it2 != it->end(); ++it2)
          if (it2->data)
            it2->func();
      }
    }
    m_queue.clear();
    m_queueData.clear();
  }

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

  Texture::Texture(const QString& name)
    : FBOImage(name),
      m_texture_id(0),
      m_bindedTexture(0),
      m_internalFormat(*this, GL_RGBA),
      m_blend(*this, 1.0),
      m_uv(*this, 0),
      m_paramsDirty(false),
      m_dirty(false)
  {
    s_init();
    setParam(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE, true);
    setParam(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE, true);
    setParam(GL_TEXTURE_MAG_FILTER, GL_LINEAR, true);
    setParam(GL_TEXTURE_MIN_FILTER, GL_LINEAR, true);
  }

  Texture::Texture(const Texture& t)
    : FBOImage(t),
      m_texture_id(0),
      m_params(t.m_params),
      m_bindedTexture(0),
      m_internalFormat(*this, t.m_internalFormat),
      m_blend(*this, t.m_blend),
      m_uv(*this, t.m_uv),
      m_paramsDirty(true),
      m_dirty(true)
  {
  }

  Texture::~Texture()
  {
    TextureChangeManager::removed(this);
    if (m_texture_id)
      glDeleteTextures(1, &m_texture_id);
    m_texture_id = 0;
  }

  void Texture::bind(int texture)
  {
    m_bindedTexture = texture;
    if (m_texture_id == 0) glRun(glGenTextures(1, &m_texture_id));
    glRun(glActiveTexture(GL_TEXTURE0 + m_bindedTexture));
    glRun(glBindTexture(GL_TEXTURE_2D, m_texture_id));
    if (m_paramsDirty) applyParams();
  }

  void Texture::unbind()
  {
    glRun(glActiveTexture(GL_TEXTURE0 + m_bindedTexture));
    glRun(glBindTexture(GL_TEXTURE_2D, 0));
  }

  void Texture::setParam(unsigned int pname, int param, bool quiet)
  {
    Param src(param);
    Param& target = m_params[pname];
    if (src == target) return;
    target = src;
    m_paramsDirty = true;
    if (!quiet)
      emit changed(std::static_pointer_cast<Texture>(shared_from_this()));
  }

  void Texture::setParam(unsigned int pname, float param, bool quiet)
  {
    Param src(param);
    Param& target = m_params[pname];
    if (src == target) return;
    target = src;
    m_paramsDirty = true;
    if (!quiet)
      emit changed(std::static_pointer_cast<Texture>(shared_from_this()));
  }

  bool Texture::setParam(QString pname, Param param, bool quiet)
  {
    if (!s_enums.contains(pname)) {
      Log::error("Invalid enum: %s", pname.toUtf8().data());
      return false;
    }
    GLenum e = s_enums.value(pname);
    if (param.is_float)
      setParam(e, param.f, quiet);
    else
      setParam(e, param.i, quiet);
    return true;
  }

  bool Texture::setParam(QString pname, QString param, bool quiet)
  {
    if (!s_enums.contains(pname)) {
      Log::error("Invalid enum: %s = %s", pname.toUtf8().data(), param.toUtf8().data());
      return false;
    }
    GLenum e = s_enums.value(pname);

    bool ok;
    if (s_choices.contains(pname)) {
      int i = param.toInt(&ok);
      if (ok) {
        setParam(e, i, quiet);
        return true;
      } else if (!s_choices.value(pname).contains(param)) {
        Log::error("Invalid value: %s = %s", pname.toUtf8().data(), param.toUtf8().data());
        return false;
      }
      assert(s_enums.contains(param));
      setParam(e, int(s_enums.value(param)), quiet);
      return true;
    } else if (s_ints.contains(pname)) {
      int i = param.toInt(&ok);
      if (!ok) {
        Log::error("Invalid value: %s = %s (should be int)", pname.toUtf8().data(), param.toUtf8().data());
        return false;
      }
      setParam(e, i, quiet);
      return true;
    } else if (s_floats.contains(pname)) {
      float f = param.toFloat(&ok);
      if (!ok) {
        Log::error("Invalid value: %s = %s (should be float)", pname.toUtf8().data(), param.toUtf8().data());
        return false;
      }
      setParam(e, f, quiet);
      return true;
    }
    Log::error("Invalid enum: %s = %s", pname.toUtf8().data(), param.toUtf8().data());
    return false;
  }

  QMap<QString, Texture::Param> Texture::paramStrings() const
  {
    QMap<QString, Texture::Param> ret;
    for (QMap<unsigned int, Param>::const_iterator it = m_params.begin(); it != m_params.end(); ++it) {
      ret[s_names.value(it.key())] = it.value();
    }
    return ret;
  }

  Texture::Param Texture::param(QString name) const
  {
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

  QStringList Texture::allParams()
  {
    return (s_choices.keys().toSet() | s_ints | s_floats).toList();
  }

  Texture::ParamType Texture::paramType(QString name)
  {
    if (s_choices.contains(name)) {
      return ENUM;
    } else if (s_ints.contains(name)) {
      return INT;
    } else if (s_floats.contains(name)) {
      return FLOAT;
    }
    return ParamType::UNKNOWN;
  }

  QStringList Texture::paramChoices(QString name)
  {
    return s_choices.value(name);
  }

  QString Texture::enumToString(unsigned int value)
  {
    return s_names.value(value);
  }

  void Texture::setBlend(float value)
  {
    m_blend = value;
  }

  void Texture::setUV(int idx)
  {
    m_uv = idx;
  }

  void Texture::setup(unsigned int fbo, int width, int height)
  {
    if (m_texture_id == 0) glRun(glGenTextures(1, &m_texture_id));

    bool type_changed = m_attachment != m_active_attachment,
         size_changed = m_width != width || m_height != height,
         fbo_changed = !m_fbo_num != fbo;

    if (m_dirty || type_changed || size_changed) {
      bind();
      if (m_attachment == GL_DEPTH_ATTACHMENT) {
        if (!depthRenderableInternalFormats().contains(m_internalFormat)) {
          m_internalFormat = GL_DEPTH_COMPONENT;
        }
        glRun(glTexImage2D(GL_TEXTURE_2D, 0 /* level */, m_internalFormat, width, height,
                           0 /* border */, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL /* data */));
      } else if (m_attachment == GL_STENCIL_ATTACHMENT) {
        if (!stencilRenderableInternalFormats().contains(m_internalFormat)) {
          m_internalFormat = GL_STENCIL;
        }
        /// @todo implement
      } else { // GL_COLOR_ATTACHMENTN
        if (!colorRenderableInternalFormats().contains(m_internalFormat)) {
          m_internalFormat = GL_RGBA;
        }
        glRun(glTexImage2D(GL_TEXTURE_2D, 0 /* level */, m_internalFormat, width, height,
                           0 /* border */, GL_RGBA, GL_UNSIGNED_BYTE, NULL /* data */));
      }

      applyParams();
      unbind();
    }

    /// @todo Should we run this if only size was changed?
    if (m_dirty || type_changed || size_changed || fbo_changed) {
      glRun(glFramebufferRenderbuffer(GL_FRAMEBUFFER, m_attachment, GL_RENDERBUFFER, 0));
      glRun(glFramebufferTexture2D(GL_FRAMEBUFFER, m_attachment, GL_TEXTURE_2D, m_texture_id, 0 /* level */));
    }

    m_width = width;
    m_height = height;
    m_active_attachment = m_attachment;
    m_dirty = false;
    if (fbo_changed) m_fbo_num = fbo;
  }

  void Texture::applyParams()
  {
    for (QMap<unsigned int, Param>::const_iterator it = m_params.begin(); it != m_params.end(); ++it) {
      if (it->is_float)
        glRun(glTexParameterf(GL_TEXTURE_2D, it.key(), it->f));
      else
        glRun(glTexParameteri(GL_TEXTURE_2D, it.key(), it->i));
    }
    m_paramsDirty = false;
    TextureChangeManager::changed(this);
  }

  TexturePtr Texture::clone() const
  {
    return TexturePtr(new Texture(*this));
  }

  QVariantMap Texture::toMap() const
  {
    QVariantMap map = FBOImage::toMap();
    map["blend"] = m_blend.value();
    map["uv"] = m_uv.value();

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
  void Texture::load(QVariantMap map)
  {
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

  void Texture::setInternalFormat(int format)
  {
    m_internalFormat = format;
    m_dirty = true;
  }

  void Texture::setInternalFormat(QString format)
  {
    if (s_internalFormatsStr.contains(format))
      setInternalFormat(s_internalFormatsStr.value(format));
  }

  QString Texture::internalFormatStr() const
  {
    return s_internalFormats.value(m_internalFormat);
  }

  const QList<QPair<QString, QStringList>>& Texture::internalFormats(bool colorRenderableOnly)
  {
    static QList<QPair<QString, QStringList>> s_res, s_res_color_renderable;
    if (!s_res.isEmpty()) return colorRenderableOnly ? s_res_color_renderable : s_res;

    QSet<QString> colors = colorRenderableInternalFormatsStr();

    QList<QString> all = s_internalFormatsStr.keys();
    qSort(all);

    QList<QPair<QString, QRegExp>> regexps;
    regexps << QPair<QString, QRegExp>("Common", QRegExp("^(RED|RG|RGB|RGBA)$"));
    regexps << QPair<QString, QRegExp>("Float", QRegExp("F$"));
    regexps << QPair<QString, QRegExp>("Integer", QRegExp("[^U]I$"));
    regexps << QPair<QString, QRegExp>("Unsigned integer", QRegExp("UI$"));
    regexps << QPair<QString, QRegExp>("Luminance/Intensity", QRegExp("(LUMINANCE|INTENSITY)"));
    regexps << QPair<QString, QRegExp>("Signed normalized", QRegExp("SNORM$"));
    regexps << QPair<QString, QRegExp>("Stencil", QRegExp("STENCIL"));
    regexps << QPair<QString, QRegExp>("Depth", QRegExp("DEPTH"));
    regexps << QPair<QString, QRegExp>("Compressed", QRegExp("COMPRESSED"));
    regexps << QPair<QString, QRegExp>("Alpha", QRegExp("^ALPHA"));

    QMap<QString, QStringList> tmp1, tmp2;

    foreach (QString format, all) {
      bool match = false;
      for (auto it = regexps.begin(); it != regexps.end(); ++it) {
        QRegExp& e = it->second;
        if (e.indexIn(format) >= 0) {
          tmp1[it->first] << format;
          if (colors.contains(format)) tmp2[it->first] << format;
          match = true;
        }
      }
      if (!match) {
        tmp1["Special"] << format;
        if (colors.contains(format)) tmp2["Special"] << format;
      }
    }

    foreach (auto p, regexps) {
      if (!tmp1[p.first].isEmpty()) s_res << qMakePair(p.first, tmp1[p.first]);
      if (!tmp2[p.first].isEmpty()) s_res_color_renderable << qMakePair(p.first, tmp2[p.first]);
    }

    return colorRenderableOnly ? s_res_color_renderable : s_res;
  }

  const QSet<int>& Texture::colorRenderableInternalFormats()
  {
    /**
     * OpenGL specification 4.1 Core Profile:
     *
     * The following base internal formats from table 3.11 are color-renderable:
     * RED, RG, RGB, and RGBA. The sized internal formats from table 3.12 that
     * have a color-renderable base internal format are also color-renderable. No
     * other formats, including compressed internal formats, are color-renderable.
     *
     * .. side note: Every format in 3.12 have color-renderable base internal
     *               format, so this is set of 3.11, 3.12 and those special
     *               3 / 4 values that should work too imo.
     */

    /*s_internalFormats[1] = "LUMINANCE",
    s_internalFormats[2] = "LUMINANCE_ALPHA";
    s_internalFormats[3] = "RGB";
    s_internalFormats[4] = "RGBA";*/

    static QSet<int> s_set;
    if (!s_set.isEmpty()) return s_set;

    s_set << GL_RED << GL_RG << GL_RGB << GL_RGBA
    << 3 << 4; // RGB and RGBA

    s_set << GL_R11F_G11F_B10F << GL_R16 << GL_R16F << GL_R16I << GL_R16UI << GL_R16_SNORM
    << GL_R32F << GL_R32I << GL_R32UI << GL_R3_G3_B2 << GL_R8 << GL_R8I << GL_R8UI
    << GL_R8_SNORM << GL_RG16 << GL_RG16F << GL_RG16I << GL_RG16UI << GL_RG16_SNORM
    << GL_RG32F << GL_RG32I << GL_RG32UI << GL_RG8 << GL_RG8I << GL_RG8UI
    << GL_RG8_SNORM << GL_RGB10 << GL_RGB10_A2 << GL_RGB10_A2UI << GL_RGB12
    << GL_RGB16 << GL_RGB16F << GL_RGB16I << GL_RGB16UI << GL_RGB16_SNORM
    << GL_RGB32F << GL_RGB32I << GL_RGB32UI << GL_RGB4 << GL_RGB5 << GL_RGB5_A1
    << GL_RGB8 << GL_RGB8I << GL_RGB8UI << GL_RGB8_SNORM << GL_RGB9_E5 << GL_RGBA12
    << GL_RGBA16 << GL_RGBA16F << GL_RGBA16I << GL_RGBA16UI << GL_RGBA16_SNORM
    << GL_RGBA2 << GL_RGBA32F << GL_RGBA32I << GL_RGBA32UI << GL_RGBA4 << GL_RGBA8
    << GL_RGBA8I << GL_RGBA8UI << GL_RGBA8_SNORM << GL_SRGB8 << GL_SRGB8_ALPHA8;

    return s_set;
  }

  const QSet<QString>& Texture::colorRenderableInternalFormatsStr()
  {
    static QSet<QString> s_set;
    if (!s_set.isEmpty()) return s_set;
    foreach (int i, colorRenderableInternalFormats()) {
      assert(s_internalFormats.contains(i));
      s_set << s_internalFormats.value(i);
    }
    return s_set;
  }

  const QSet<int>& Texture::depthRenderableInternalFormats()
  {
    /**
     * OpenGL specification 4.1 Core Profile:
     *
     * An internal format is depth-renderable if it is DEPTH_COMPONENT or one of
     * the formats from table 3.13 whose base internal format is DEPTH_COMPONENT
     * or DEPTH_STENCIL. No other formats are depth-renderable.
     */

    static QSet<int> s_set;
    if (!s_set.isEmpty()) return s_set;

    /// @todo should DEPTH_STENCIL also be in this list?

    s_set << GL_DEPTH_COMPONENT << GL_DEPTH_COMPONENT16 << GL_DEPTH_COMPONENT24
    << GL_DEPTH_COMPONENT32 << GL_DEPTH_COMPONENT32F << GL_DEPTH24_STENCIL8
    << GL_DEPTH32F_STENCIL8;

    return s_set;
  }

  const QSet<QString>& Texture::depthRenderableInternalFormatsStr()
  {
    static QSet<QString> s_set;
    if (!s_set.isEmpty()) return s_set;
    foreach (int i, depthRenderableInternalFormats()) {
      assert(s_internalFormats.contains(i));
      s_set << s_internalFormats.value(i);
    }
    return s_set;
  }

  const QSet<int>& Texture::stencilRenderableInternalFormats()
  {
    /**
     * OpenGL specification 4.1 Core Profile:
     *
     * An internal format is stencil-renderable if it is STENCIL_INDEX or
     * DEPTH_STENCIL, if it is one of the STENCIL_INDEX formats from table 4.10,
     * or if it is one of the formats from table 3.13 whose base internal
     * format is DEPTH_STENCIL. No other formats are stencil-renderable.
     */

    static QSet<int> s_set;
    if (!s_set.isEmpty()) return s_set;

    s_set << GL_STENCIL << GL_DEPTH_STENCIL
    << GL_STENCIL_INDEX1 << GL_STENCIL_INDEX4 << GL_STENCIL_INDEX8
    << GL_STENCIL_INDEX16
    << GL_DEPTH24_STENCIL8 << GL_DEPTH32F_STENCIL8;

    return s_set;
  }

  const QSet<QString>& Texture::stencilRenderableInternalFormatsStr()
  {
    static QSet<QString> s_set;
    if (!s_set.isEmpty()) return s_set;
    foreach (int i, stencilRenderableInternalFormats()) {
      assert(s_internalFormats.contains(i));
      s_set << s_internalFormats.value(i);
    }
    return s_set;
  }

  void Texture::dataUpdated()
  {
    TextureChangeManager::changed(this, true);
  }

  void Texture::attributeChanged()
  {
    emit changed(std::static_pointer_cast<Texture>(shared_from_this()));
  }

  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  TextureFile::TextureFile(const QString& name)
    : Texture(name)
  {}

  TextureFile::TextureFile(const TextureFile& t)
    : Texture(t),
      FileResource(t)
  {}

  TexturePtr TextureFile::clone() const
  {
    return TexturePtr(new TextureFile(*this));
  }

  QVariantMap TextureFile::toMap() const
  {
    QVariantMap map = Texture::toMap();
    if (!rawFilename().isEmpty()) map["file"] = rawFilename();
    return map;
  }

  void TextureFile::load(QVariantMap map)
  {
    Texture::load(map);
    if (map.contains("file")) setFilename(map["file"].toString());
  }

  QIcon TextureFile::icon() const
  {
    return QIcon();
  }

  void TextureFile::bind(int texture)
  {
    bool reload = m_loadedFile != filename();
    if (reload && m_texture_id > 0) {
      QGLContext* cx = const_cast<QGLContext*>(QGLContext::currentContext());
      cx->deleteTexture(m_texture_id);
      m_texture_id = 0;
      m_width = m_height = 0;
      if (!filename().isEmpty()) {
        QImage image(filename());
        if (image.isNull()) {
          Log::error("Failed to load image: %s (%s)", filename().toUtf8().data(), rawFilename().toUtf8().data());
        } else {
          m_texture_id = cx->bindTexture(image, GL_TEXTURE_2D, m_internalFormat,
                                 QGLContext::InvertedYBindOption | QGLContext::MipmapBindOption);
          m_width = image.width();
          m_height = image.height();
          GLint internalFormat = 0;
          glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &internalFormat);
          m_internalFormat = internalFormat;
          applyParams();
          Log::info("Image %s, #%d %dx%d", filename().toUtf8().data(), m_texture_id, m_width, m_height);
        }
      }
      m_loadedFile = filename();
    }
    glRun(glEnable(GL_TEXTURE_2D));
    Texture::bind(texture);
  }

  void TextureFile::setInternalFormat(int format)
  {
    Texture::setInternalFormat(format);
    m_loadedFile = "";
  }

  void TextureFile::filenameChanged()
  {
    attributeChanged();
  }

} // namespace Shaderkit
