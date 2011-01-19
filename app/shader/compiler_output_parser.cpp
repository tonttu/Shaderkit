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
#include <iostream>

/**
 * NVIDIA:
 *  0(11) : warning C7522: OpenGL requires constants to be initialized
 *  0(11) : error C0000: syntax error, unexpected identifier, expecting ',' or ';' at token "foo"
 *
 * MESA:
 *  0:8(1): error: syntax error, unexpected XOR_ASSIGN, expecting ',' or ';'
 */
ShaderCompilerOutputParser::ShaderCompilerOutputParser(QString compiler_output)
  : m_pos(0) {
  m_lines = compiler_output.split(QRegExp("[\\r\\n]+"), QString::SkipEmptyParts);
}

bool ShaderCompilerOutputParser::left() {
  return m_pos < m_lines.size();
}

ShaderError ShaderCompilerOutputParser::next() {
  static QList<QRegExp> s_patterns;
  if (s_patterns.isEmpty()) {
    QString pattern;

    // 1.50 NVIDIA via Cg compiler
    pattern = "\\d+ \\(  (\\d+)  \\)"  // "0(11)", shader(line number [1])
              "\\s* : \\s*"            // ":", separator
              "([^\\s]+)  \\s+"        // "warning", type [2]
              "[^\\s:]+"               // "C7522", nvidia error code
              "\\s* : \\s*"            // ":", separator
              "(.*)";                  // the actual error [3]
    pattern.remove(QChar(' '));
    s_patterns << QRegExp(pattern, Qt::CaseSensitive, QRegExp::RegExp2);

    // MESA
    /// @todo Use the column information here and skip the recompile hack totally
    pattern = "\\d+ : (\\d+) \\(\\d+\\)"  // "0:8(1)", shader:line [1] (column)
              "\\s* : \\s*"               // ":", separator
              "([^\\s]+)"                 // "warning:", type [2]
              "\\s* : \\s*"               // ":", separator
              "(.*)";                     // the actual error [3]
    pattern.remove(QChar(' '));
    s_patterns << QRegExp(pattern, Qt::CaseSensitive, QRegExp::RegExp2);
  }

  QStringList list;
  QString msg = m_lines[m_pos++];
  for (int i = 0; i < s_patterns.size(); ++i) {
    int ret = s_patterns[i].indexIn(msg);
    if(ret >= 0) {
      list = s_patterns[i].capturedTexts();
      if (i != 0) {
        QRegExp r = s_patterns[i];
        s_patterns.removeAt(i);
        s_patterns.insert(0, r);
      }
    }
  }
//  m_pos++;

  if (list.size() == 4) {
    return ShaderError(ShaderPtr(), list[3], list[2], list[1].toInt()-1);
  } else {
    std::cerr << "Failed to parse error string: '" << msg.toUtf8().data() << "'" << std::endl;
    return ShaderError(ShaderPtr(), msg, "error", 0, 0);
  }
}

