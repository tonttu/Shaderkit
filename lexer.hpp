#ifndef LEXER_HPP
#define LEXER_HPP

extern "C" {
#include "glsl.tab.hpp"

  void yyset_scan_string(const char *str, int len);

  int yylex_wrapper(void);
  int yyget_leng(void);
  int yyget_column(void);
  int yyget_line(void);
  int yyget_pos(void);
  char *yyget_text(void);
}

#endif // LEXER_HPP
