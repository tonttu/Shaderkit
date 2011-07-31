/**
 * Copyright 2010,2011 Riku Palomäki.
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

#include "buffer_object.hpp"
#include "state.hpp"

BufferObject::BufferObject() : m_id(0), m_target(0), m_cache_size(0), m_use_cache(true) {
}

BufferObject::BufferObject(const BufferObject& obj)
  : m_id(0), m_target(obj.m_target), m_cache_size(0), m_use_cache(true) {
}

BufferObject::~BufferObject() {
  if (m_id) {
    glRun(glDeleteBuffers(1, &m_id));
    m_id = 0;
  }
}

BufferObject& BufferObject::operator=(const BufferObject& obj) {
  m_id = 0;
  m_cache_size = 0;
  m_use_cache = obj.m_use_cache;
  m_target = obj.m_target;
  return *this;
}

void BufferObject::enableArray(State& state, GLenum cap, int components) {
  bind(state, GL_ARRAY_BUFFER);
  glRun(glEnableClientState(cap));
  if (cap == GL_COLOR_ARRAY) {
    glRun(glColorPointer(components, GL_FLOAT, 0, 0));
  } else if (cap == GL_NORMAL_ARRAY) {
    glRun(glNormalPointer(GL_FLOAT, 0, 0));
  } else if (cap == GL_TEXTURE_COORD_ARRAY) {
    glRun(glTexCoordPointer(components, GL_FLOAT, 0, 0));
  } else if (cap == GL_VERTEX_ARRAY) {
    glRun(glVertexPointer(components, GL_FLOAT, 0, 0));
  } else {
    Log::error("Unsupported client state: %d", cap);
  }
}

void BufferObject::enableArray(State& state, GLenum cap, int components,
                               const void* data, size_t len) {
  bind(state, GL_ARRAY_BUFFER, data, len);
  enableArray(state, cap, components);
}

void BufferObject::bind(State& state, GLenum target) {
  if (m_id == 0) glRun(glGenBuffers(1, &m_id));
  m_target = target;
  /// @todo handle the state correct, so no need to re-bind this
  if (m_target == GL_TRANSFORM_FEEDBACK_BUFFER || m_target == GL_UNIFORM_BUFFER)
    glRun(glBindBufferBase(m_target, 0, m_id));
  else
    glRun(glBindBuffer(m_target, m_id));
}

void BufferObject::bind(State& state, GLenum target, const void* data, size_t len) {
  bind(state, target);

  if (len > 0 && (!m_use_cache || m_cache_size != len)) {
    glRun(glBufferData(target, len, data, GL_STATIC_DRAW));
    m_cache_size = len;
  }
}

void BufferObject::bind(/*State& state, */ GLenum target, GLenum hint, size_t len) {
  State state(0, 0);
  bind(state, target);
  if (len > 0 && (!m_use_cache || m_cache_size != len)) {
    glRun(glBufferData(target, len, 0, hint));
    m_cache_size = len;
  }
}

void BufferObject::unbind(/*State& state, */) {
  if (m_target == GL_TRANSFORM_FEEDBACK_BUFFER || m_target == GL_UNIFORM_BUFFER)
    glRun(glBindBufferBase(m_target, 0, 0));
  else
    glRun(glBindBuffer(m_target, 0));
}

const float* BufferObject::map() {
  State state(0, 0);
  bind(state, m_target);
  return reinterpret_cast<const float*>(glRun2(glMapBuffer(m_target, GL_READ_ONLY)));
}

float* BufferObject::mapRW() {
  State state(0, 0);
  bind(state, m_target);
  return reinterpret_cast<float*>(glRun2(glMapBuffer(m_target, GL_READ_WRITE)));
}

void BufferObject::unmap() {
  glUnmapBuffer(m_target);
}
