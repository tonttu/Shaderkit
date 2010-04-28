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

#ifndef SHADER_SHADER_HPP
#define SHADER_SHADER_HPP

#include "forward.hpp"

#include <QObject>
#include <QGLShader>

/**
 * GLSL Shader.
 *
 * It is safe to create Shader objects before actually having an OpenGL context.
 * The actual shader object is created only when the shader is compiled the first time.
 */
class Shader : public QObject {
  Q_OBJECT

public:
  /// Creates a new shader of type `type' that is part of program `prog'
  Shader(ProgramPtr prog, QGLShader::ShaderTypeBit type);

  /**
   * Loads shader from file, and stages the shader for recompiling.
   * Returns true if loading was successful and shader was changed. However
   * the shader isn't compiled yet, so successful loading doesn't mean that
   * the shader will compile/link correctly.
   */
  bool loadFile(const QString& filename);

  /**
   * Loads shader source from string, and stages the shader for recompiling.
   * Returns true if loading was successful and shader was changed. However
   * the shader isn't compiled yet, so successful loading doesn't mean that
   * the shader will compile/link correctly.
   */
  bool loadSrc(const QString& data);

  /**
   * Compiles the shader, if m_needCompile is true. Returns list of compile errors.
   *
   * Sets the m_needCompile to false even if the compiling failed, since broken
   * code won't work even if compiled many times..
   *
   * @param compiled Will be set to true if the shader was compiled successfully,
   *                 false means that either the shader was already compiled, or
   *                 there was an error while compiling.
   */
  QList<ShaderError> compile(bool& compiled);

  /**
   * If the shader was once loaded from a file, return the original filename or
   * an empty string.
   *
   * Note that the source code might have changed either in the file or in the
   * shader (by loadSrc).
   */
  QString filename() const { return m_filename; }

  /// Typecast operator to QGLShader, since currently the Shader is a wrapper for that.
  operator QGLShader*() { return m_shader; }

protected:
  /**
   * Recompiles the shader in a way that the error list can be generated.
   * Call this only if there are some errors with the shader.
   */
  static QList<ShaderError> handleCompilerOutput(QGLShader& shader, const QString& src);

  /// The actual wrapped shader object
  /// @todo should we just use plain OpenGL API, since QGLShader hardly gives
  ///       us anything useful, and we have our own wrapper classes in any case.
  QGLShader* m_shader;

  /// The shader program this shader belongs to.
  /// @todo can there actually be many programs? We don't probably want that?
  ProgramPtr m_prog;

  /// After loadFile or loadSrc this is set to true
  /// compile() won't do anything if this is false
  bool m_needCompile;

  /// @see filename()
  QString m_filename;

  /// The current shader source code - it might not be the source code of the
  /// current compiled version, if compile() isn't called yet, or if the
  /// compiling failed.
  QString m_src;

  /// The type of the shader
  QGLShader::ShaderTypeBit m_type;
};

#endif
