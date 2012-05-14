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

#include "gl/shader.hpp"
#include "gl/program.hpp"
#include "gl/compiler_output_parser.hpp"
#ifndef _WIN32
#include "gl/sandbox_compiler.hpp"
#endif
#include "gl/query_shader.hpp"
#include "gl/opengl.hpp"

#include <QFile>

namespace Shaderkit
{

  bool Shader::s_sandbox_compile = true;

  Shader::Shader(ProgramPtr prog, Shader::Type type)
    : m_shader(0), m_prog(prog), m_needCompile(false), m_type(type)
  {
  }

  Shader::~Shader()
  {
    if (m_shader) {
      ProgramPtr prog = program();
      if (prog) prog->removeShader(shared_from_this());
      glDeleteShader(m_shader);
    }
  }

  bool Shader::loadFile(const QString& f)
  {
    bool changed = false;
    setFilename(f);
    QFile file(filename());
    if (file.open(QFile::ReadOnly | QFile::Text)) {
      changed = loadSrc(file.readAll());
    }
    return changed;
  }

  bool Shader::loadSrc(const QString& data)
  {
    if (m_src != data) {
      m_src = data;
      m_needCompile = true;
      // Tell the program object, that it needs to recompile stuff
      ProgramPtr p = m_prog.lock();
      if (p) p->setIsCompiled(false);
      return true;
    }
    return false;
  }

  Shader::CompileStatus Shader::compile(ShaderErrorList& errors)
  {
    glCheck("Shader::compile");
    if (m_needCompile) {
      m_needCompile = false;
      if (!m_shader) {
        m_shader = glRun2(glCreateShader(m_type));
        if (!m_shader) {
          Log::error("Shader: could not create shader");
          return ERRORS;
        }
      }

      QByteArray src_ = m_src.toAscii();

#ifndef _WIN32
      if (s_sandbox_compile && !SandboxCompiler::check(shared_from_this(), src_, errors)) {
        return ERRORS;
      }
#endif

      const char* src = src_.data();
      GLint len = src_.length();
      glRun(glShaderSource(m_shader, 1, &src, &len));
      glRun(glCompileShader(m_shader));

      GLint ok = 0;
      glRun(glGetShaderiv(m_shader, GL_COMPILE_STATUS, &ok));
      glRun(glGetShaderiv(m_shader, GL_INFO_LOG_LENGTH, &len));
      int error_count = errors.size();
      // len may include the zero byte
      if (len > 1 && !handleCompilerOutput(errors)) {
        Log::error("Failed to parse GLSL compiler output");
      }
      error_count = errors.size() - error_count;
      return ok ? error_count ? WARNINGS : OK : ERRORS;
    } else {
      return NONE;
    }
  }

  void Shader::setFilename(const QString& filename)
  {
    FileResource::setFilename(filename);
    emit changed(shared_from_this());
  }

  GLuint Shader::id() const
  {
    return m_shader;
  }

  QIcon Shader::icon(Type type)
  {
    const char* icon = ":/icons/frag.png";
    if (type == Shader::Vertex)
      icon = ":/icons/vert.png";
    else if (type == Shader::Geometry)
      icon = ":/icons/geom.png";

    return QIcon(icon);
  }

  QIcon Shader::icon()
  {
    return icon(m_type);
  }

  bool Shader::getBuiltinMacro(QString name, float& out)
  {
    /// @todo use cleaned up version of the actual shader here
    QString src = "#version 150\n"
                  "out float x;\n"
                  "void main() {\n"
                  "  x = %1;\n"
                  "}\n";

    QueryShader& s = QueryShader::instance();
    bool ok = false;
    if (s.compile(Vertex, src.arg(name)) && s.bind("x")) {
      glRun(glDrawArrays(GL_POINTS, 0, 1));
      ok = s.unbind(out);
    }
    return ok;
  }

  ShaderPtr Shader::clone(ProgramPtr prog) const
  {
    ShaderPtr s(new Shader(prog, m_type));
    s->setFilename(rawFilename());
    s->m_src = m_src;
    s->m_needCompile = true;
    if (prog) prog->setIsCompiled(false);
    return s;
  }

  ProgramPtr Shader::program() const
  {
    return m_prog.lock();
  }

  void Shader::setProgram(ProgramPtr prog)
  {
    m_prog = prog;
  }

  void Shader::setSandboxCompile(bool v)
  {
#ifndef _WIN32
    if (s_sandbox_compile && !v)
      SandboxCompiler::close();
#endif
    s_sandbox_compile = v;
  }

  Shader::Type Shader::guessType(const QString& filename)
  {
    /// @todo tesselation shaders?
    if (filename.endsWith(".fs") || filename.endsWith(".frag"))
      return Fragment;
    if (filename.endsWith(".vs") || filename.endsWith(".vert"))
      return Vertex;
    if (filename.endsWith(".gs") || filename.endsWith(".geom"))
      return Geometry;

    /// @todo we could parse the file and use some heuristic to determine the type by it's contents
    return Unknown;
  }

  void Shader::filenameChanged()
  {
    emit changed(shared_from_this());
  }

  bool Shader::handleCompilerOutput(ShaderErrorList& errors)
  {
    glCheck("handleCompilerOutput");

    ShaderCompilerOutputParser& parser = ShaderCompilerOutputParser::instance();
    return parser.parse(*this, errors);
  }

} // namespace Shaderkit
