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

#ifndef SHADER_PROGRAM_HPP
#define SHADER_PROGRAM_HPP

#include "forward.hpp"
#include "shader/uniform.hpp"
#include "shader/error.hpp"

#include <QObject>

#include <boost/enable_shared_from_this.hpp>

#include <set>

/**
 * GLSL Program object.
 *
 * Program has different kind of shaders (see Shader), and it handles all uniform
 * variables that the shaders use. Compiling and linking is managed from here, and
 * all kinds of events related to shaders are dispatched through this object.
 *
 * Like Shader, GLProgram objects can be safely created before any OpenGL context
 * is created; shaders are compiled, underlying program object created and the
 * program linked when the object is bind()ed first time.
 */
class GLProgram : public QObject, public boost::enable_shared_from_this<GLProgram> {
  Q_OBJECT

public:
  typedef std::set<ShaderPtr> Shaders;

  /// Creates an empty program object and connects linked-signal to Properties::update.
  GLProgram(const QString& name);
  virtual ~GLProgram() {}

  /**
   * Binds the program object. On the first run time creates the real shader
   * program object. Also (re)compiles all the shaders and (re)links the program
   * if needed. For every compiled shader the shaderCompiled()-signal is emitted.
   */
  void bind();

  /// Unbinds the program object.
  void unbind();

  /// Sets the compiled status
  void setIsCompiled(bool state) { m_compiled = state; }

  /// Sets a uniform variable, binds the object if needed,
  /// and restores the old state before returning.
  template <typename T>
  void setUniform(GLint location, T t) {
    GLint prog = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &prog);
    if (!m_prog || prog != (GLint)m_prog->programId()) bind();
    m_prog->setUniformValue(location, t);

    // restore the old state
    if (prog != (GLint)m_prog->programId()) glUseProgram(prog);
  }

  /// Restore the uniform state stored in list.
  void setUniform(UniformVar::List list, bool relocate = true);

  /// Creates a new Shader and adds it to this program.
  virtual ShaderPtr addShader(const QString& filename, QGLShader::ShaderTypeBit type);

  /**
   * (Re)Links the program. If the program is currently linked, saves the uniform
   * variable state before relinking. After successful linking, restores the state.
   */
  virtual void link();

  /// Object name, like "Phong"
  const QString& name() const { return m_name; }

  /// Returns the current state of all uniform variables
  UniformVar::List getUniformList();

  /// Returns the actual OpenGL program id, or -1 if there is no program created yet.
  int id() const;

  Shaders shaders() const { return m_shaders; }

signals:
  /// Emitted every time the program is successfully linked
  void linked(ProgramPtr);
  /// Shader was compiled
  void shaderCompiled(ShaderPtr, ShaderError::List);

protected:
  QString m_name;
  QGLShaderProgram* m_prog;

  Shaders m_shaders;

  /// Are all shaders compiled
  bool m_compiled;

  /// The last known uniform values of the shader program.
  /// This is updated every time before we relink anything, and restored after successful linking.
  UniformVar::List m_uniformList;
};

#endif
