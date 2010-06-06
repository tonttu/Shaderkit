/**
 * Copyright 2010 Riku Palomäki.
 * This file is part of GLSL Lab.
 *
 * GLSL Lab is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
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

#include <QMap>
#include <QSet>

class FBOImage {
public:
  FBOImage(QString name);
  virtual ~FBOImage() {}
  virtual void setup(unsigned int fbo, int width, int height) = 0;
  void setType(int type) { m_type = type; }

  QString name() const { return m_name; }

protected:
  QString m_name;

  unsigned int m_id;
  int m_type, m_active_type;
  int m_width, m_height;
  QSet<unsigned int> m_fbos;
};

class RenderBuffer : public FBOImage {
public:
  RenderBuffer(QString name);
  virtual ~RenderBuffer();

  void setup(unsigned int fbo, int width, int height);

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
