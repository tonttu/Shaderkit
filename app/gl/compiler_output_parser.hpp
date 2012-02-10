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

#ifndef SHADER_COMPILER_OUTPUT_PARSER_HPP
#define SHADER_COMPILER_OUTPUT_PARSER_HPP

#include "gl/error.hpp"

#include <QStringList>
#include <QRegExp>


class ParserImpl {
public:
  virtual ~ParserImpl() {}
  virtual bool parse(const QStringList& list, ShaderErrorList& errors) = 0;
  virtual bool hasColumnInformation() const;
};

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
  ShaderCompilerOutputParser();

  bool parse(const QString & output, ShaderErrorList& errors);
  bool parse(Shader& shader, ShaderErrorList& errors);

  QStringList split(const QString& str) const;

  static ShaderCompilerOutputParser& instance();

protected:
  QList<std::shared_ptr<ParserImpl>> m_parsers;
};

#endif
