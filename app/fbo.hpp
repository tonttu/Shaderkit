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

#ifndef FBO_HPP
#define FBO_HPP

#include "forward.hpp"
#include "scene_object.hpp"

#include <QMap>
#include <QSet>

class FBOImage : public SceneObject {
public:
  FBOImage(QString name);
  virtual ~FBOImage() {}
  virtual void setup(unsigned int fbo, int width, int height) = 0;
  /// type is GL_DEPTH_ATTACHMENT etc..
  void setType(int type) { m_type = type; }
  /// Role, like "diffuse", "normalmap", "lightmap" etc
  void setRole(QString role) { m_role = role; }

  QString role() const { return m_role; }

  int width() const { return m_width; }
  int height() const { return m_height; }

  /// Is this a "texture", "renderbuffer" or what
  virtual QString imageClass() const = 0;

  virtual QVariantMap save() const;
  virtual void load(QVariantMap);

  unsigned int id() const { return m_id; }

  virtual void dataUpdated();

protected:
  QString m_role;

  unsigned int m_id;
  int m_type, m_active_type;
  int m_width, m_height;
  QSet<unsigned int> m_fbos;
};

class RenderBuffer : public FBOImage {
public:
  RenderBuffer(QString name = "");
  virtual ~RenderBuffer();

  void setup(unsigned int fbo, int width, int height);

  QString imageClass() const { return "renderbuffer"; }

private:
  void bind();
  void unbind();
};

class FrameBufferObject {
public:
  FrameBufferObject();
  virtual ~FrameBufferObject();

  void resize(int width, int height);

  void set(int type, FBOImagePtr buffer);
  int width() const { return m_width; }
  int height() const { return m_height; }

  void bind();
  void unbind();

  static int colorAttachments();

private:
  typedef QMap<int, FBOImagePtr> Buffers;
  Buffers m_buffers;
  unsigned int m_id;
  int m_width, m_height;
};

#endif // FBO_HPP
