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
