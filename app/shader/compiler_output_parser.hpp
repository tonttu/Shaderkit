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

#ifndef SHADER_COMPILER_OUTPUT_PARSER_HPP
#define SHADER_COMPILER_OUTPUT_PARSER_HPP

#include "shader/error.hpp"

#include <QStringList>
#include <QRegExp>

/**
 * Tries to parse the GLSL Compiler output. Unfortunately the output format is
 * implementation-dependent, so the parsing is not guaranteed to work.
 *
 * This class works also as an iterator to ShaderErrors that are generated
 * from the compiler output string.
 *
 * @todo In some cases, this should try to locate the correct token (which
 *       isn't usually the one got from Lexer at the point of the error, but
 *       somewhere before that) and include that information to the ShaderError.
 */
class ShaderCompilerOutputParser {
public:
  /// Initializes the parser with given compiler output
  /// @todo Implement also some other formats than "1.50 NVIDIA via Cg compiler"
  ShaderCompilerOutputParser(QString compiler_output);

  /// Returns true if there is still at least one ShaderError to read
  bool left();

  /// Reads the next ShaderError generated from the compiler output,
  /// and moves to the next error.
  ShaderError next();

protected:
  /// The current parsing regexp, compiled in the constructor
  QRegExp m_regexp;

  /// The compiler output splitted to lines
  QStringList m_lines;

  /// The current line we are parsing
  int m_pos;
};

#endif
