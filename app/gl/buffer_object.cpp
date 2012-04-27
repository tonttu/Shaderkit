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

#include "gl/buffer_object.hpp"
#include "gl/state.hpp"
#include "gl/program.hpp"

#include "core/scene.hpp"
#include "core/material.hpp"

namespace Shaderkit
{

  BufferObject::BufferObject() : m_id(0), m_target(0), m_cache_size(0), m_use_cache(true)
  {
  }

  BufferObject::BufferObject(const BufferObject& obj)
    : m_id(0), m_target(obj.m_target), m_cache_size(0), m_use_cache(true)
  {
  }

  BufferObject::~BufferObject()
  {
    if (m_id) {
      glRun(glDeleteBuffers(1, &m_id));
      m_id = 0;
    }
  }

  BufferObject& BufferObject::operator=(const BufferObject& obj)
  {
    m_id = 0;
    m_cache_size = 0;
    m_use_cache = obj.m_use_cache;
    m_target = obj.m_target;
    return *this;
  }

  void BufferObject::enableArray(State& state, GLenum cap, int components)
  {
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
                                 const void* data, size_t len)
  {
    bind(state, GL_ARRAY_BUFFER, data, len);
    enableArray(state, cap, components);
  }

  void BufferObject::bind(State& state, GLenum target)
  {
    if (m_id == 0) glRun(glGenBuffers(1, &m_id));
    m_target = target;
    /// @todo handle the state correct, so no need to re-bind this
    if (m_target == GL_TRANSFORM_FEEDBACK_BUFFER || m_target == GL_UNIFORM_BUFFER)
      glRun(glBindBufferBase(m_target, 0, m_id));
    else
      glRun(glBindBuffer(m_target, m_id));
  }

  void BufferObject::bind(State& state, GLenum target, const void* data, size_t len)
  {
    bind(state, target);

    if (len > 0 && (!m_use_cache || m_cache_size != len)) {
      glRun(glBufferData(target, len, data, GL_STATIC_DRAW));
      m_cache_size = len;
    }
  }

  void BufferObject::bind(/*State& state, */ GLenum target, GLenum hint, size_t len)
  {
    /// @todo fix this
    Scene s;
    State state(s, 0, 0);
    bind(state, target);
    if (len > 0 && (!m_use_cache || m_cache_size != len)) {
      glRun(glBufferData(target, len, 0, hint));
      m_cache_size = len;
    }
  }

  void BufferObject::unbind(/*State& state, */)
  {
    if (m_target == GL_TRANSFORM_FEEDBACK_BUFFER || m_target == GL_UNIFORM_BUFFER)
      glRun(glBindBufferBase(m_target, 0, 0));
    else
      glRun(glBindBuffer(m_target, 0));
  }

  const float* BufferObject::map()
  {
    /// @todo fix
    Scene s;
    State state(s, 0, 0);
    bind(state, m_target);
    return reinterpret_cast<const float*>(glRun2(glMapBuffer(m_target, GL_READ_ONLY)));
  }

  float* BufferObject::mapRW()
  {
    Scene s;
    State state(s, 0, 0);
    bind(state, m_target);
    return reinterpret_cast<float*>(glRun2(glMapBuffer(m_target, GL_READ_WRITE)));
  }

  void BufferObject::unmap()
  {
    glUnmapBuffer(m_target);
  }



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


  BufferObject2::BufferObject2(GLenum target, GLenum usage)
    : m_id(0),
      m_target(target),
      m_usage(usage),
      m_stride(0),
      m_sizeBytes(0),
      m_bind_stack(0) {}

  BufferObject2::BufferObject2(BufferObject2&& bo)
  {
    *this = bo;
    bo.m_id = 0;
    bo.m_active_vertex_attribs.clear();
    bo.m_bind_stack = 0;
  }

  BufferObject2& BufferObject2::operator=(BufferObject2&& bo)
  {
    *this = bo;
    bo.m_id = 0;
    bo.m_active_vertex_attribs.clear();
    bo.m_bind_stack = 0;
    return *this;
  }

  BufferObject2::~BufferObject2()
  {
    assert(m_bind_stack == 0);
    if (m_id)
      glRun(glDeleteBuffers(1, &m_id));
  }

  void* BufferObject2::map(int offsetBytes, int bytes, int access)
  {
    bind();
    if (m_sizeBytes < offsetBytes + bytes) {
      glRun(glBufferData(m_target, offsetBytes + bytes, 0, m_usage));
      m_sizeBytes = offsetBytes + bytes;
    }
    return glRun2(glMapBufferRange(m_target, offsetBytes, bytes, access));
  }

  void BufferObject2::unmap()
  {
    if (m_id && m_target) {
      glRun(glUnmapBuffer(m_target));
      if (m_bind_stack == 0) unbind();
    }
  }

  void BufferObject2::unbind(const BindHolder& holder)
  {
    --m_bind_stack;
    assert(m_bind_stack >= 0);
    if (m_id && m_target && m_bind_stack == 0) {
      unbind();
    }
    foreach (int attrib, holder.m_active) {
      int v = --m_active_vertex_attribs[attrib];
      assert(v >= 0);
      if (v == 0)
        glRun(glDisableVertexAttribArray(attrib));
    }
  }

  BufferObject2::BindHolder BufferObject2::bind(State& state)
  {
    // * m_attribs will map logical channel (like Vertex0 or UV1) to specific
    //   offset to the actual struct
    // * attr will give the list of logical channels that are asked to bind
    //   and also maps those logical channels to shader attribute locations

    BufferObject2::BindHolder ret(*this);

    if (!state.material() || !state.material()->prog())
      return ret;

    GLProgram& prog = *state.material()->prog();
    for (auto it = state.attr()->begin(), end = state.attr()->end(); it != end; ++it) {
      if (!m_attribs.contains(it.key())) continue;
      const AttribInfo& info = m_attribs[it.key()];
      int loc = prog.attribLocation(it.value());
      glRun(glVertexAttribPointer(loc, info.components, info.type, false,
                                  m_stride, (char*)0 + info.offset));
      glRun(glEnableVertexAttribArray(loc));
      ret.m_active << loc;
      ++m_active_vertex_attribs[loc];
    }

    return ret;
  }

  void BufferObject2::bind(const BindHolder&)
  {
    ++m_bind_stack;
    bind();
  }

  void BufferObject2::bind()
  {
    if (m_id == 0) glRun(glGenBuffers(1, &m_id));
    /// @todo handle the state correct, so no need to re-bind this
    if (m_target == GL_TRANSFORM_FEEDBACK_BUFFER || m_target == GL_UNIFORM_BUFFER)
      glRun(glBindBufferBase(m_target, 0, m_id));
    else
      glRun(glBindBuffer(m_target, m_id));
  }

  void BufferObject2::unbind()
  {
    if (m_target == GL_TRANSFORM_FEEDBACK_BUFFER || m_target == GL_UNIFORM_BUFFER)
      glRun(glBindBufferBase(m_target, 0, 0));
    else
      glRun(glBindBuffer(m_target, 0));
  }

  void BufferObject2::uploadData(const void* data, int offsetBytes, int sizeBytes)
  {
    bind();
    if (m_sizeBytes < offsetBytes + sizeBytes) {
      glRun(glBufferData(m_target, offsetBytes + sizeBytes, 0, m_usage));
      m_sizeBytes = offsetBytes + sizeBytes;
    }

    glRun(glBufferSubData(m_target, offsetBytes, sizeBytes, data));
    unbind();
  }

} // namespace Shaderkit
