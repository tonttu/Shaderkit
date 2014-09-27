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

#ifndef SHADER_SHADER_HPP
#define SHADER_SHADER_HPP

#include "forward.hpp"
#include "gl/error.hpp"
#include "gl/opengl.hpp"
#include "core/file_resource.hpp"

#include <QIcon>

namespace Shaderkit
{

  /**
   * GLSL Shader.
   *
   * It is safe to create Shader objects before actually having an OpenGL context.
   * The actual shader object is created only when the shader is compiled the first time.
   */
  class Shader : public QObject, public std::enable_shared_from_this<Shader>, public FileResource
  {
    Q_OBJECT

  public:
    /// Supported shader types
    enum Type {
      Unknown = 0,
      Vertex = GL_VERTEX_SHADER,
      TessCtrl = GL_TESS_CONTROL_SHADER,
      TessEval = GL_TESS_EVALUATION_SHADER,
      Geometry = GL_GEOMETRY_SHADER,
      Fragment = GL_FRAGMENT_SHADER,
      Compute = GL_COMPUTE_SHADER
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

    const QString& src() const { return m_src; }

    /**
     * Compiles the shader, if m_needCompile is true. Returns the outcome of the
     * compiling and fills the ErrorList given as a reference.
     *
     * Sets the m_needCompile to false even if the compiling failed, since broken
     * code won't work even if compiled many times..
     *
     * @param errors All the compile errors will be appended to the list.
     */
    CompileStatus compile(ShaderErrorList& errors);

#if 0
    /**
     * If the shader was once loaded from a file, return the original filename or
     * an empty string.
     *
     * Note that the source code might have changed either in the file or in the
     * shader (by loadSrc).
     * /// @todo this should now be res, not filename
     */
    QString res() const { return m_filename; }
#endif

    virtual void setFilename(const QString& filename);

    /// Returns the actual OpenGL shader id, or 0 if there is no shader created yet.
    GLuint id() const;

    Type type() const { return m_type; }

    static QIcon icon(Type type);
    QIcon icon();

    /// @shouldn't this be asynchronous, what if we don't have a valid context right now?
    bool getBuiltinMacro(QString name, float& out);

    ShaderPtr clone(ProgramPtr prog) const;

    ProgramPtr program() const;
    void setProgram(ProgramPtr prog);

    static void setSandboxCompile(bool v);
    static bool sandboxCompile() { return s_sandbox_compile; }

    static Type guessType(const QString& filename);

    virtual void filenameChanged();

  signals:
    void changed(ShaderPtr);

  protected:
    /**
     * Recompiles the shader in a way that the error list can be generated.
     * Call this only if there are some errors with the shader.
     *
     * @return true if Returns true if the output parsing is successful.
     */
    bool handleCompilerOutput(ShaderErrorList& errors);

    /// The actual wrapped shader object
    GLuint m_shader;

    /// The shader program this shader belongs to.
    std::weak_ptr<GLProgram> m_prog;

    /// After loadFile or loadSrc this is set to true
    /// compile() won't do anything if this is false
    bool m_needCompile;


    /// The current shader source code - it might not be the source code of the
    /// current compiled version, if compile() isn't called yet, or if the
    /// compiling failed.
    QString m_src;

    /// The type of the shader
    Type m_type;

    /// Some OpenGL drivers crash the application on some specific cases
    /// when the source has weird syntax errors. If this is enabled, we
    /// always compile shaders first in a safe environment.
    /// I mean you, NVIDIA!
    static bool s_sandbox_compile;
  };

} // namespace Shaderkit

#endif
