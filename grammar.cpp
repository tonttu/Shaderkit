#include "grammar.hpp"
#include "lexer.hpp"

#include <QFile>

ShaderLexer::Token::Token() : token(0), pos(0), len(0), column(0), line(0) {}
ShaderLexer::Token::Token(int token_, int pos_, int len_, int column_, int line_)
  : token(token_), pos(pos_), len(len_), column(column_), line(line_) {}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

ShaderLexer::ShaderLexer() : m_tokenized_length(0) {}

void ShaderLexer::loadFile(const QString& name) {
  QFile file(name);
  if (file.open(QFile::ReadOnly | QFile::Text)) {
    m_data = file.readAll();
    tokenize();
  }
}

void ShaderLexer::loadSrc(const QString& src) {
  m_data = src.toUtf8();
  tokenize();
}

void ShaderLexer::tokenize() {
  /// @todo flex code is not thread safe
  yyset_scan_string(m_data.data(), m_data.length());
  m_tokenized_length = 0;

  int token;
  while ((token = yylex_wrapper())) {
    int l = yyget_leng();
    m_tokens.push_back(Token(token, yyget_pos()-l, l, yyget_column() - l, yyget_line()));
    m_tokenized_length += l;
  }
}

std::string& ShaderLexer::toLines() {
  m_splitted.clear();
  // the length of the splitted src is the length of all tokens splitted to lines
  m_splitted.reserve(m_tokenized_length + m_tokens.size());
  const char *data = m_data.data();
  for (QVector<Token>::const_iterator it = m_tokens.begin(); it != m_tokens.end(); it++) {
    m_splitted.append(data + it->pos, it->len);
    m_splitted.append(1, '\n');
  }
  return m_splitted;
}

/// splitted line number -> (original line, original column)
const ShaderLexer::Token& ShaderLexer::transform(int line) {
  if (line < 0) return m_tokens[0];
  return m_tokens[line];
}
