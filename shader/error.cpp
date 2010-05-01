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

#include "shader/error.hpp"

ShaderError::ShaderError() : m_line(0), m_column(0), m_length(0) {}
ShaderError::ShaderError(ShaderPtr shader, QString msg, QString type, int line, int column, int length)
  : m_shader(shader), m_msg(msg), m_type(type),
    m_line(line), m_column(column), m_length(length) {}

bool ShaderError::operator<(const ShaderError& o) const {
  if (m_shader != o.m_shader) return m_shader < o.m_shader;
  if (m_line != o.m_line) return m_line < o.m_line;
  if (m_column != o.m_column) return m_column < o.m_column;
  if (m_length != o.m_length) return m_length < o.m_length;
  if (m_type != o.m_type) return m_type < o.m_type;
  return m_msg < o.m_msg;
}
