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
#include "shader/shader.hpp"
#include "shader/error.hpp"
#include "opengl.hpp"

#include <QObject>

#include <boost/enable_shared_from_this.hpp>

#include <set>
#include <iostream>

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
  virtual ~GLProgram();

  /**
   * Binds the program object. On the first run time creates the real shader
   * program object. Also (re)compiles all the shaders and (re)links the program
   * if needed. For every compiled shader the shaderCompiled()-signal is emitted.
   */
  bool bind();

  /// Unbinds the program object.
  void unbind();

  /// Sets the compiled status
  void setIsCompiled(bool state) { m_compiled = state; }

  /// Sets a uniform variable, binds the object if needed,
  /// and restores the active program before returning.
  template <typename T>
  void setUniform(QString name, T t, bool restore = false) {
    glCheck("setUniform");
    GLint prog = 0;
    glRun(glGetIntegerv(GL_CURRENT_PROGRAM, &prog));
    if (!prog != m_prog)
      bind();

    GLint loc = glRun2(glGetUniformLocation(m_prog, name.toAscii().data()));
    if(loc == -1) {
      std::cerr << "Failed to query uniform variable " << name.toStdString() << " location" << std::endl;
    } else {
      setUniform(loc, t);
    }

    // restore the old state
    if (restore && prog != (GLint)m_prog)
      glRun(glUseProgram(prog));
  }

  void setUniform(GLint loc, GLfloat v0) { glRun(glUniform1f(loc, v0)); }
  void setUniform(GLint loc, GLfloat v0, GLfloat v1) { glRun(glUniform2f(loc, v0, v1)); }
  void setUniform(GLint loc, GLfloat v0, GLfloat v1, GLfloat v2) { glRun(glUniform3f(loc, v0, v1, v2)); }
  void setUniform(GLint loc, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) { glRun(glUniform4f(loc, v0, v1, v2, v3)); }
  void setUniform(GLint loc, GLint v0) { glRun(glUniform1i(loc, v0)); }
  void setUniform(GLint loc, GLint v0, GLint v1) { glRun(glUniform2i(loc, v0, v1)); }
  void setUniform(GLint loc, GLint v0, GLint v1, GLint v2) { glRun(glUniform3i(loc, v0, v1, v2)); }
  void setUniform(GLint loc, GLint v0, GLint v1, GLint v2, GLint v3) { glRun(glUniform4i(loc, v0, v1, v2, v3)); }

  /// Restore the uniform state stored in list.
  void setUniform(UniformVar::List list, bool relocate = true);

  /// Creates a new Shader and adds it to this program.
  virtual ShaderPtr addShader(const QString& filename, Shader::Type type);

  /**
   * (Re)Links the program. If the program is currently linked, saves the uniform
   * variable state before relinking. After successful linking, restores the state.
   */
  virtual void link();

  bool isLinked();

  /// Object name, like "Phong"
  const QString& name() const { return m_name; }

  /// Returns the current state of all uniform variables
  UniformVar::List getUniformList();

  /// Returns the actual OpenGL program id, or 0 if there is no program created yet.
  GLuint id() const;

  Shaders shaders() const { return m_shaders; }

signals:
  /// Emitted every time the program is successfully linked
  void linked(ProgramPtr);
  /// Shader was compiled
  void shaderCompiled(ShaderPtr, ShaderError::List);

protected:
  QString m_name;
  GLuint m_prog;

  Shaders m_shaders;

  /// Are all shaders compiled
  bool m_compiled;

  /// The last known uniform values of the shader program.
  /// This is updated every time before we relink anything, and restored after successful linking.
  UniformVar::List m_uniformList;
};

#endif
