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
