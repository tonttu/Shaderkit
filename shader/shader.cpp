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
#include "shader/grammar.hpp"
#include "shader/compiler_output_parser.hpp"

#include <QFile>

Shader::Shader(ProgramPtr prog, QGLShader::ShaderTypeBit type)
  : m_shader(0), m_prog(prog), m_needCompile(false), m_type(type) {
}

bool Shader::loadFile(const QString& filename) {
  bool changed = false;
  QFile file(filename);
  if (file.open(QFile::ReadOnly | QFile::Text)) {
    changed = loadSrc(file.readAll());
    m_filename = filename;
  }
  return changed;
}

bool Shader::loadSrc(const QString& data) {
  if (m_src != data) {
    m_src = data;
    m_needCompile = true;
    // m_prog->setIsCompiled(false);
    return true;
  }
  return false;
}

QList<ShaderError> Shader::compile(bool& compiled) {
  if (m_needCompile) {
    m_needCompile = false;
    if (!m_shader) m_shader = new QGLShader(m_type, this);

    compiled = m_shader->compileSourceCode(m_src);

    GLint len = 0;
    glGetShaderiv(m_shader->shaderId(), GL_INFO_LOG_LENGTH, &len);
    // len may include the zero byte
    if (len > 1) return handleCompilerOutput(*m_shader, m_src);
  } else {
    compiled = false;
  }
  return QList<ShaderError>();
}

QList<ShaderError> Shader::handleCompilerOutput(QGLShader& shader, const QString& src) {
  QList<ShaderError> errors;

  // Split the source tokens to lines so that we can find the exact error location
  ShaderLexer lexer;
  lexer.loadSrc(src);
  std::string& data = lexer.toLines();
  const GLchar* str = data.c_str();
  GLint len = data.length();

  // Recompile
  glShaderSource(shader.shaderId(), 1, &str, &len);
  glCompileShader(shader.shaderId());

  glGetShaderiv(shader.shaderId(), GL_INFO_LOG_LENGTH, &len);

  // Usually this shouldn't happen, since this function is called only
  // when there actually are some errors in the source code.
  if (len < 1) return errors;

  // Read the info log
  GLchar log[len];
  glGetShaderInfoLog(shader.shaderId(), len, &len, log);

  ShaderCompilerOutputParser parser(QString::fromAscii(log, len));
  while (parser.left()) {
    ShaderError e = parser.next();
    const ShaderLexer::Token& token = lexer.transform(e.line());
    e.setLine(token.line);
    e.setColumn(token.column);
    e.setLength(token.len);
    errors.push_back(e);
  }
  return errors;
}
