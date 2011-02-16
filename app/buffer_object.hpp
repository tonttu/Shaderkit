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
  virtual ~BufferObject();

  void enableArray(State& state, GLenum cap, int components, std::vector<float>& data);
  void enableArray(State& state, GLenum cap, int components);

  void bind(State& state, GLenum target, const void* data, size_t len);
  void bind(State& state, GLenum target);

  template <typename T>
  void bind(State& state, GLenum target, const std::vector<T>& data) {
    bind(state, target, &data.front(), data.size()*sizeof(data.front()));
  }

  /// @todo use state
  void bind(/*State& state, */ GLenum target, GLenum hint, size_t size);
  void unbind();

  size_t size() const { return m_cache_size; }

  const float* map();
  void unmap();

private:
  unsigned int m_id;
  GLenum m_target;
  size_t m_cache_size;
};

#endif // BUFFER_OBJECT_HPP
