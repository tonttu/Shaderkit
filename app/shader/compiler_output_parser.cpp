/**
 * Copyright 2010 Riku Palomäki.
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

#include "shader/compiler_output_parser.hpp"

#include <cassert>

/**
 * NVIDIA:
 *  0(11) : warning C7522: OpenGL requires constants to be initialized
 *  0(11) : error C0000: syntax error, unexpected identifier, expecting ',' or ';' at token "foo"
 *
 * MESA:
 *  0:8(1): error: syntax error, unexpected XOR_ASSIGN, expecting ',' or ';'
 */
ShaderCompilerOutputParser::ShaderCompilerOutputParser(QString compiler_output) : m_pos(0) {
  QString pattern;
  if (true /* 1.50 NVIDIA via Cg compiler */) {
    pattern = "\\d+ \\(  (\\d+)  \\)"  // "0(11)", shader(line number [1])
              "\\s* : \\s*"            // ":", separator
              "([^\\s]+)  \\s+"        // "warning", type [2]
              "[^\\s:]+"               // "C7522", nvidia error code
              "\\s* : \\s*"            // ":", separator
              "(.*)";                  // the actual error [3]
  } else if (true /* MESA */) {
    pattern = "\\d+ : (\\d+) \\(\\d+\\)"  // "0:8(1)", shader:line [1] (???)
              "\\s* : \\s*"               // ":", separator
              "([^\\s]+)"                 // "warning:", type [2]
              "\\s* : \\s*"               // ":", separator
              "(.*)";                     // the actual error [3]
  }
  pattern.remove(QChar(' '));
  m_regexp = QRegExp(pattern, Qt::CaseSensitive, QRegExp::RegExp2);
  m_lines = compiler_output.split(QRegExp("[\\r\\n]+"), QString::SkipEmptyParts);
}

bool ShaderCompilerOutputParser::left() {
  return m_pos < m_lines.size();
}

ShaderError ShaderCompilerOutputParser::next() {
  int ret = m_regexp.indexIn(m_lines[m_pos++]);
  /// @todo assert is way too strong for this, fix this
  assert(ret >= 0);

  QStringList list = m_regexp.capturedTexts();

  /// @todo assert is way too strong for this, fix this
  assert(list.size() == 4);

  return ShaderError(ShaderPtr(), list[3], list[2], list[1].toInt()-1);
}

