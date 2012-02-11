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

// #define PP_DEBUG 1

char* skip_(char* str, int s, int epos);
char* copystr(const char* str);
bool isTrueState(int state);

const char* stateName(int state);
const char* tokenName(int token);

void yy_push_state_debug(int state, yyscan_t scanner);

#define skip(s, e) yylval->string = copystr(skip_((yytext), (s), (yyleng)-(e)))
#define skip2(s, e) yylval->string = copystr(skip_((yytext), (s), -(yyleng - (e))))

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

%x OBJ FUNC CTRL CALL0 CALL1 CALL2 FOBJ CTRL_EVAL

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
  {HASH}elif { changeState(true, EVAL); return HASH_ELIF; }
  {HASH}else.* { changeState(false, ELSE); }
}

 /* else when we have already found true block */
<IF>{HASH}else.*{NEWLINE} { changeState(false, ELSE_SKIP); newline(); }

<IF,IF_SEEK,IF_SKIP,ELSE,ELSE_SKIP>{HASH}endif.*{NEWLINE} { newline(); pop(); }

<INITIAL,IF,ELSE>{
  /* Nested or top-level #if when processing normal block */
  /** @todo most of these shouldn't actually use eval state */
  {HASH}ifndef{HSPACE}+{IDENTIFIER}{HSPACE}*{NEWLINE} {
    skip2(6, 1);
    newline();
    return HASH_IFNDEF;
  }
  {HASH}ifdef{HSPACE}+{IDENTIFIER}{HSPACE}*{NEWLINE} {
    skip2(6, 1);
    newline();
    return HASH_IFDEF;
  }
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
    newline();
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
    newline();
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


<EVAL,CTRL_EVAL>{
  "defined"{HSPACE}*"("{HSPACE}*{IDENTIFIER}{HSPACE}*")" {
    char* tmp = yytext + 7;
    int i = yyleng-8;
    while (i > 0 && (tmp[i] == ' ' || tmp[i] == '\t' || tmp[i] == ')')) --i;
    tmp[i+1] = '\0';
    while (*tmp == ' ' || *tmp == '\t' || *tmp == '(') ++tmp;
    bool found = m_objs.count(tmp) > 0 || m_funcs.count(tmp) > 0;
    if (!found && m_undefs.count(tmp) == 0) m_require.insert(tmp);
    yylval->integer = !!found; return DECIMAL;
  }
  "defined"{HSPACE}*{IDENTIFIER} {
    char* tmp = yytext + 7;
    while (*tmp == ' ' || *tmp == '\t') ++tmp;
    bool found = m_objs.count(tmp) > 0 || m_funcs.count(tmp) > 0;
    if (!found && m_undefs.count(tmp) == 0) m_require.insert(tmp);
    yylval->integer = !!found; return DECIMAL;
  }
  "require" { return REQUIRE; }
  "enable" { return ENABLE; }
  "warn" { return WARN; }
  "disable" { return DISABLE; }
}

<CTRL,CTRL_EVAL,EVAL,INITIAL,IF,ELSE>{IDENTIFIER}/{HSPACE}*"(" {
  /*if (std::find(m_stack.begin(), m_stack.end(), yytext) != m_stack.end()) {
    printf("%s", yytext);
  } else {*/
    if (m_funcs.count(yytext) > 0) {
      changeState(true, CALL0);
      m_func = yytext;
      m_args.clear();
      m_txt.clear();
    } else {
      Objs::iterator it = m_objs.find(yytext);
      if (it != m_objs.end()) {
        push_string(yytext, it->second.c_str());
      } else {
        if (m_undefs.count(yytext) == 0) m_require.insert(yytext);
        if ((YY_START) == CTRL_EVAL || (YY_START) == EVAL) {
          fprintf(stderr, "Not found: '%s' %d\n", yytext, m_macro_stack.size());
          yylval->integer = 0; return DECIMAL;
        } else {
          ECHO;
        }
      }
    }
  /*}*/
}
<CTRL,CTRL_EVAL,EVAL,INITIAL,IF,ELSE>{IDENTIFIER} {
  Objs::iterator it = m_objs.find(yytext);
  if (it != m_objs.end() /*&& std::find(m_stack.begin(), m_stack.end(), yytext) == m_stack.end()*/) {
    fprintf(stderr, "pushing %s -> %s\n", yytext, it->second.c_str());
    push_string(yytext, it->second.c_str());
  } else {
    if (m_undefs.count(yytext) == 0) m_require.insert(yytext);
    if ((YY_START) == CTRL_EVAL || (YY_START) == EVAL) {
      fprintf(stderr, "Not found: '%s' %d\n", yytext, m_macro_stack.size());
      yylval->integer = 0; return DECIMAL;
    } else {
      ECHO;
    }
  }
}

<CTRL,CTRL_EVAL>{
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

<CALL0>{
  {HSPACE}*"(" { changeState(false, CALL1); }
  . { assert(false); }
}

<CALL1>{
  "(" { m_txt += yytext; m_depth = 1; changeState(true, CALL2); }
  "," { m_args.push_back(m_txt); m_txt = ""; }
  ")" {
    m_args.push_back(m_txt);
    fprintf(stderr, "Macro call %s of %lu arguments\n", m_func.c_str(), m_args.size());
    for (int i = 0; i < m_args.size(); ++i)
      fprintf(stderr, "Arg %d: '%s'\n", i, m_args[i].c_str());
    GLpp::Func& f = m_funcs[m_func];
    std::string str;
    str.reserve(128);
    for (size_t i = 0; i < f.chunks.size(); ++i) {
      std::pair<std::string, int>& p = f.chunks[i];
      if (p.second == -1) {
        str += p.first;
      } else {
        if (m_args.size() >= size_t(p.second+1)) {
          str += m_args[p.second];
        } else {
          fprintf(stderr, "ERROR ERROR\n");
        }
      }
    }
    pop();
    push_string(m_func.c_str(), str.c_str());
  }
  [^\n(),]+ { m_txt += yytext; }
  \n { fprintf(stderr, "ERRERRERR\n"); }
}

<CALL2>{
  "(" { m_txt += yytext; ++m_depth; }
  ")" { m_txt += yytext; if (--m_depth == 0) pop(); }
  [^\n()]+ { m_txt += yytext; }
  \n { fprintf(stderr, "ERRERRERR 2\n"); }
}

<EVAL,CTRL_EVAL>{
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

  [|^&<>+*/%!~():-] { return yytext[0]; }

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
  return str;
}

char* copystr(const char* str) {
  return strdup(str);
}

bool isTrueState(int state) {
  return state != IF_SKIP && state != IF_SEEK && state != ELSE_SKIP;
}

const char* stateName(int state) {
  static int lst[] = {INITIAL, OBJ, FUNC, CTRL, CALL0, CALL1, CALL2, FOBJ, CTRL_EVAL,
    IF, ELSE, IF_SKIP, IF_SEEK, ELSE_SKIP, EVAL, IN_COMMENT, PRAGMA};
  static const char* names[] = {"INITIAL", "OBJ", "FUNC", "CTRL", "CALL0", "CALL1", "CALL2", "FOBJ", "CTRL_EVAL",
    "IF", "ELSE", "IF_SKIP", "IF_SEEK", "ELSE_SKIP", "EVAL", "IN_COMMENT", "PRAGMA"};
  int* end = lst + sizeof(lst)/sizeof(lst[0]);
  int* it = std::find(lst, end, state);
  return it == end ? "<?>" : names[it - lst];
}

const char* tokenName(int token) {
  static int lst[] = {HASH_ERROR, DEFINE_OBJ, DEFINE_FUNC, DATA, IDENTIFIER,
    ARG, CHUNK, PASTE, UNDEF, HASH_PRAGMA, DECIMAL, NL,
    HASH_VERSION, HASH_EXTENSION, HASH_INCLUDE, HASH_LINE, HASH_IFNDEF,
    HASH_IFDEF, HASH_IF, HASH_ELIF, REQUIRE, DISABLE, WARN, ENABLE, OR, AND,
    EQUAL, NOT_EQUAL, LESS_OR_EQUAL, GREATER_OR_EQUAL, LEFT_SHIFT, RIGHT_SHIFT,
    UNARY};
  static const char* names[] = {"HASH_ERROR", "DEFINE_OBJ", "DEFINE_FUNC",
    "DATA", "IDENTIFIER", "ARG", "CHUNK", "PASTE", "UNDEF",
    "HASH_PRAGMA", "DECIMAL", "NL", "HASH_VERSION", "HASH_EXTENSION",
    "HASH_INCLUDE", "HASH_LINE", "HASH_IFNDEF", "HASH_IFDEF", "HASH_IF",
    "HASH_ELIF", "REQUIRE", "DISABLE", "WARN", "ENABLE", "OR", "AND", "EQUAL",
    "NOT_EQUAL", "LESS_OR_EQUAL", "GREATER_OR_EQUAL", "LEFT_SHIFT",
    "RIGHT_SHIFT", "UNARY"};
  int* end = lst + sizeof(lst)/sizeof(lst[0]);
  int* it = std::find(lst, end, token);
  return it == end ? 0 : names[it - lst];
}

void pp_push_state(int state, yyscan_t scanner) {
#if PP_DEBUG
  struct yyguts_t* yyg = (struct yyguts_t*)scanner;
  fprintf(stderr, "PUSH %s -> %s\n", stateName(YY_START), stateName(state));
#endif
  yy_push_state(state, scanner);
}

void GLpp::pp_return(bool push, bool b) {
  changeState(push, b ? IF : IF_SEEK);
  m_line_values[line()-1] = b;
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
  pp_push_state(YY_START == EVAL || YY_START == CTRL_EVAL ? CTRL_EVAL : CTRL, yyscanner);
}

void GLpp::pop() {
  struct yyguts_t* yyg = (struct yyguts_t*)yyscanner;
  int from = YY_START;
  yy_pop_state(yyscanner);
#ifdef PP_DEBUG
  fprintf(stderr, "POP  %s -> %s\n", stateName(from), stateName(YY_START));
#endif
  fillLineValues(YY_START);
}

int GLpp::line() const {
  return ppget_lineno(yyscanner);
}

int GLpp::column() const {
  return ppget_column(yyscanner);
}

int GLpp::lex_debug(YYSTYPE* lvalp) {
  int token = lex(lvalp);
  const char* t = tokenName(token);
  if (t) fprintf(stderr, "Token: %s\n", t);
  else fprintf(stderr, "Token: %c\n", token);
  return token;
}

void GLpp::newline() {
  m_out.push_back('\n');
  ppset_lineno(line()+1, yyscanner);
}

void GLpp::changeState(bool push, int state) {
  fillLineValues(state);
  if(push) {
    pp_push_state(state, yyscanner);
  } else {
    struct yyguts_t* yyg = (struct yyguts_t*)yyscanner;
#ifdef PP_DEBUG
    fprintf(stderr, "CHANGE %s -> %s\n", stateName(YY_START), stateName(state));
#endif
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
