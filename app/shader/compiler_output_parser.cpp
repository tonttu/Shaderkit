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

#include "shader/compiler_output_parser.hpp"

#include <cassert>

/**
 * NVIDIA:
 *  0(11) : warning C7522: OpenGL requires constants to be initialized
 *  0(11) : error C0000: syntax error, unexpected identifier, expecting ',' or ';' at token "foo"
 *    (0) : error C0000: syntax error, unexpected $end at token "<EOF>"
 *
 * MESA:
 *  0:8(1): error: syntax error, unexpected XOR_ASSIGN, expecting ',' or ';'
 */
ShaderCompilerOutputParser::ShaderCompilerOutputParser(QString compiler_output) {
  m_lines = compiler_output.split(QRegExp("[\\r\\n]+"), QString::SkipEmptyParts);
}

ShaderError::List ShaderCompilerOutputParser::parse() {
  static QList<QRegExp> s_patterns;
  static QList<QRegExp> s_modes;
  static QList<QRegExp> s_ignore;
  if (s_patterns.isEmpty()) {
    QString pattern;

    // 1.50 NVIDIA via Cg compiler
    pattern = "\\s*\\d* \\( (\\d+) \\)" // "0(11)", shader(line number [1])
              "\\s* : \\s*"             // ":", separator
              "([^\\s]+)  \\s+"         // "warning", type [2]
              "[^\\s:]+"                // "C7522", nvidia error code
              "\\s* : \\s*"             // ":", separator
              "(.*)";                   // the actual error [3]
    pattern.remove(QChar(' '));
    s_patterns << QRegExp(pattern, Qt::CaseSensitive, QRegExp::RegExp2);

    // MESA
    /// @todo Use the column information here and skip the recompile hack totally
    pattern = "\\s*\\d+ : (\\d+) \\(\\d+\\)" // "0:8(1)", shader:line [1] (column)
              "\\s* : \\s*"                  // ":", separator
              "([^\\s]+)"                    // "warning:", type [2]
              "\\s* : \\s*"                  // ":", separator
              "(.*)";                        // the actual error [3]
    pattern.remove(QChar(' '));
    s_patterns << QRegExp(pattern, Qt::CaseSensitive, QRegExp::RegExp2);

    // 3.30 NVIDIA via Cg compiler
    pattern = "\\s*(\\w*)\\s+info";
    s_modes << QRegExp(pattern, Qt::CaseInsensitive, QRegExp::RegExp2);

    // 3.30 NVIDIA via Cg compiler
    pattern = "\\s*-*\\s*";
    s_ignore << QRegExp(pattern, Qt::CaseSensitive, QRegExp::RegExp2);
  }

  ShaderError::List out;
  QString mode = "";

  foreach (QString msg, m_lines) {
    bool found = false;
    for (int i = 0; i < s_ignore.size(); ++i) {
      if (s_ignore[i].exactMatch(msg)) {
        found = true;
        break;
      }
    }
    if (found) continue;

    for (int i = 0; i < s_modes.size(); ++i) {
      if (s_modes[i].exactMatch(msg)) {
        mode = s_modes[i].cap(1);
        found = true;
        if (i != 0) {
          QRegExp r = s_modes[i];
          s_modes.removeAt(i);
          s_modes.insert(0, r);
        }
        break;
      }
    }
    if (found) continue;

    QStringList list;
    for (int i = 0; i < s_patterns.size(); ++i) {
      if (s_patterns[i].exactMatch(msg)) {
        list = s_patterns[i].capturedTexts();
        if (i != 0) {
          QRegExp r = s_patterns[i];
          s_patterns.removeAt(i);
          s_patterns.insert(0, r);
        }
      }
    }

    if (list.size() == 4) {
      QString str = mode.isEmpty() ? list[3] : list[3] + " (" + mode + ")";
      out << ShaderError(ShaderPtr(), str, list[2], list[1].toInt()-1);
    } else {
      Log::error("Failed to parse error string: '%s'", msg.toUtf8().data());
      QString str = mode.isEmpty() ? msg : msg + " (" + mode + ")";
      out << ShaderError(ShaderPtr(), msg, "error", 0, 0);
    }
  }
  return out;
}

