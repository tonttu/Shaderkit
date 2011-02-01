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

  BufferObject();
  virtual ~BufferObject();

  void enableArray(State& state, GLenum cap, int components, std::vector<float>& data);

  void bind(State& state, GLenum target, const void* data, size_t len);

  template <typename T>
  void bind(State& state, GLenum target, const std::vector<T>& data) {
    bind(state, target, &data.front(), data.size()*sizeof(data.front()));
  }

private:
  unsigned int m_id;
  size_t m_cache_size;
};

#endif // BUFFER_OBJECT_HPP
