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

#include "gl/query_shader.hpp"

#include <cassert>

QueryShader* QueryShader::s_instance = 0;

QueryShader::QueryShader() : m_shader(0), m_prog(0), m_attached(0) {
  assert(!s_instance);
}

QueryShader& QueryShader::instance() {
  if (!s_instance) {
    s_instance = new QueryShader;
  }
  return *s_instance;
}

bool QueryShader::compile(Shader::Type type, QString src) {
  if (!m_shader) {
    /// @todo handle different sets of shaders depending on the type
    m_shader = glRun2(glCreateShader(type));
    if (!m_shader) {
      Log::error("QueryShader: could not create shader object");
      return false;
    }
  }

  if (!m_prog) {
    m_prog = glRun2(glCreateProgram());
    if (!m_prog) {
      Log::error("QueryShader: could not create program object");
      return false;
    }
  }

  QByteArray bytes = src.toAscii();
  const char* data = bytes.data();
  GLint len = bytes.length();
  glRun(glShaderSource(m_shader, 1, &data, &len));
  glRun(glCompileShader(m_shader));

  GLint ok = 0;
  glRun(glGetShaderiv(m_shader, GL_COMPILE_STATUS, &ok));

  if (!ok) {
    glRun(glGetShaderiv(m_shader, GL_INFO_LOG_LENGTH, &len));
    std::vector<GLchar> log(len);
    glRun(glGetShaderInfoLog(m_shader, len, &len, &log[0]));
    Log::error("Compile log: %s", &log[0]);
    return false;
  }

  if (!m_attached) glRun(glAttachShader(m_prog, m_shader));
  m_attached = true;

  return true;
}

bool QueryShader::bind(const char* name) {
  GLint ok = 0;

  glRun(glTransformFeedbackVaryings(m_prog, 1, &name, GL_SEPARATE_ATTRIBS));

  glRun(glLinkProgram(m_prog));
  glRun(glGetProgramiv(m_prog, GL_LINK_STATUS, &ok));

  if (!ok) {
    GLint len = 0;
    glRun(glGetProgramiv(m_prog, GL_INFO_LOG_LENGTH, &len));
    std::vector<GLchar> log(len);
    GLsizei size = len;
    glRun(glGetProgramInfoLog(m_prog, size, &size, &log[0]));
    Log::error("Link log: %s", &log[0]);
    return false;
  }

  glRun(glUseProgram(m_prog));
  m_feedback.begin(GL_POINTS, 1);
  return true;
}

bool QueryShader::unbind(float& out) {
  bool ok = m_feedback.end(&out, 1);
  glRun(glUseProgram(0));
  return ok;
}
