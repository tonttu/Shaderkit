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

#ifndef BUFFER_OBJECT_HPP
#define BUFFER_OBJECT_HPP

#include "gl/opengl.hpp"
#include "forward.hpp"

#include <cassert>
#include <vector>

#include <QSet>
#include <QMap>

namespace Shaderkit
{

  class BufferObject
  {
  public:
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

  template <GLenum T> struct gl_sizes;
  template <> struct gl_sizes<GL_FLOAT> { enum { Size = 4 }; };

  class VertexAttrib
  {
  public:
    enum Target {
      Vertex0, Vertex1, Vertex2,
      UV0, UV1, UV2, UV3, UV4, UV5, UV6, UV7,
      Normal, Tangent, Bitangent,
      Color0, Color1, Color2, Color3, Color4, Color5, Color6, Color7,
      Index
    };

    QMap<Target, QString>* operator->() { return &m_mapping; }
    const QMap<Target, QString>* operator->() const { return &m_mapping; }

    QMap<Target, QString>& operator*() { return m_mapping; }
    const QMap<Target, QString>& operator*() const { return m_mapping; }

    QString& operator[](Target t) { return m_mapping[t]; }

  private:
    // map from logical channel to one specific shader attribute
    QMap<Target, QString> m_mapping;
  };

/// OpenGL Buffer Object
  class BufferObject2
  {
  public:
    /**
     * Creates a new Buffer Object with given target and usage.
     *
     * Constructor doesn't require an OpenGL context, since it's created lazily
     *
     * @param target ARRAY_BUFFER          Vertex attributes
     *               COPY_READ_BUFFER      Buffer copy source (for copying between buffers)
     *               COPY_WRITE_BUFFER     Buffer copy destination
     *               DRAW_INDIRECT_BUFFER  Indirect command arguments
     *               ELEMENT_ARRAY_BUFFER  Vertex array indices (DrawElements)
     *               PIXEL_PACK_BUFFER     ReadPixels(), reading data from frame buffer
     *               PIXEL_UNPACK_BUFFER   TexImage-functions, buffer object -> texture
     *               TEXTURE_BUFFER        Buffer texture data backend
     *               TRANSFORM_FEEDBACK_BUFFER
     *                                     Transform feedback target buffer
     *               UNIFORM_BUFFER        Values of uniforms in named uniform blocks read
     *                                     their values from here
     *
     * @param usage  STREAM_DRAW   The data store contents will be specified once by the application,
     *                             and used at most a few times as the source for GL drawing and image
     *                             specification commands.
     *               STREAM_READ   The data store contents will be specified once by reading data from
     *                             the GL, and queried at most a few times by the application.
     *               STREAM_COPY   The data store contents will be specified once by reading data from
     *                             the GL, and used at most a few times as the source for GL drawing
     *                             and image specification commands.
     *               STATIC_DRAW   The data store contents will be specified once by the application,
     *                             and used many times as the source for GL drawing and image
     *                             specification commands.
     *               STATIC_READ   The data store contents will be specified once by reading data from
     *                             the GL, and queried many times by the application.
     *               STATIC_COPY   The data store contents will be specified once by reading data from
     *                             the GL, and used many times as the source for GL drawing and image
     *                             specification commands.
     *               DYNAMIC_DRAW  The data store contents will be respecified repeatedly by the
     *                             application, and used many times as the source for GL drawing and
     *                             image specification commands.
     *               DYNAMIC_READ  The data store contents will be respecified repeatedly by reading
     *                             data from the GL, and queried many times by the application.
     *               DYNAMIC_COPY  The data store contents will be respecified repeatedly by reading
     *                             data from the GL, and used many times as the source for GL drawing
     *                             and image specification commands.
     */
    BufferObject2(GLenum target = GL_ARRAY_BUFFER, GLenum usage = GL_STATIC_DRAW);

    BufferObject2(const BufferObject2&) = delete;

    BufferObject2(BufferObject2&&);
    BufferObject2& operator=(BufferObject2&&);

    /// Deletes the buffer, requires a working OpenGL context, unless this is a
    /// null buffer (never used)
    virtual ~BufferObject2();

    std::size_t size() const { return m_sizeBytes; }
    void setUsage(GLenum usage) { m_usage = usage; }

    // BaseType could be generated automatically from ValueType and N, but it's
    // more clear this way
    template <GLenum BaseType, typename ValueType, typename Struct, size_t N>
    void link(ValueType (Struct::*member)[N], VertexAttrib::Target target, std::size_t size = 0) {
      int offset = (char*)&(((Struct*)0)->*member) - (char*)((Struct*)0);
      AttribInfo& attr = m_attribs[target];
      attr.offset = offset;
      attr.components = size == 0 ? N * sizeof(ValueType) / gl_sizes<BaseType>::Size : size;
      attr.type = BaseType;
      assert(m_stride == 0 || m_stride == sizeof(Struct));
      m_stride = sizeof(Struct);
    }

    template <GLenum BaseType, typename ValueType, typename Struct>
    void link(ValueType (Struct::*member), VertexAttrib::Target target, std::size_t size = 0) {
      int offset = (char*)&(((Struct*)0)->*member) - (char*)((Struct*)0);
      AttribInfo& attr = m_attribs[target];
      attr.offset = offset;
      attr.components = size == 0 ? sizeof(ValueType) / gl_sizes<BaseType>::Size : size;
      attr.type = BaseType;
      assert(m_stride == 0 || m_stride == sizeof(Struct));
      m_stride = sizeof(Struct);
    }

