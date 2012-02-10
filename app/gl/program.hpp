/**
 * Copyright 2010-2012 Riku Palomäki.
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

#ifndef SHADER_PROGRAM_HPP
#define SHADER_PROGRAM_HPP

#include "app/forward.hpp"
#include "gl/uniform.hpp"
#include "gl/shader.hpp"
#include "gl/error.hpp"
#include "gl/opengl.hpp"

#include "Eigen/Geometry"

#include <QObject>
#include <QSet>

class ShaderManager : public QObject {
  Q_OBJECT

public:
  ShaderManager();
  virtual ~ShaderManager();
  static ShaderManager& instance();

signals:
  /// Emitted every time the program is linked
  void linked(ShaderErrorList);
  /// Shader was compiled
  void compiled(ShaderErrorList);

  void changed(ShaderPtr);

  friend class GLProgram;
  friend class Shader;
};


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
class GLProgram : public QObject, public std::enable_shared_from_this<GLProgram> {
  Q_OBJECT

public:
  typedef QSet<ShaderPtr> Shaders;

  /// Creates an empty program object and connects linked-signal to Properties::update.
  GLProgram(const QString& name);
  virtual ~GLProgram();

  /**
   * Binds the program object. On the first run time creates the real shader
   * program object. Also (re)compiles all the shaders and (re)links the program
   * if needed. For every compiled shader the shaderCompiled()-signal is emitted.
   */
  bool bind(State* state);

  /// Unbinds the program object.
  void unbind();

  int attribLocation(const QString& name);

  /// Sets the compiled status
  void setIsCompiled(bool state) { m_compiled = state; }

  /// Sets a uniform variable, binds the object if needed,
  /// and restores the active program before returning.
  template <typename T>
  void setUniform(State* state, QString name, const T& t, bool restore = false) {
    glCheck("setUniform");
    GLint prog = 0;
    glRun(glGetIntegerv(GL_CURRENT_PROGRAM, &prog));
    if (!prog != m_prog)
      bind(state);

    GLint loc = glRun2(glGetUniformLocation(m_prog, name.toAscii().data()));
    if(loc == -1) {
      //Log::error("Failed to query uniform variable %s location", name.toAscii().data());
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

  void setUniform(GLint loc, const Eigen::Vector2f& v) { glRun(glUniform2fv(loc, 1, v.data())); }
  void setUniform(GLint loc, const Eigen::Vector3f& v) { glRun(glUniform3fv(loc, 1, v.data())); }
  void setUniform(GLint loc, const Eigen::Vector4f& v) { glRun(glUniform4fv(loc, 1, v.data())); }


  /// Restore the uniform state stored in list.
  void setUniform(UniformVar::List list, bool relocate = true);

  /// Creates a new Shader and adds it to this program.
  virtual ShaderPtr addShader(const QString& filename, Shader::Type type);

  virtual ShaderPtr addShaderSrc(const QString& data, Shader::Type type);

  virtual void addShader(ShaderPtr shader);

  bool removeShader(ShaderPtr shader);

  /**
   * (Re)Links the program. If the program is currently linked, saves the uniform
   * variable state before relinking. After successful linking, restores the state.
   */
  virtual void link(State* state);

  bool isLinked();

  /// Object name, like "Phong"
  const QString& name() const { return m_name; }

  /// Returns the current state of all uniform variables
  UniformVar::List getUniformList();

  /// Returns the actual OpenGL program id, or 0 if there is no program created yet.
  GLuint id() const;

  QVariantMap toMap(ScenePtr scene, QVariantMap& map, bool pack = false) const;

  Shaders shaders() const { return m_shaders; }
  bool hasShader(const QString& filename) const;

  ProgramPtr clone() const;

  void setTransformFeedbackVaryings(QStringList lst);

signals:
  void changed();

protected:
  QString m_name;
  GLuint m_prog;

  Shaders m_shaders;

  /// Are all shaders compiled
  bool m_compiled;

  /// Force relink
  bool m_relink;

  /// The last known uniform values of the shader program.
  /// This is updated every time before we relink anything, and restored after successful linking.
  UniformVar::List m_uniformList;

  QList<QByteArray> m_transformFeedback;
};

#endif
