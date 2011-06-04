/**
 * Copyright 2010,2011 Riku Palomäki.
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

#include "shader/grammar.hpp"
#include "shader/lexer.hpp"

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
  glslset_scan_string(m_data.data(), m_data.length());
  m_tokenized_length = 0;
  m_tokens.clear();

  int token;
  while ((token = glsllex_wrapper())) {
    int l = glslget_leng();
    m_tokens.push_back(Token(token, glslget_pos()-l, l, glslget_column() - l, glslget_line()));
    m_tokenized_length += l;
  }
}

std::string& ShaderLexer::toLines() {
  m_splitted.clear();
  // the length of the splitted src is the length of all tokens splitted to lines
  m_splitted.reserve(m_tokenized_length + m_tokens.size());
  const char *data = m_data.data();
  for (auto it = m_tokens.begin(); it != m_tokens.end(); it++) {
    m_splitted.append(data + it->pos, it->len);
    m_splitted.append(1, '\n');
  }
  return m_splitted;
}

int ShaderLexer::tokens() const {
  return m_tokens.size();
}

/// splitted line number -> (original line, original column)
const ShaderLexer::Token& ShaderLexer::transform(int line) {
  if (line < 0) return m_tokens[0];
  return m_tokens[line];
}
