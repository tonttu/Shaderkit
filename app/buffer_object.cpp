#include "buffer_object.hpp"

BufferObject::BufferObject() : m_id(0), m_cache_size(0) {
}

BufferObject::~BufferObject() {
  if (m_id) {
    glRun(glDeleteBuffers(1, &m_id));
    m_id = 0;
  }
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
                               std::vector<float>& data) {
  bind(state, GL_ARRAY_BUFFER, data);
  enableArray(state, cap, components);
}

void BufferObject::bind(State& state, GLenum target) {
  if (m_id == 0) glRun(glGenBuffers(1, &m_id));
  /// @todo handle the state correct, so no need to re-bind this
  glRun(glBindBuffer(target, m_id));
}

void BufferObject::bind(State& state, GLenum target, const void* data, size_t len) {
  bind(state, target);

  if (len > 0 && m_cache_size != len) {
    glRun(glBufferData(target, len, data, GL_STATIC_DRAW));
    m_cache_size = len;
  }
}
