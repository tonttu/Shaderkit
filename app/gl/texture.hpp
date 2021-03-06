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

#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include "forward.hpp"
#include "gl/fbo.hpp"
#include "core/file_resource.hpp"

#include <QVariantMap>

#include <functional>

class QTimer;

namespace Shaderkit
{

/// Signals when OpenGL texture was changed, so for example this isn't emitted
/// on setParam but only on applyParams
  class TextureChangeManager : public QObject
  {
    Q_OBJECT

  public:
    virtual ~TextureChangeManager();

    static void listen(TexturePtr texture, QObject* listener, std::function<void ()> func, bool data = true);
    static void forget(TexturePtr texture, QObject* listener);

    static void changed(Texture* tex, bool data = false);
    static void removed(Texture* tex);

  private slots:
    void listenerDeleted(QObject*);
    void run();

  private:
    struct Listener {
      QObject* obj;
      bool data;
      std::function<void ()> func;
    };

    TextureChangeManager();
    typedef QList<Listener> List;
    QMap<Texture*, List> m_callbacks;
    QSet<Texture*> m_queue, m_queueData;
    QTimer* m_timer;
  };

  class Texture : public FBOImage
  {
    Q_OBJECT

  public:
    enum ParamType { UNKNOWN, ENUM, INT, FLOAT };
    struct Param {
      Param(int v = 0) : is_float(false), i(v) {}
      Param(float v) : is_float(true), f(v) {}
      bool operator==(const Param& p)
      {
        return is_float == p.is_float &&
            ((is_float && f == p.f) ||
             (!is_float && i == p.i));
      }

      bool is_float;
      union {
        float f;
        int i;
      };
    };

    Texture(const QString& name);
    virtual ~Texture();

    void setup(unsigned int fbo, int width, int height);

    virtual void bind(int texture = 0);
    virtual void unbind();

    void setParam(unsigned int pname, int param, bool quiet = false);
    void setParam(unsigned int pname, float param, bool quiet = false);
    bool setParam(QString pname, Param param, bool quiet = false);
    bool setParam(QString pname, QString param, bool quiet = false);

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
    virtual QVariantMap toMap() const;
    virtual void load(QVariantMap map);

    int internalFormat() const { return m_internalFormat; }
    virtual void setInternalFormat(int format);
    void setInternalFormat(QString format);

    QString internalFormatStr() const;

    static const QList<QPair<QString, QStringList>>& internalFormats(bool colorRenderableOnly);

    static const QSet<int>& colorRenderableInternalFormats();
    static const QSet<QString>& colorRenderableInternalFormatsStr();

    static const QSet<int>& depthRenderableInternalFormats();
    static const QSet<QString>& depthRenderableInternalFormatsStr();

    static const QSet<int>& stencilRenderableInternalFormats();
    static const QSet<QString>& stencilRenderableInternalFormatsStr();

    unsigned int textureId() const { return m_texture_id; }

    void dataUpdated();

    virtual void attributeChanged();

  signals:
    void changed(TexturePtr);

  protected:
    explicit Texture(const Texture& t);
    void applyParams();

  protected:
    unsigned int m_texture_id;
    QMap<unsigned int, Param> m_params;
    unsigned int m_bindedTexture;
    /// @todo Should be saved
    Attribute<int> m_internalFormat;
    Attribute<float> m_blend;
    Attribute<int> m_uv;
    bool m_paramsDirty, m_dirty;
  };

  class TextureFile : public Texture, public FileResource
  {
  public:
    TextureFile(const QString& name);
    virtual ~TextureFile() {}

    virtual void bind(int texture = 0);
    virtual void setInternalFormat(int format);

    virtual TexturePtr clone() const;
    virtual QVariantMap toMap() const;
    virtual void load(QVariantMap map);

    QIcon icon() const;

  protected:
    explicit TextureFile(const TextureFile& t);
    virtual void filenameChanged();

  private:
    QString m_loadedFile;
  };

} // namespace Shaderkit

#endif // TEXTURE_HPP
