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

#ifndef SHADER_SHADER_HPP
#define SHADER_SHADER_HPP

#include "app/forward.hpp"
#include "shader/error.hpp"
#include "app/opengl.hpp"

#include <QObject>

/**
 * GLSL Shader.
 *
 * It is safe to create Shader objects before actually having an OpenGL context.
 * The actual shader object is created only when the shader is compiled the first time.
 */
class Shader : public QObject, public std::enable_shared_from_this<Shader> {
  Q_OBJECT

public:
  /// Supported shader types
  enum Type {
    Fragment, Vertex, Geometry
  };

  /// compile() returns the compile status
  enum CompileStatus {
    NONE,     /// Nothing was made, m_needCompile was already false
    ERRORS,   /// The program was compiled with errors or there was an internal error
              /// (couldn't parse the output, wrong OpenGL context or something similar)
    WARNINGS, /// The program was compiled with non-fatal errors or warnings
    OK        /// The program was compiled successfully
  };

  /// Creates a new shader of type `type' that is part of program `prog'
  Shader(ProgramPtr prog, Type type);

  virtual ~Shader();

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
   * Compiles the shader, if m_needCompile is true. Returns the outcome of the
   * compiling and fills the ErrorList given as a reference.
   *
   * Sets the m_needCompile to false even if the compiling failed, since broken
   * code won't work even if compiled many times..
   *
   * @param errors All the compile errors will be appended to the list.
   */
  CompileStatus compile(ShaderError::List& errors);

  /**
   * If the shader was once loaded from a file, return the original filename or
   * an empty string.
   *
   * Note that the source code might have changed either in the file or in the
   * shader (by loadSrc).
   */
  QString filename() const { return m_filename; }

  /// Returns the actual OpenGL shader id, or 0 if there is no shader created yet.
  GLuint id() const;

  Type type() const { return m_type; }

protected:
  /**
   * Recompiles the shader in a way that the error list can be generated.
   * Call this only if there are some errors with the shader.
   *
   * Returns true if the output parsing is successful.
   */
  bool handleCompilerOutput(const QString& src, ShaderError::List& errors);

  /// The actual wrapped shader object
  GLuint m_shader;

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
  Type m_type;
};

#endif
