%{
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

#include <cassert>

#include "shader/glpp.hpp"
#include "pp_yacc.h"

char* skip_(char* str, int s, int epos);
char* copystr(const char* str);

#define skip(s, e) yylval->string = skip_((yytext), (s), (yyleng)-(e))
#define skip2(s, e) yylval->string = skip_((yytext), (s), -(yyleng - (e)))

#define YY_DECL int GLpp::lex(YYSTYPE* yylval_param)
#define ECHO m_out.append(yytext, yyleng)
%}
 /* Do not call ppwrap() at the EOF
    (meant to select the next file, maybe when using #include etc) */
%option noyywrap

%option reentrant


%option bison-bridge
%option stack
%option never-interactive
%option header-file="pp_lex.hpp"

%x OBJ FUNC CTRL CALL FOBJ IF ELSE IF_SKIP IF_SEEK ELSE_SKIP EVAL IN_COMMENT

NEWLINE     \n
HSPACE      [ \t]
HASH        ^{HSPACE}*#{HSPACE}*
IDENTIFIER  [_a-zA-Z][_a-zA-Z0-9]*
NONID       [0-9]+[_a-zA-Z0-9]*|[^_a-zA-Z0-9\n/*]+
NONPASTE    [^_a-zA-Z0-9\n#/*]+
PASTEABLE   [_a-zA-Z0-9]+

%%

<IN_COMMENT>{
  "*/" { yy_pop_state(yyscanner); }
  [^*\r\n]+
  "*"
  {NEWLINE} { m_out.push_back('\n'); }
}

<*>{
  "/*" { yy_push_state(IN_COMMENT, yyscanner); }
  "//".*
}

 /* Nested #if when processing ignored block */
<IF_SKIP,ELSE_SKIP,IF_SEEK>{
  {HASH}if.*{NEWLINE} { yy_push_state(IF_SKIP, yyscanner); m_out.push_back('\n'); }
  .
  {NEWLINE} { m_out.push_back('\n'); }
}

 /* elif when we have already found true block */
<IF>{HASH}elif.*{NEWLINE} { BEGIN(IF_SKIP); m_out.push_back('\n'); }

 /* elif when we are still looking for a true block */
<IF_SEEK>{
  {HASH}elif { return HASH_ELIF; }
  {HASH}else.* { BEGIN(ELSE); }
}

 /* else when we have already found true block */
<IF>{HASH}else.*{NEWLINE} { BEGIN(ELSE_SKIP); m_out.push_back('\n'); }

<IF,IF_SEEK,IF_SKIP,ELSE,ELSE_SKIP>{HASH}endif.*{NEWLINE} { yy_pop_state(yyscanner); /* m_out.push_back('\n'); */ }

<INITIAL,IF,ELSE>{
  /* Nested or top-level #if when processing normal block */
  /** @todo most of these shouldn't actually use eval state */
  {HASH}ifndef { yy_push_state(EVAL, yyscanner); return HASH_IFNDEF; }
  {HASH}ifdef { yy_push_state(EVAL, yyscanner); return HASH_IFDEF; }
  {HASH}if { yy_push_state(EVAL, yyscanner); return HASH_IF; }
  {HASH}version { yy_push_state(EVAL, yyscanner); return HASH_VERSION; }
  {HASH}extension { yy_push_state(EVAL, yyscanner); return HASH_EXTENSION; }
  {HASH}pragma.* { return HASH_PRAGMA; }
  {HASH}include.* { return HASH_INCLUDE; }
  {HASH}line{HSPACE}.*{NEWLINE} { return HASH_LINE; m_out.push_back('\n'); }
   /* {HASH}line{HSPACE}+{DIGITS}{HSPACE}*$ { return HASH_LINE; } */
  {HASH}{NEWLINE} { m_out.push_back('\n'); }

  {HASH}error.*{NEWLINE} {
    skip2(5, 1);
    return HASH_ERROR;
  }

  {HASH}define{HSPACE}+{IDENTIFIER}{HSPACE}* {
    yy_push_state(OBJ, yyscanner);
    skip2(6, 0);
    return DEFINE_OBJ;
  }

  {HASH}define{HSPACE}+{IDENTIFIER}"(" {
    yy_push_state(FUNC, yyscanner);
    skip(6, 1);
    return DEFINE_FUNC;
  }

  {HASH}undef{HSPACE}+{IDENTIFIER}{HSPACE}*{NEWLINE} {
    skip2(5, 1);
    return UNDEF;
  }
}