    template <GLenum BaseType>
    void set(VertexAttrib::Target target, int components, std::size_t stride = 0, std::size_t offset = 0) {
      AttribInfo& attr = m_attribs[target];
      attr.offset = offset;
      attr.components = components;
      attr.type = BaseType;
      assert(m_stride == 0 || m_stride == stride);
      m_stride = stride;
    }

    template <typename T>
    class Array
    {
    public:
      Array(BufferObject2& bo, void* data, std::size_t size) : m_bo(bo),
        m_data(reinterpret_cast<T*>(data)), m_begin(m_data), m_sizeBytes(size) {
        assert(data);
      }
      ~Array() { if (m_data) m_bo.unmap(); }

      T* operator->() { assert_deref(); return m_data; }
      T& operator[](int offset) { assert_deref(offset); return m_data[offset]; }
      T& operator*() { assert_deref(); return *m_data; }
      Array& operator+=(int offset) { m_data += offset; return *this; }
      Array& operator-=(int offset) { m_data -= offset; return *this; }
      Array& operator++() { ++m_data; return *this; }
      Array& operator--() { --m_data; return *this; }

      Array& operator=(std::size_t offset) { m_data = m_begin + offset; return *this; }

      Array& operator<<(const T& t) { **this = t; return ++*this; }

      int pos() const { return m_data - m_begin; }

      //Array(const Array&& other);
      Array(const Array& other) : m_bo(other.m_bo), m_data(other.m_data),
        m_begin(other.m_begin), m_sizeBytes(other.m_sizeBytes) { assert(false); }
    private:
      //Array(const Array&);

      inline void assert_deref(int d = 0) {
        assert(m_data + d >= m_begin);
        assert((const char*)(m_data + d) < (const char*)m_begin + m_sizeBytes);
      }

      BufferObject2& m_bo;
      T* m_data;
      T* const m_begin;
      std::size_t m_sizeBytes;
    };

    class BindHolder
    {
    public:
      BindHolder(BufferObject2& bo) : m_bo(&bo) { bo.bind(*this); }
      ~BindHolder() { if (m_bo) m_bo->unbind(*this); }

      /// This class is only movable, not copyable
      BindHolder(const BindHolder&) = delete;
      BindHolder& operator=(const BindHolder&) = delete;

      BindHolder(BindHolder&& other) : m_bo(0) {
        std::swap(m_bo, other.m_bo);
        std::swap(m_active, other.m_active);
      }

      BindHolder& operator=(BindHolder&& other) {
        if (m_bo) m_bo->unbind(*this);
        m_bo = other.m_bo;
        m_active = other.m_active;

        other.m_bo = 0;
        other.m_active.clear();
        return *this;
      }

    private:
      friend class BufferObject2;
      BufferObject2* m_bo;
      QSet<int> m_active;
    };

    void bind(const BindHolder& holder);
    void unbind(const BindHolder& holder);
    BindHolder bind(State& state);

    template <typename T>
    Array<const T> mapRead(std::size_t offset, std::size_t size) {
      return Array<const T>(*this, map(offset * sizeof(T), size * sizeof(T), GL_MAP_READ_BIT), (offset+size) * sizeof(T));
    }

    template <typename T>
    Array<T> mapReadWrite(std::size_t offset, std::size_t size) {
      return Array<T>(*this, map(offset * sizeof(T), size * sizeof(T), GL_MAP_READ_BIT | GL_MAP_WRITE_BIT), (offset+size) * sizeof(T));
    }

    template <typename T>
    Array<T> mapWrite(std::size_t offset, std::size_t size) {
      return Array<T>(*this, map(offset * sizeof(T), size * sizeof(T), GL_MAP_WRITE_BIT), (offset+size) * sizeof(T));
    }

    template <typename T>
    void upload(const T* t, std::size_t offsetItems, std::size_t numberOfItems) {
      uploadData(t, offsetItems*sizeof(T), numberOfItems*sizeof(T));
    }

    template <typename T>
    void upload(const std::vector<T>& t, std::size_t offsetItems = 0) {
      upload(t.data(), offsetItems, t.size());
    }

    /// Like upload(), but only uploads data if it's size is different from
    /// what we already have.
    template <typename T>
    void sync(const std::vector<T>& t) {
      if (t.size() * sizeof(T) != m_sizeBytes)
        upload(t.data(), 0, t.size());
    }

  private:
    BufferObject2& operator=(const BufferObject2&) = default;

    /**
     * Maps the Buffer Object to client memory space
     *
     * Binds and creates the object automatically if necessary
     *
     * @param bytes The size of the mapped memory region (in bytes)
     * @param access MAP_READ_BIT / MAP_WRITE_BIT +
     *               MAP_INVALIDATE_RANGE_BIT
     *               MAP_INVALIDATE_BUFFER_BIT
     *               MAP_FLUSH_EXPLICIT_BIT
     *               MAP_UNSYNCHRONIZED_BIT
     */
    void* map(std::size_t offsetBytes, std::size_t bytes, int access);
    void unmap();

    void bind();
    void unbind();
    void uploadData(const void* data, std::size_t offsetBytes, std::size_t sizeBytes);

    /// Buffer object name
    unsigned int m_id;

    /// @see BufferObject()
    GLenum m_target;
    /// @see BufferObject()
    GLenum m_usage;

    struct AttribInfo {
      int offset;
      /// 1, 2, 3 or 4
      int components;
      GLenum type;
    };

    QMap<VertexAttrib::Target, AttribInfo> m_attribs;

    std::size_t m_stride;
    std::size_t m_sizeBytes;

    QMap<int, int> m_active_vertex_attribs;
    int m_bind_stack;
  };

} // namespace Shaderkit

#endif // BUFFER_OBJECT_HPP
