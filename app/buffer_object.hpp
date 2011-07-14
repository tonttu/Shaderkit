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

#ifndef BUFFER_OBJECT_HPP
#define BUFFER_OBJECT_HPP

#include "opengl.hpp"
#include "forward.hpp"

class BufferObject {
public:
  /**
   * ARRAY_BUFFER               Vertex attributes
   * COPY_READ_BUFFER           Buffer copy source (for copying between buffers)
   * COPY_WRITE_BUFFER          Buffer copy destination
   * ELEMENT_ARRAY_BUFFER       Vertex array indices (DrawElements)
   * PIXEL_PACK_BUFFER          ReadPixels(), reading data from frame buffer
   * PIXEL_UNPACK_BUFFER        TexImage-functions, buffer object -> texture
   * TEXTURE_BUFFER             Buffer texture data backend
   * TRANSFORM_FEEDBACK_BUFFER  Transform feedback target buffer
   * UNIFORM_BUFFER             Values of uniforms in named uniform blocks read their values from here
   */

  /**
   * STREAM_DRAW The data store contents will be specified once by the application,
   *     and used at most a few times as the source for GL drawing and image speci-
   *     fication commands.
   * STREAM_READ The data store contents will be specified once by reading data from
   *     the GL, and queried at most a few times by the application.
   * STREAM_COPY The data store contents will be specified once by reading data from
   *     the GL, and used at most a few times as the source for GL drawing and image
   *     specification commands.
   * STATIC_DRAW The data store contents will be specified once by the application,
   *     and used many times as the source for GL drawing and image specification
   *     commands.
   * STATIC_READ The data store contents will be specified once by reading data from
   *     the GL, and queried many times by the application.
   * STATIC_COPY The data store contents will be specified once by reading data from
   *     the GL, and used many times as the source for GL drawing and image spec-
   *     ification commands.
   * DYNAMIC_DRAW The data store contents will be respecified repeatedly by the ap-
   *     plication, and used many times as the source for GL drawing and image
   *     specification commands.
   * DYNAMIC_READ The data store contents will be respecified repeatedly by reading
   *     data from the GL, and queried many times by the application.
   * DYNAMIC_COPY The data store contents will be respecified repeatedly by reading
   *     data from the GL, and used many times as the source for GL drawing and
   *     image specification commands.
   */

  BufferObject();
  BufferObject(const BufferObject& obj);
  virtual ~BufferObject();

  BufferObject& operator=(const BufferObject& obj);

  void enableArray(State& state, GLenum cap, int components, const void* data, size_t len);
  void enableArray(State& state, GLenum cap, int components);

  void bind(State& state, GLenum target, const void* data, size_t len);
  void bind(State& state, GLenum target);

  template <typename T>
  void bind(State& state, GLenum target, const std::vector<T>& data) {
    bind(state, target, &data.front(), data.size()*sizeof(data.front()));
  }

  template <typename T>
  void enableArray(State& state, GLenum cap, int components, const std::vector<T>& data) {
    enableArray(state, cap, components, &data.front(), data.size()*sizeof(data.front()));
  }

  template <typename T>
  void enableArray(State& state, GLenum cap, int components, size_t points, const T* data) {
    enableArray(state, cap, components, data, components * points * sizeof(T));
  }

  /// @todo use state
  void bind(/*State& state, */ GLenum target, GLenum hint, size_t size);
  void unbind();

  void setCache(bool v) { m_use_cache = v; }

  size_t size() const { return m_cache_size; }

  const float* map();
  float* mapRW();
  void unmap();

private:
  unsigned int m_id;
  GLenum m_target;
  size_t m_cache_size;
  bool m_use_cache;
};

#endif // BUFFER_OBJECT_HPP