<OBJ>{
  {NONPASTE} { yylval->string = copystr(yytext); return CHUNK; }
  [ \t]*"##"[ \t]* { return PASTE; }
  [#/*] { yylval->string = copystr(yytext); return CHUNK; }
  {PASTEABLE} { yylval->string = copystr(yytext); return IDENTIFIER; }
  {NEWLINE} { yy_pop_state(yyscanner); m_out.push_back('\n'); return NL; }
  . { assert(false); }
}

<FOBJ>{
  {NONID} { yylval->string = copystr(yytext); return CHUNK; }
  [/*] { yylval->string = copystr(yytext); return CHUNK; }
  {IDENTIFIER} { yylval->string = copystr(yytext); return IDENTIFIER; }
  {NEWLINE} { yy_pop_state(yyscanner); m_out.push_back('\n'); return NL; }
}

<FUNC>{
  ")"{HSPACE}* { BEGIN(FOBJ); return ')'; }
  "," { return ','; }

  {IDENTIFIER} { yylval->string = copystr(yytext); return IDENTIFIER; }
  [\t ]+
  \n { fprintf(stderr, "ERROR\n"); }
  . { fprintf(stderr, "ERROR2\n"); }
}

<CTRL,INITIAL,IF,ELSE>{IDENTIFIER}/{HSPACE}*"(" {
  /*if (std::find(m_stack.begin(), m_stack.end(), yytext) != m_stack.end()) {
    printf("%s", yytext);
  } else {*/
    if (m_funcs.count(yytext) > 0) {
      yy_push_state(CALL, yyscanner);
      yylval->string = copystr(yytext);
      return EXPAND_FUNC;
    }
    Objs::iterator it = m_objs.find(yytext);
    if (it != m_objs.end()) {
      push_string(yytext, it->second.c_str());
    } else ECHO;
  /*}*/
}
<CTRL,INITIAL,IF,ELSE>{IDENTIFIER} {
  Objs::iterator it = m_objs.find(yytext);
  if (it != m_objs.end() /*&& std::find(m_stack.begin(), m_stack.end(), yytext) == m_stack.end()*/) {
    push_string(yytext, it->second.c_str());
  } else ECHO;
}

<CTRL>{
     /* {IDENTIFIER} { yylval->string = copystr(yytext); return IDENTIFIER; } */
  <<EOF>> { pppop_buffer_state(yyscanner); yy_pop_state(yyscanner); /* m_stack.pop_back(); */ }
}

<CALL>{
  [(),] { return yytext[0]; }
  /** @todo comments doesn't work correctly here */
  [^\n(),]+ { yylval->string = copystr(yytext); return ARG; }
  \n { fprintf(stderr, "ERROR3\n"); }
  . { fprintf(stderr, "ERROR4\n"); }
}

<EVAL>{
  [1-9][0-9]*[uU]? { yylval->integer = strtoll(yytext, NULL, 10); return DECIMAL; }
  0[0-7]*[uU]? { yylval->integer = strtoll(yytext, NULL, 8); return DECIMAL; }
  0[xX][0-9a-fA-F]+[uU]? { yylval->integer = strtoll(yytext + 2, NULL, 16); return DECIMAL; }

  "<<" { return LEFT_SHIFT; }
  ">>" { return RIGHT_SHIFT; }
  "<=" { return LESS_OR_EQUAL; }
  ">=" { return GREATER_OR_EQUAL; }
  "==" { return EQUAL; }
  "!=" { return NOT_EQUAL; }
  "&&" { return AND; }
  "||" { return OR; }
  "##" { return PASTE; }
  "defined" { return DEFINED; }

  [|^&<>+*/%!~():-] { return yytext[0]; }

  "require" { return REQUIRE; }
  "enable" { return ENABLE; }
  "warn" { return WARN; }
  "disable" { return DISABLE; }

  {IDENTIFIER} { yylval->string = copystr(yytext); return IDENTIFIER; }

  [ \t]+
  {NEWLINE} { yy_pop_state(yyscanner); m_out.push_back('\n'); return NL; }
  . { fprintf(stderr, "Extra garbage: '%s'\n", yytext); }
}

%%

char* skip_(char* str, int s, int epos) {
  if (epos >= 0) {
    str[epos] = '\0';
  } else {
    int i = -epos - 1;
    while (i > 0 && (str[i] == ' ' || str[i] == '\t')) --i;
    str[i+1] = '\0';
  }

  while (*str == ' ' || *str == '\t' || *str == '#')
    ++str;
  str += s;
  while (*str == ' ' || *str == '\t')
    ++str;
  return copystr(str);
}

char* copystr(const char* str) {
  return strdup(str);
}

void GLpp::pp_return(bool push, bool b) {
  if (push) {
    yy_push_state(b ? IF : IF_SEEK, yyscanner);
  } else {
    struct yyguts_t* yyg = (struct yyguts_t*)yyscanner;
    BEGIN(b ? IF : IF_SEEK);
  }
}

void GLpp::push_string(const char* name, const char* str) {
  // m_stack.push_back(name);
  struct yyguts_t* yyg = (struct yyguts_t*)yyscanner;
  pppush_buffer_state(YY_CURRENT_BUFFER, yyscanner);
  pp_scan_string(str, yyscanner);
  yy_push_state(CTRL, yyscanner);
}

void GLpp::pop() {
  yy_pop_state(yyscanner);
}
