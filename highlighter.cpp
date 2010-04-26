#include "highlighter.hpp"
#include "lexer.hpp"

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

void Highlighter::loadConfig() {
  /// @todo All these should be loadable from some kind of config file run time,
  //        maybe with the help of setProperty and QVariant.

  errorFormat.setUnderlineColor(Qt::red);
  errorFormat.setUnderlineStyle(QTextCharFormat::WaveUnderline);

  commentFormat.setFontItalic(true);
  commentFormat.setForeground(Qt::darkGray);

  ppFormat.setForeground(Qt::darkMagenta);

  typeFormat.setForeground(Qt::darkBlue);

  constantFormat.setForeground(QColor(100, 30, 30));

  keywordFormat.setFontWeight(QFont::Bold);

  qualifierFormat.setForeground(Qt::darkCyan);

  /* operatorFormat, parenthesesFormat, buildinVarFormat, idFormat */
}

void Highlighter::highlightBlock(const QString &text) {
  // Maps SyntaxType to format
  static QTextCharFormat* formats[] = { &errorFormat, &commentFormat, &ppFormat, &typeFormat,
    &qualifierFormat, &keywordFormat, &constantFormat, &operatorFormat, &parenthesesFormat,
    &buildinVarFormat, &idFormat };

  std::string ascii = text.toStdString();
  yyset_scan_string(ascii.c_str(), ascii.size());

  int token;
  while ((token = yylex_wrapper())) {
    size_t len = yyget_leng();
    setFormat(yyget_column() - len, len, *formats[getSyntaxType(token)]);
  }
}
