/**
 * Copyright 2010 Riku Palomäki.
 * This file is part of GLSL Lab.
 *
 * GLSL Lab is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GLSL Lab is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GLSL Lab.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "shader/program.hpp"
#include "shader/shader.hpp"
#include "properties.hpp"

GLProgram::GLProgram(const QString& name)
    : m_name(name), m_prog(0), m_compiled(false) {
  connect(this, SIGNAL(linked(ProgramPtr)),
          &Properties::instance(), SLOT(update(ProgramPtr)));
}

GLProgram::~GLProgram() {
  if (m_prog) glDeleteProgram(m_prog);
}

void GLProgram::bind() {
  if (!m_compiled) {
    if (!m_prog) {
      glCheck("GLProgram::bind");
      m_prog = glRun2(glCreateProgram());
    }

    for (Shaders::iterator it = m_shaders.begin(); it != m_shaders.end(); ++it) {
      ShaderError::List errors;
      Shader::CompileStatus status = (*it)->compile(errors);
      if (status != Shader::NONE) emit shaderCompiled(*it, errors);
      if (status == Shader::OK || status == Shader::WARNINGS)
        glAttachShader(m_prog, (*it)->id());
    }

    m_compiled = true;
    /// @todo should we link only when there was a successful compiling?
    link();
  }
  if (isLinked())
    glRun(glUseProgram(m_prog));
}

void GLProgram::unbind() {
  glCheck("GLProgram::unbind");
  glRun(glUseProgram(0));
}

void GLProgram::setUniform(UniformVar::List list, bool relocate) {
  for (UniformVar::List::iterator it = list.begin(); it != list.end(); ++it) {
    it->set(shared_from_this(), relocate);
  }
}

ShaderPtr GLProgram::addShader(const QString& filename, Shader::Type type) {
  ShaderPtr shader(new Shader(shared_from_this(), type));
  shader->loadFile(filename);
  m_shaders.insert(shader);

  return shader;
}

void GLProgram::link() {
  glCheck("GLProgram::link");
  GLint prog = 0;
  glRun(glGetIntegerv(GL_CURRENT_PROGRAM, &prog));
  if (isLinked()) {
    glRun(glUseProgram(m_prog)); /// @todo Do we need this?
    m_uniformList = getUniformList();
  }

  glRun(glLinkProgram(m_prog));
  GLint ok = 0;
  glRun(glGetProgramiv(m_prog, GL_LINK_STATUS, &ok));

  GLint len = 0;
  glRun(glGetProgramiv(m_prog, GL_INFO_LOG_LENGTH, &len));
  // len may include the zero byte
  if (len > 1) {
    GLchar log[len];
    /// @todo generate shadererrors
    glRun(glGetProgramInfoLog(m_prog, len, &len, log));
    //qDebug() << log;
  }

  if (ok) {
    glRun(glUseProgram(m_prog)); /// @todo Do we need this?
    setUniform(m_uniformList);
    emit linked(shared_from_this());
  }

  glRun(glUseProgram(prog));
}

UniformVar::List GLProgram::getUniformList() {
  glCheck("GLProgram::getUniformList");
  UniformVar::List list;

  GLint num = 0, buffer_size = 0;
  glRun(glGetProgramiv(m_prog, GL_ACTIVE_UNIFORMS, &num));
  glRun(glGetProgramiv(m_prog, GL_ACTIVE_UNIFORM_MAX_LENGTH, &buffer_size));

  for (GLint i = 0; i < num; i++) {
    GLchar name[buffer_size];
    GLsizei length;
    GLint size;
    GLenum type;
    glRun(glGetActiveUniform(m_prog, i, buffer_size,
                             &length, &size, &type, name));

    list.push_back(UniformVar(shared_from_this(), name, type));
  }
  return list;
}

bool GLProgram::isLinked() {
  glCheck("GLProgram::isLinked");
  if (!m_prog) return false;
  GLint b = 0;
  glRun(glGetProgramiv(m_prog, GL_LINK_STATUS, &b));
  return b;
}

GLuint GLProgram::id() const {
  return m_prog;
}
