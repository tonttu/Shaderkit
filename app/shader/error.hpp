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

#ifndef SHADER_ERROR_HPP
#define SHADER_ERROR_HPP

#include "app/forward.hpp"

#include <QString>

/**
 * ShaderError is a parsed GLSL compile error. These are generated by
 * ShaderCompilerOutputParser.
 */
class ShaderError {
public:
  ShaderError();
  /**
   * @param type Should be "error" or "warning", but might also be
   *        something implementation specific
   * @param line Line numbers start from 0
   * @param column Column numbers start from 0
   * @param length The length of the token/error area where the error was seen.
   *               Editor can use this information to underline the error part.
   */
  ShaderError(QString msg, QString type,
              int line, int column = 0, int length = 0);

  void setMaterial(MaterialPtr material) { m_material = material; }
  void setProgram(QString res) { m_program = res; }
  void setShader(QString res) { m_shader = res; }
  void setLine(int line) { m_line = line; }
  void setColumn(int column) { m_column = column; }
  void setLength(int length) { m_length = length; }

  /// Error string, usually straight from the external GLSL compiler, but might
  /// be changed to fix line numbers or similar. The compiled source might have
  /// different line numbers to the source in the editor, see ShaderLexer::toLines
  const QString& msg() const { return m_msg; }

  /// Type of the error, can be "error", "warning" or some implementation dependent value
  const QString& type() const { return m_type; }

  /// The line number (starting from zero) the error begins
  int line() const { return m_line; }
  /// The column position (starting from zero) the error begins
  int column() const { return m_column; }
  /// The length of the error area, obviously only a guess
  int length() const { return m_length; }

  MaterialPtr material() const { return m_material; }
  QString program() const { return m_program; }
  QString shader() const { return m_shader; }

  /// Implemented only for containers that require this.
  bool operator<(const ShaderError& o) const;

protected:
  MaterialPtr m_material;
  QString m_program, m_shader;
  QString m_msg, m_type;
  int m_line, m_column, m_length;
};

class ShaderErrorList : QList<ShaderError> {
public:
  typedef QList<ShaderError> List;
  using List::const_iterator;
  using List::begin;
  using List::end;

  ShaderErrorList(MaterialPtr material_ = MaterialPtr(), QString program_ = "", QString shader_ = "")
    : material(material_), program(program_), shader(shader_) {}

  ShaderErrorList& operator<<(const ShaderError& error) {
    append(error);
    back().setMaterial(material);
    back().setProgram(program);
    back().setShader(shader);
    return *this;
  }

  MaterialPtr material;
  QString program, shader;
};

#endif
