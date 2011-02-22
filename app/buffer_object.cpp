#include "buffer_object.hpp"
#include "state.hpp"

BufferObject::BufferObject() : m_id(0), m_target(0), m_cache_size(0) {
}

BufferObject::BufferObject(const BufferObject& obj)
  : m_id(0), m_target(obj.m_target), m_cache_size(0) {
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
                               std::vector<float>& data) {
  bind(state, GL_ARRAY_BUFFER, data);
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

  if (len > 0 && m_cache_size != len) {
    glRun(glBufferData(target, len, data, GL_STATIC_DRAW));
    m_cache_size = len;
  }
}

void BufferObject::bind(/*State& state, */ GLenum target, GLenum hint, size_t len) {
  State state(0);
  bind(state, target);
  if (len > 0 && m_cache_size != len) {
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
  State state(0);
  bind(state, m_target);
  return reinterpret_cast<const float*>(glRun2(glMapBuffer(m_target, GL_READ_ONLY)));
}

void BufferObject::unmap() {
  glUnmapBuffer(m_target);
}
