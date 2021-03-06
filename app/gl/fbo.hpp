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

#ifndef FBO_HPP
#define FBO_HPP

#include "forward.hpp"

#include "core/attribute.hpp"

#include "gl/scene_object.hpp"

#include <QMap>
#include <QSet>

namespace Shaderkit
{
  /// @todo Should this and RenderBuffer have (virtual) clone()?
  class FBOImage : public QObject, public SceneObject,
      public std::enable_shared_from_this<FBOImage>
  {
    Q_OBJECT

  public:
    FBOImage(const QString& name);
    virtual ~FBOImage() {}
    virtual void setup(unsigned int fbo, int width, int height) = 0;
    /// type is GL_DEPTH_ATTACHMENT etc..
    void setAttachment(int attachment) { m_attachment = attachment; }
    /// Role, like "diffuse", "normalmap", "lightmap" etc
    void setRole(const QString& role) { m_role = role; }

    const QString& role() const { return m_role.value(); }

    int width() const { return m_width; }
    int height() const { return m_height; }

    /// Is this a "texture", "renderbuffer" or what
    virtual QString imageClass() const = 0;

    virtual QVariantMap toMap() const;
    virtual void load(QVariantMap);

    unsigned int renderBufferId() const { return m_render_buffer_id; }

    virtual void dataUpdated();

    void setFBO(FBOPtr fbo);

    int attachment() const { return m_attachment; }

    virtual void attributeChanged();

  signals:
    void changed(FBOImagePtr);

  protected:
    explicit FBOImage(const FBOImage& f);

  protected:
    Attribute<QString> m_role;

    unsigned int m_render_buffer_id;
    /// This is saved implicitly by RenderPass
    Attribute<int> m_attachment;
    int m_active_attachment;
    int m_width, m_height;
    std::weak_ptr<FrameBufferObject> m_fbo;
    unsigned int m_fbo_num;
  };

  class RenderBuffer : public FBOImage
  {
  public:
    RenderBuffer(const QString& name = "");
    virtual ~RenderBuffer();

    void setup(unsigned int fbo, int width, int height);

    QString imageClass() const { return "renderbuffer"; }

  private:
    void bind();
    void unbind();
  };

  class FrameBufferObject : public std::enable_shared_from_this<FrameBufferObject>
  {
  public:
    /// eg. GL_DEPTH_ATTACHMENT => texture
    typedef QMap<int, FBOImagePtr> Buffers;
    FrameBufferObject();
    virtual ~FrameBufferObject();

    void resize(int width, int height);

    void set(int type, FBOImagePtr buffer);
    void clearBuffers();
    void remove(FBOImagePtr buffer);

    int width() const { return m_width; }
    int height() const { return m_height; }

    void bind();
    void unbind();

    void printDebug();

    static int colorAttachments();

    Buffers buffers() const { return m_buffers; }
    bool isEmpty() const { return m_buffers.isEmpty(); }

  private:
    Buffers m_buffers;
    QSet<int> m_lazyRemove;
    unsigned int m_id;
    int m_width, m_height;
  };

} // namespace Shaderkit

#endif // FBO_HPP
