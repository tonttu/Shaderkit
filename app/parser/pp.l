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

%{
#include <cassert>

#include "parser/glpp.hpp"
#include "pp_yacc.h"

char* skip_(char* str, int s, int epos);
char* copystr(const char* str);
bool isTrueState(int state);

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

%x OBJ FUNC CTRL CALL FOBJ

 // "True" - Inside #if or #ifdef section that was evaluated to true
%x IF
 // "True" - Inside #else when all other tests were false
%x ELSE
 // "False, already found True" - Inside #if or #ifdef in a section that was
 //                               evaluated to false, one of the previous
 //                               sections has already been true
%x IF_SKIP
 // "False, looking for True" - Inside #if, #ifdef or #elif when all tests
 //                             so far have been evaluated to false
%x IF_SEEK
 // "False" - Inside #else when one of the previous tests was already true
%x ELSE_SKIP
%x EVAL IN_COMMENT PRAGMA

NEWLINE     \n
HSPACE      [ \t]
HASH        ^{HSPACE}*#{HSPACE}*
IDENTIFIER  [_a-zA-Z][_a-zA-Z0-9]*
NONID       [0-9]+[_a-zA-Z0-9]*|[^_a-zA-Z0-9\n/*]+
NONPASTE    [^_a-zA-Z0-9\n#/*]+
PASTEABLE   [_a-zA-Z0-9]+

%%

<IN_COMMENT>{
  "*/" { pop(); }
  [^*\r\n]+
  "*"
  {NEWLINE} { newline(); }
}

<*>{
  "/*" { changeState(true, IN_COMMENT); }
  "//".*
}

 /* Nested #if when processing ignored block */
<IF_SKIP,ELSE_SKIP,IF_SEEK>{
  {HASH}if.*{NEWLINE} { changeState(true, IF_SKIP); newline(); }
  .
  {NEWLINE} { newline(); }
}

 /* elif when we have already found true block */
<IF>{HASH}elif.*{NEWLINE} { changeState(false, IF_SKIP); newline(); }

 /* elif when we are still looking for a true block */
<IF_SEEK>{
  {HASH}elif { return HASH_ELIF; }
  {HASH}else.* { changeState(false, ELSE); }
}

 /* else when we have already found true block */
<IF>{HASH}else.*{NEWLINE} { changeState(false, ELSE_SKIP); newline(); }

<IF,IF_SEEK,IF_SKIP,ELSE,ELSE_SKIP>{HASH}endif.*{NEWLINE} { pop(); /* m_out.push_back('\n'); */ }

<INITIAL,IF,ELSE>{
  /* Nested or top-level #if when processing normal block */
  /** @todo most of these shouldn't actually use eval state */
  {HASH}ifndef { changeState(true, EVAL); return HASH_IFNDEF; }
  {HASH}ifdef { changeState(true, EVAL); return HASH_IFDEF; }
  {HASH}if { changeState(true, EVAL); return HASH_IF; }
  {HASH}version { changeState(true, EVAL); return HASH_VERSION; }
  {HASH}extension { changeState(true, EVAL); return HASH_EXTENSION; }
  {HASH}pragma { changeState(true, PRAGMA); return HASH_PRAGMA; }
  {HASH}include.* { return HASH_INCLUDE; }
  {HASH}line{HSPACE}.*{NEWLINE} { return HASH_LINE; newline(); }
   /* {HASH}line{HSPACE}+{DIGITS}{HSPACE}*$ { return HASH_LINE; } */
  {HASH}{NEWLINE} { newline(); }

  {HASH}error.*{NEWLINE} {
    skip2(5, 1);
    return HASH_ERROR;
  }

  {HASH}define{HSPACE}+{IDENTIFIER}{HSPACE}* {
    changeState(true, OBJ);
    skip2(6, 0);
    return DEFINE_OBJ;
  }

  {HASH}define{HSPACE}+{IDENTIFIER}"(" {
    changeState(true, FUNC);
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
  {NEWLINE} { pop(); newline(); return NL; }
  . { assert(false); }
}

<FOBJ>{
  {NONID} { yylval->string = copystr(yytext); return CHUNK; }
  [/*] { yylval->string = copystr(yytext); return CHUNK; }
  {IDENTIFIER} { yylval->string = copystr(yytext); return IDENTIFIER; }
  {NEWLINE} { pop(); newline(); return NL; }
}

<FUNC>{
  ")"{HSPACE}* { changeState(false, FOBJ); return ')'; }
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
      changeState(true, CALL);
      yylval->string = copystr(yytext);
      return EXPAND_FUNC;
    }
    Objs::iterator it = m_objs.find(yytext);
    if (it != m_objs.end()) {
      push_string(yytext, it->second.c_str());
    } else {
      if (m_undefs.count(yytext) == 0) m_require.insert(yytext);
      ECHO;
    }
  /*}*/
}
<CTRL,INITIAL,IF,ELSE>{IDENTIFIER} {
  Objs::iterator it = m_objs.find(yytext);
  if (it != m_objs.end() /*&& std::find(m_stack.begin(), m_stack.end(), yytext) == m_stack.end()*/) {
    push_string(yytext, it->second.c_str());
  } else {
    if (m_undefs.count(yytext) == 0) m_require.insert(yytext);
    ECHO;
  }
}

<CTRL>{
     /* {IDENTIFIER} { yylval->string = copystr(yytext); return IDENTIFIER; } */
  <<EOF>> {
    /* m_stack.pop_back(); */
    std::pair<MacroValue*, int> pair = m_macro_stack.top();
    pair.first->value = m_out.substr(pair.second);
    m_macro_stack.pop();
    if (m_macro_stack.empty())
      m_macros[m_macro_line].push_back(m_current_macro);
    pppop_buffer_state(yyscanner);
    // We need to do m_macro_stack.pop() before calling pop(), since pop()
    // will read the size of the m_macro_stack.
    pop();
  }
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
  {NEWLINE} { pop(); newline(); return NL; }
  . { fprintf(stderr, "Extra garbage: '%s'\n", yytext); }
}

<PRAGMA>{
  {NEWLINE} { pop(); newline(); return NL; }
  [^\n/*]+ { yylval->string = copystr(yytext); return CHUNK; }
  . { yylval->string = copystr(yytext); return CHUNK; }
}

<*>{
  {NEWLINE} { newline(); }
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

bool isTrueState(int state) {
  return state != IF_SKIP && state != IF_SEEK && state != ELSE_SKIP;
}

void GLpp::pp_return(bool push, bool b) {
  changeState(push, b ? IF : IF_SEEK);
}

void GLpp::push_string(const char* name, const char* str) {
  // m_stack.push_back(name);
  //Log::info("push: %d (%s)", line(), str);
  if (m_macro_stack.empty()) {
    m_current_macro = MacroValue(name, str);
    m_macro_line = line();
    m_macro_stack.push(std::make_pair(&m_current_macro, m_out.size()));
  } else {
    m_macro_stack.top().first->children.push_back(MacroValue(name, str));
    m_macro_stack.push(std::make_pair(&m_macro_stack.top().first->children.back(), m_out.size()));
  }
  struct yyguts_t* yyg = (struct yyguts_t*)yyscanner;
  pppush_buffer_state(YY_CURRENT_BUFFER, yyscanner);
  pp_scan_string(str, yyscanner);
  ppset_lineno(1, yyscanner);
  yy_push_state(CTRL, yyscanner);
}

void GLpp::pop() {
  yy_pop_state(yyscanner);
  struct yyguts_t* yyg = (struct yyguts_t*)yyscanner;
  fillLineValues(YY_START);
}

int GLpp::line() const {
  return ppget_lineno(yyscanner);
}

int GLpp::column() const {
  return ppget_column(yyscanner);
}

void GLpp::newline() {
  m_out.push_back('\n');
  ppset_lineno(line()+1, yyscanner);
}

void GLpp::changeState(bool push, int state) {
  fillLineValues(state);
  if(push) {
    yy_push_state(state, yyscanner);
  } else {
    struct yyguts_t* yyg = (struct yyguts_t*)yyscanner;
    BEGIN(state);
  }
}

void GLpp::fillLineValues(int new_state) {
  if (new_state == -1) new_state = INITIAL;
  if (m_macro_stack.empty()) {
    for (int i = m_line_values.size(), l = line(); i < l; ++i)
      m_line_values.push_back(m_last_status);
    m_last_status = isTrueState(new_state);
  }
}
