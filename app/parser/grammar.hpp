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

#ifndef GRAMMAR_H
#define GRAMMAR_H

#include <QPair>
#include <QVector>

/**
 * GLSL Lexer, wrapper for Flex lexer code (lex.yy.c).
 *
 * Can also split the GLSL source code to multiple lines and transform between
 * the original line numbers and splitted line numbers. This can be useful when
 * wanting to find the exact token where the GLSL compiler stops, since usually
 * compilers only tell the line number of an error/warning.
 *
 * @see ShaderCompilerOutputParser
 */
class ShaderLexer {
public:
  /// Token is just a reference to m_data
  struct Token {
    Token();
    Token(int token, int pos, int len, int column, int line);

    /// token is a token defined in enum yytokentype in glsl.tab.hpp
    int token;
    int pos, len;
    int column, line;
  };

  ShaderLexer();

  /// Loads the file and tokenizes it
  void loadFile(const QString& filename);
  /// Loads the src and tokenizes it
  void loadSrc(const QString& src);

  /// Splits the tokenized shader code to lines, so that every token is in it's own line
  /// also saves the transformation table of this split to be usable in transform()
  std::string& toLines();

  /// Returns ne number of tokens (original source code lines)
  int tokens() const;

  /// Gives the token (in the original source code context) that is on the given splitted line
  /// Line indexing starts from zero.
  const Token& transform(int line);

protected:
  void tokenize();

  /// The original source code
  QByteArray m_data;

  /// The sum of lengths of all the tokens without any whitespace.
  /// This is calculated in tokenize()
  size_t m_tokenized_length;

  /// Filled in tokenize()
  QVector<Token> m_tokens;

  /// The string that toLines returns
  std::string m_splitted;
};

#endif // GRAMMAR_H
