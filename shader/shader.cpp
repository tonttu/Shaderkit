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

#include "shader/shader.hpp"
#include "shader/program.hpp"
#include "shader/grammar.hpp"
#include "shader/compiler_output_parser.hpp"
#include "opengl.hpp"

#include <QFile>

Shader::Shader(ProgramPtr prog, QGLShader::ShaderTypeBit type)
  : m_shader(0), m_prog(prog), m_needCompile(false), m_type(type) {
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
  if (m_needCompile) {
    m_needCompile = false;
    if (!m_shader) m_shader = new QGLShader(m_type, this);

    bool ok = glRun2(m_shader->compileSourceCode(m_src));

    GLint len = 0;
    glRun(glGetShaderiv(m_shader->shaderId(), GL_INFO_LOG_LENGTH, &len));
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

int Shader::id() const {
  if (m_shader) return m_shader->shaderId();
  return -1;
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
  GLchar log[len];
  glRun(glGetShaderInfoLog(id(), len, &len, log));

  // unless we get something parsed from the output, we think this as a failure
  bool ok = false;

  ShaderCompilerOutputParser parser(QString::fromAscii(log, len));
  while (parser.left()) {
    ShaderError e = parser.next();
    const ShaderLexer::Token& token = lexer.transform(e.line());
    e.setShader(shared_from_this());
    e.setLine(token.line);
    e.setColumn(token.column);
    e.setLength(token.len);
    errors.push_back(e);
    ok = true;
  }
  return ok;
}
