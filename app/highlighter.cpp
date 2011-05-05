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

#include "highlighter.hpp"
#include "shader/lexer.hpp"
#include "log.hpp"

#define YY_HEADER_EXPORT_START_CONDITIONS
#include "glsl_lex.hpp"

#include <vector>

/// group all tokens to groups
namespace SyntaxHL {
  static const int comment_tokens[] = { COMMENT, COMMENT_BEGIN, COMMENT_END };

  static const int pp_tokens[] = { PP };

  static const int type_tokens[] = { BVEC2, BVEC3, BVEC4, IVEC2, IVEC3, IVEC4,
    UVEC2, UVEC3, UVEC4, VEC2, VEC3, VEC4, MAT2, MAT3, MAT4, MAT2X2, MAT2X3,
    MAT2X4, MAT3X2, MAT3X3, MAT3X4, MAT4X2, MAT4X3, MAT4X4, SAMPLER1D,
    SAMPLER2D, SAMPLER3D, SAMPLERCUBE, SAMPLER1DSHADOW, SAMPLER2DSHADOW,
    SAMPLERCUBESHADOW, SAMPLER1DARRAY, SAMPLER2DARRAY, SAMPLER1DARRAYSHADOW,
    SAMPLER2DARRAYSHADOW, ISAMPLER1D, ISAMPLER2D, ISAMPLER3D, ISAMPLERCUBE,
    ISAMPLER1DARRAY, ISAMPLER2DARRAY, USAMPLER1D, USAMPLER2D, USAMPLER3D,
    USAMPLERCUBE, USAMPLER1DARRAY, USAMPLER2DARRAY, SAMPLER2DRECT,
    SAMPLER2DRECTSHADOW, ISAMPLER2DRECT, USAMPLER2DRECT, SAMPLERBUFFER,
    ISAMPLERBUFFER, USAMPLERBUFFER, SAMPLER2DMS, ISAMPLER2DMS, USAMPLER2DMS,
    SAMPLER2DMSARRAY, ISAMPLER2DMSARRAY, USAMPLER2DMSARRAY, VOID, BOOL,
    FLOAT, INT, UINT, STRUCT };

  static const int qualifier_tokens[] = { CENTROID, IN, OUT, INOUT, UNIFORM,
    VARYING, NOPERSPECTIVE, FLAT, SMOOTH, LAYOUT, ATTRIBUTE, CONST, INVARIANT,
    HIGH_PRECISION, MEDIUM_PRECISION, LOW_PRECISION, PRECISION };

  static const int keyword_tokens[] = { WHILE, BREAK, CONTINUE, DO, ELSE, FOR,
    IF, DISCARD, RETURN, SWITCH, CASE, DEFAULT };

  static const int constant_tokens[] = { FLOATCONSTANT, INTCONSTANT,
    UINTCONSTANT, TRUECONSTANT, FALSECONSTANT };

  static const int operator_tokens[] = { LEFT_OP, RIGHT_OP, INC_OP, DEC_OP,
    LE_OP, GE_OP, EQ_OP, NE_OP, AND_OP, OR_OP, XOR_OP, MUL_ASSIGN, DIV_ASSIGN,
    ADD_ASSIGN, LEFT_ASSIGN, RIGHT_ASSIGN, AND_ASSIGN, XOR_ASSIGN, OR_ASSIGN,
    SUB_ASSIGN, MOD_ASSIGN, '.', ',', '+', '-', '!', '~', '*',  '/', '%', '&',
    '^', '|', '?', ':', '=', ';' };

  static const int parenthesis_tokens[] = { '(', ')', '[', ']', '<', '>', '{',
    '}' };

  static const int error_tokens[] = { ERROR };

  static const int id_tokens[] = { IDENTIFIER };
}

enum SyntaxType { SERROR = 0, SCOMMENT, SPP, STYPE, SQUALIFIER, SKEYWORDS, SCONSTANTS,
  SOPERATORS, SPARENTHESES, SBUILDIN_VAR, SID };

static void set_tokens(std::vector<SyntaxType> &v, const int *tokens, size_t size, SyntaxType t) {
  for (size_t i = 0; i < size; i++) {
    int tmp = tokens[i];
    if (int(v.size()) <= tmp) v.resize(tmp+1);
    v[tmp] = t;
  }
}

