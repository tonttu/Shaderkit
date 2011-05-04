/**
 * Copyright 2010,2011 Riku Palomäki.
 * This file is part of GLSL Lab.
 *
 * GLSL Lab is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License Version 3 as
 * published by the Free Software Foundation.
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
#include "shader/compiler_output_parser.hpp"
#include "app/properties.hpp"
#include "state.hpp"

#include <cassert>

namespace {
  ShaderManager* s_shader_manager = 0;
}

ShaderManager::ShaderManager() {
  assert(!s_shader_manager);
  s_shader_manager = this;
}

ShaderManager::~ShaderManager() {
  assert(s_shader_manager == this);
  s_shader_manager = 0;
}

ShaderManager& ShaderManager::instance() {
  assert(s_shader_manager);
  return *s_shader_manager;
}



GLProgram::GLProgram(const QString& name)
    : m_name(name), m_prog(0), m_compiled(false) {
}

GLProgram::~GLProgram() {
  if (m_prog) glDeleteProgram(m_prog);
}

bool GLProgram::bind(State* state) {
  if (!m_compiled) {
    bool relink = !isLinked();
    if (!m_prog) {
      glCheck("GLProgram::bind");
      m_prog = glRun2(glCreateProgram());
    }

    if (!m_prog)
      return false;

    for (Shaders::iterator it = m_shaders.begin(); it != m_shaders.end(); ++it) {
      ShaderErrorList errors(state ? state->material() : MaterialPtr(), name(), (*it)->res());
      Shader::CompileStatus status = (*it)->compile(errors);
      if (status != Shader::NONE) emit ShaderManager::instance().compiled(errors);
      if (status == Shader::OK || status == Shader::WARNINGS) {
        /// @todo This should not be re-attached
        glRun(glAttachShader(m_prog, (*it)->id()));
        relink = true;
      }
    }

    m_compiled = true;
    if (relink)
      link(state);
  }
  if (isLinked()) {
    glRun(glUseProgram(m_prog));
    return true;
  } else
    return false;
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
  m_shaders << shader;

  return shader;
}

void GLProgram::link(State* state) {
  glCheck("GLProgram::link");
  GLint prog = 0;
  glRun(glGetIntegerv(GL_CURRENT_PROGRAM, &prog));
  if (isLinked()) {
    glRun(glUseProgram(m_prog)); /// @todo Do we need this?
    m_uniformList = getUniformList();
  }

  ShaderErrorList errors(state ? state->material() : MaterialPtr(), name());

  glRun(glLinkProgram(m_prog));
  GLint ok = 0;
  glRun(glGetProgramiv(m_prog, GL_LINK_STATUS, &ok));

  GLint len = 0;
  glRun(glGetProgramiv(m_prog, GL_INFO_LOG_LENGTH, &len));
  // len may include the zero byte
  if (len > 1) {
    std::vector<GLchar> log(len);
    GLsizei size = len;
    glRun(glGetProgramInfoLog(m_prog, size, &size, &log[0]));
    ShaderCompilerOutputParser parser(QString::fromUtf8(&log[0], size));
    parser.parse(errors);
  }

  if (ok) {
    glRun(glUseProgram(m_prog)); /// @todo Do we need this?
    setUniform(m_uniformList);
  }
  emit ShaderManager::instance().linked(errors);

  glRun(glUseProgram(prog));
}

UniformVar::List GLProgram::getUniformList() {
  glCheck("GLProgram::getUniformList");
  UniformVar::List list;

  GLint num = 0, buffer_size = 0;
  glRun(glGetProgramiv(m_prog, GL_ACTIVE_UNIFORMS, &num));
  glRun(glGetProgramiv(m_prog, GL_ACTIVE_UNIFORM_MAX_LENGTH, &buffer_size));

  std::vector<GLchar> name(buffer_size);
  for (GLint i = 0; i < num; i++) {
    GLsizei length;
    GLint size;
    GLenum type;
    glRun(glGetActiveUniform(m_prog, i, buffer_size,
                             &length, &size, &type, &name[0]));

    // For now skip build-in uniforms, since those can't be changed the same way as others.
    /// @todo handle these magical variables somehow better
    QSet<QString> special; special << "time" << "far" << "near";
    if (strncmp(&name[0], "gl_", 3) != 0 && !special.contains(QString::fromAscii(&name[0])))
      list.push_back(UniformVar(shared_from_this(), &name[0], type));
  }
  return list;
}

bool GLProgram::isLinked() {
  glCheck("GLProgram::isLinked");
  if (!m_prog) return false;

  /// for example i915_program_error: Exceeded max instructions etc can be
  /// checked with this
  if (!glRun2(glIsProgram(m_prog))) return false;

  GLint b = 0;
  glRun(glGetProgramiv(m_prog, GL_LINK_STATUS, &b));
  return b;
}

GLuint GLProgram::id() const {
  return m_prog;
}

QVariantMap GLProgram::save(QVariantMap& map, QString root, bool pack) const {
  assert(!pack && "not implemented");

  QMap<Shader::Type, QStringList> shaders;

  /// @todo shouldn't use QDir here, res is already relative
  QDir rootd(root);
  foreach (ShaderPtr s, m_shaders)
    shaders[s->type()] << rootd.relativeFilePath(s->res());

  if (shaders.contains(Shader::Geometry))
    map["geometry"] = shaders[Shader::Geometry];
  if (shaders.contains(Shader::Vertex))
    map["vertex"] = shaders[Shader::Vertex];
  if (shaders.contains(Shader::Fragment))
    map["fragment"] = shaders[Shader::Fragment];

  return map;
}
