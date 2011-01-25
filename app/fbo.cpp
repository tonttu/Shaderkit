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

#include "fbo.hpp"
#include "opengl.hpp"

#include <cassert>

FBOImage::FBOImage(QString name) : m_name(name),
  m_id(0), m_type(0), m_active_type(0), m_width(0), m_height(0) {}

RenderBuffer::RenderBuffer(QString name) : FBOImage(name) {}

RenderBuffer::~RenderBuffer() {
  if (m_id)
    glDeleteRenderbuffers(1, &m_id);
}

void RenderBuffer::bind() {
  glRun(glBindRenderbuffer(GL_RENDERBUFFER, m_id));
}

void RenderBuffer::unbind() {
  glRun(glBindRenderbuffer(GL_RENDERBUFFER, 0));
}

void RenderBuffer::setup(unsigned int fbo, int width, int height) {
  if (m_id == 0) glRun(glGenRenderbuffers(1, &m_id));

  bool type_changed = m_type != m_active_type,
       size_changed = m_width != width || m_height != height,
       fbo_changed = !m_fbos.contains(fbo);

  if (type_changed || size_changed) {
    int format = m_type == GL_DEPTH_ATTACHMENT ? GL_DEPTH_COMPONENT24 :
                 m_type == GL_STENCIL_ATTACHMENT ? GL_DEPTH24_STENCIL8 : /** @todo is this right? */
                 GL_RGBA;
    bind();
    glRun(glRenderbufferStorageMultisample(GL_RENDERBUFFER, 0 /* samples */,
                                           format, width, height));
    unbind();
  }

  /// @todo Should we run this if only size was changed?
  if (type_changed || size_changed || fbo_changed)
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, m_type, GL_RENDERBUFFER, m_id);

  m_width = width;
  m_height = height;
  m_active_type = m_type;
  if (fbo_changed) m_fbos.insert(fbo);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FrameBufferObject::FrameBufferObject() : m_id(0), m_width(0), m_height(0) {
}

FrameBufferObject::~FrameBufferObject() {
  if (m_id)
    glRun(glDeleteFramebuffers(1, &m_id));
}

void FrameBufferObject::resize(int width, int heigth) {
  m_width = width;
  m_height = heigth;
}

void FrameBufferObject::set(int type, FBOImagePtr buffer) {
  buffer->setType(type);
  m_buffers[type] = buffer;
}

void FrameBufferObject::bind() {
  if (m_id == 0)
    glRun(glGenFramebuffers(1, &m_id));

  glRun(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_id));

  int max_attachments = colorAttachments();
  GLenum* buffers = new GLenum[max_attachments];
  for (int i = 0; i < max_attachments; ++i)
    buffers[i] = m_buffers.contains(GL_COLOR_ATTACHMENT0 + i)
        ? GL_COLOR_ATTACHMENT0 + i : GL_NONE;
  glRun(glDrawBuffers(max_attachments, buffers));
  delete[] buffers;

  for (Buffers::iterator it = m_buffers.begin(); it != m_buffers.end(); ++it)
    (*it)->setup(m_id, m_width, m_height);

  GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER_EXT);
  assert(status == GL_FRAMEBUFFER_COMPLETE);
}

void FrameBufferObject::unbind() {
  glRun(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
}

int FrameBufferObject::colorAttachments() {
  static GLint m_value = -1;
  if (m_value <= 0) {
    glRun(glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &m_value));
  }
  return m_value;
}
