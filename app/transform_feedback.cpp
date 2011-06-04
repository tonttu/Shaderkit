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

#include "transform_feedback.hpp"

#include "opengl.hpp"

#include <cassert>

TransformFeedback::TransformFeedback() : m_query(0), m_id(0) {
}

TransformFeedback::~TransformFeedback() {
  if (m_id) {
    glRun(glDeleteTransformFeedbacks(1, &m_id));
  }
  if (m_query) {
    glDeleteQueries(1, &m_query);
  }
}

bool TransformFeedback::begin() {
  if (glewIsSupported("GL_ARB_transform_feedback2")) {
    if (!m_id) {
      assert(glGenTransformFeedbacks);
      glRun(glGenTransformFeedbacks(1, &m_id));
      if (!m_id) return false;
    }

    glRun(glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_id));
  }

  if (!m_query) {
    glRun(glGenQueries(1, &m_query));
  }

  m_buffer.bind(GL_TRANSFORM_FEEDBACK_BUFFER, GL_DYNAMIC_READ, 1000);
  glRun(glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, m_query));
  glRun(glBeginTransformFeedback(GL_POINTS));
  return true;
}

bool TransformFeedback::end(float& out) {
  bool ok = false;
  glRun(glEndTransformFeedback());
  glRun(glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN));
  unsigned int res = 0;
  glRun(glGetQueryObjectuiv(m_query, GL_QUERY_RESULT, &res));

  const float* data = m_buffer.map();
  if (res == 1 && data) {
    out = data[0];
    ok = true;
  }
  m_buffer.unmap();
  m_buffer.unbind();
  if (glewIsSupported("GL_ARB_transform_feedback2"))
    glRun(glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0));

  return ok;
}