SyntaxType getSyntaxType(int token) {
  using namespace SyntaxHL;
  static std::vector<SyntaxType> tokens;
  /// build the static mapping from any token to SyntaxType
  if (tokens.empty()) {
    set_tokens(tokens, comment_tokens, sizeof(comment_tokens)/sizeof(int), SCOMMENT);
    set_tokens(tokens, pp_tokens, sizeof(pp_tokens)/sizeof(int), SPP);
    set_tokens(tokens, type_tokens, sizeof(type_tokens)/sizeof(int), STYPE);
    set_tokens(tokens, qualifier_tokens, sizeof(qualifier_tokens)/sizeof(int), SQUALIFIER);
    set_tokens(tokens, keyword_tokens, sizeof(keyword_tokens)/sizeof(int), SKEYWORDS);
    set_tokens(tokens, constant_tokens, sizeof(constant_tokens)/sizeof(int), SCONSTANTS);
    set_tokens(tokens, operator_tokens, sizeof(operator_tokens)/sizeof(int), SOPERATORS);
    set_tokens(tokens, parenthesis_tokens, sizeof(parenthesis_tokens)/sizeof(int), SPARENTHESES);
    set_tokens(tokens, error_tokens, sizeof(error_tokens)/sizeof(int), SERROR);
    set_tokens(tokens, id_tokens, sizeof(id_tokens)/sizeof(int), SID);
  }
  return tokens[token];
}

Highlighter::Highlighter(QTextDocument *parent) : QSyntaxHighlighter(parent) {
  loadConfig();
}

Highlighter::~Highlighter() {
}

void Highlighter::loadConfig() {
  /// @todo All these should be loadable from some kind of config file run time,

  QTextCharFormat* formats[] = { &m_errorFormat, &m_commentFormat, &m_ppFormat,
    &m_typeFormat, &m_qualifierFormat, &m_keywordFormat, &m_constantFormat, &m_operatorFormat,
    &m_parenthesesFormat, &m_buildinVarFormat, &m_idFormat, 0 };
  QTextCharFormat** it = formats;
  while (*it) {
    QTextCharFormat* f = *it++;
    f->setForeground(Qt::black);
    //f->setBackground(Qt::white);
  }

  m_errorFormat.setUnderlineColor(Qt::red);
  m_errorFormat.setUnderlineStyle(QTextCharFormat::WaveUnderline);

  m_commentFormat.setFontItalic(true);
  m_commentFormat.setForeground(Qt::darkGray);

  m_ppFormat.setForeground(Qt::darkMagenta);

  m_typeFormat.setForeground(Qt::darkBlue);

  m_constantFormat.setForeground(QColor(100, 30, 30));

  m_keywordFormat.setFontWeight(QFont::Bold);

  m_qualifierFormat.setForeground(Qt::darkCyan);

  /* m_operatorFormat, m_parenthesesFormat, m_buildinVarFormat, m_idFormat */
}

void Highlighter::highlightBlock(const QString &text) {
  // Maps SyntaxType to format
  QTextCharFormat* formats[] = { &m_errorFormat, &m_commentFormat, &m_ppFormat,
    &m_typeFormat, &m_qualifierFormat, &m_keywordFormat, &m_constantFormat, &m_operatorFormat,
    &m_parenthesesFormat, &m_buildinVarFormat, &m_idFormat };

  QByteArray ascii = text.toAscii();

  glslset_scan_string(ascii.data(), ascii.size(), previousBlockState());
  int token;
  int start = 0;
  while ((token = glsllex_wrapper())) {
    if (token == COMMENT_BEGIN) {
      start = glslget_column() - glslget_leng();
    } else if (token == COMMENT_END) {
      int stop = glslget_column();
      setFormat(start, stop-start+1, *formats[SCOMMENT]);
    } else {
      size_t len = glslget_leng();
      setFormat(glslget_column() - len, len, *formats[getSyntaxType(token)]);
    }
  }
  if (glslget_state() == IN_COMMENT) {
    int stop = glslget_column();
    setFormat(start, stop-start+1, *formats[SCOMMENT]);
  }
  setCurrentBlockState(glslget_state());
}
