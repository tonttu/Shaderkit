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

#include "shader/shader.hpp"
#include "shader/program.hpp"
#include "shader/grammar.hpp"
#include "shader/compiler_output_parser.hpp"
#include "shader/sandbox_compiler.hpp"
#include "shader/query_shader.hpp"
#include "app/opengl.hpp"

#include <QFile>

bool Shader::s_sandbox_compile = true;

Shader::Shader(ProgramPtr prog, Shader::Type type)
  : m_shader(0), m_prog(prog), m_needCompile(false), m_type(type) {
}

Shader::~Shader() {
  if (m_shader)
    glDeleteShader(m_shader);
}

bool Shader::loadFile(const QString& filename) {
  bool changed = false;
  m_filename = filename;
  QFile file(filename);
  if (file.open(QFile::ReadOnly | QFile::Text)) {
    changed = loadSrc(file.readAll());
  }
  return changed;
}

bool Shader::loadSrc(const QString& data) {
  if (m_src != data) {
    m_src = data;
    m_needCompile = true;
    // Tell the program object, that it needs to recompile stuff
    m_prog->setIsCompiled(false);
    return true;
  }
  return false;
}

Shader::CompileStatus Shader::compile(ShaderError::List& errors) {
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

    if (s_sandbox_compile && !SandboxCompiler::check(shared_from_this(), src_, errors)) {
      return ERRORS;
    }

    const char* src = src_.data();
    GLint len = src_.length();
    glRun(glShaderSource(m_shader, 1, &src, &len));
    glRun(glCompileShader(m_shader));

    GLint ok = 0;
    glRun(glGetShaderiv(m_shader, GL_COMPILE_STATUS, &ok));
    glRun(glGetShaderiv(m_shader, GL_INFO_LOG_LENGTH, &len));
    // len may include the zero byte
    if (len > 1) {
      bool parse_ok = handleCompilerOutput(m_src, errors);
      return ok && parse_ok ? WARNINGS : ERRORS;
    } else {
      return ok ? OK : ERRORS;
    }
  } else {
    return NONE;
  }
}

GLuint Shader::id() const {
  return m_shader;
}


QIcon Shader::icon() {
  const char* icon = ":/icons/frag.png";
  if (m_type == Shader::Vertex)
    icon = ":/icons/vert.png";
  else if (m_type == Shader::Geometry)
    icon = ":/icons/geom.png";

  return QIcon(icon);
}

bool Shader::getBuiltinMacro(QString name, float& out) {
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


void Shader::setSandboxCompile(bool v) {
  if (s_sandbox_compile && !v)
    SandboxCompiler::close();
  s_sandbox_compile = v;
}

bool Shader::handleCompilerOutput(const QString& src, ShaderError::List& errors) {
  glCheck("handleCompilerOutput");

  // Split the source tokens to lines so that we can find the exact error location
  ShaderLexer lexer;
  lexer.loadSrc(src);
  std::string& data = lexer.toLines();
  const GLchar* str = data.c_str();
  GLint len = data.length();

  // Recompile
  glRun(glShaderSource(id(), 1, &str, &len));
  glRun(glCompileShader(id()));

  glRun(glGetShaderiv(id(), GL_INFO_LOG_LENGTH, &len));

  // Usually this shouldn't happen, since this function is called only
  // when there actually are some errors in the source code.
  if (len < 1) return false;

  // Read the info log
#ifdef _MSC_VER
  GLchar* log = new GLchar[len];
#else
  GLchar log[len];
#endif
  glRun(glGetShaderInfoLog(id(), len, &len, log));

  // unless we get something parsed from the output, we think this as a failure
  bool ok = false;

  ShaderCompilerOutputParser parser(QString::fromUtf8(log, len));
  int l = lexer.tokens();
  foreach (ShaderError e, parser.parse()) {
    e.setRes(res());

    if (e.line() > l || l == 0) {
      Log::error("BUG on Shader::handleCompilerOutput, e.line: %d, l: %d, log: %s, data: %s, src: %s",
                 e.line(), l, log, data.c_str(), src.toUtf8().data());
      errors.push_back(e);
      continue;
    }
    const ShaderLexer::Token& token = lexer.transform(e.line());
    e.setLine(token.line);
    e.setColumn(token.column);
    e.setLength(token.len);
    errors.push_back(e);
    ok = true;
  }

#ifdef _MSC_VER
  delete[] log;
#endif

  return ok;
}
