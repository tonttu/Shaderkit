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

#include "shader/glpp.hpp"
#include "pp_lex.hpp"

%}

// Create a header
%defines

// Do not use global variables (reentrant parser)
%define api.pure

%parse-param { GLpp& parser }

%union {
  const char* string;
  std::string* stdstr;
  long integer;
  std::vector<std::string>* strlst;
  std::map<std::string, int>* tokenmap;
  std::list<std::pair<std::string, int> >* tokenlst;
}

%debug
%error-verbose

%{
#undef yylex
#undef yyerror
#define yylex parser.lex
#define yyerror parser.error

typedef std::pair<std::string, int> Token;
typedef std::list<Token> TokenList;

%}

%token <string> HASH_ERROR DEFINE_OBJ DEFINE_FUNC DATA IDENTIFIER EXPAND_FUNC ARG CHUNK PASTE UNDEF
%token <integer> DECIMAL
%token NL

%token HASH_VERSION HASH_EXTENSION HASH_PRAGMA HASH_INCLUDE HASH_LINE
%token HASH_IFNDEF HASH_IFDEF HASH_IF HASH_ELIF
%token REQUIRE DISABLE WARN ENABLE

%type <tokenmap> identifier_list identifier_list2
%type <strlst> list
%type <tokenlst> data
%type <stdstr> arg arg2 parg obj_data obj_data2
%type <integer> expression

%left OR
%left AND
%left '|'
%left '^'
%left '&'
%left EQUAL NOT_EQUAL
%left '<' '>' LESS_OR_EQUAL GREATER_OR_EQUAL
%left LEFT_SHIFT RIGHT_SHIFT
%left '+' '-'
%left '*' '/' '%'
%right UNARY DEFINED

%%

input
  : /* empty */
  | stuff input
  ;

stuff
  : HASH_VERSION DECIMAL IDENTIFIER NL {
    fprintf(stderr, "Got version %ld with profile %s\n", $2, $3);
  }
  | HASH_VERSION DECIMAL NL {
    fprintf(stderr, "Got version %ld with no profile info\n", $2);
  }
  | HASH_EXTENSION IDENTIFIER ':' REQUIRE NL {
  }
  | HASH_EXTENSION IDENTIFIER ':' ENABLE NL {
  }
  | HASH_EXTENSION IDENTIFIER ':' WARN NL {
  }
  | HASH_EXTENSION IDENTIFIER ':' DISABLE NL {
  }
  | HASH_PRAGMA
  | HASH_INCLUDE
  | HASH_LINE
  | HASH_IF expression NL {
    parser.pp_return(true, $2);
  }
  | HASH_ELIF expression NL {
    parser.pp_return(false, $2);
  }
  | HASH_IFDEF IDENTIFIER NL {
    parser.pp_return(true, parser.m_objs.count($2) > 0 || parser.m_funcs.count($2) > 0);
  }
  | HASH_IFNDEF IDENTIFIER NL {
    parser.pp_return(true, parser.m_objs.count($2) == 0 && parser.m_funcs.count($2) == 0);
  }
  | HASH_ERROR {
    fprintf(stderr, "ERROR: %s\n", $1);
    return -1;
  }
  | DEFINE_OBJ obj_data NL {
    parser.m_objs[$1] = $2 ? *$2 : "";
    fprintf(stderr, "Defined '%s' as '%s'\n", $1, $2 ? $2->c_str() : 0);
  }
  | DEFINE_FUNC identifier_list ')' data NL {
    GLpp::Func& f = parser.m_funcs[$1];
    f.chunks.reserve($4->size());
    for (TokenList::iterator it = $4->begin(); it != $4->end(); ++it) {
      if ($2 && it->second == IDENTIFIER) {
        std::map<std::string, int>::iterator it2 = $2->find(it->first);
        if (it2 != $2->end()) {
          fprintf(stderr, "ID #%d: '%s'\n", it2->second, it->first.c_str());
          f.chunks.push_back(std::make_pair(it->first, it2->second));
          continue;
        }
      }
      fprintf(stderr, "Chunk: '%s'\n", it->first.c_str());
      f.chunks.push_back(std::make_pair(it->first, -1));
    }
    fprintf(stderr, "Defined '%s', %lu parameters, %lu chunks\n", $1, $2 ? $2->size() : 0, f.chunks.size());
    delete $2;
    delete $4;
  }
  | EXPAND_FUNC '(' list ')' {
    parser.pop();
    fprintf(stderr, "Macro call %s of %lu arguments\n", $1, $3->size());

    GLpp::Func& f = parser.m_funcs[$1];
    std::string str;
    str.reserve(128);
    for (size_t i = 0; i < f.chunks.size(); ++i) {
      std::pair<std::string, int>& p = f.chunks[i];
      if (p.second == -1) {
        str += p.first;
      } else {
        if ($3->size() >= size_t(p.second+1)) {
          str += (*$3)[p.second];
        } else {
          fprintf(stderr, "ERROR ERROR\n");
        }
      }
    }
    parser.push_string($1, str.c_str());
    delete $3;
  }
  | UNDEF {
    parser.m_funcs.erase($1);
    parser.m_objs.erase($1);
  }
  ;

identifier_list
  : /* empty */ { $$ = 0; }
  | identifier_list2 { $$ = $1; }
  ;

identifier_list2
  : IDENTIFIER {
    $$ = new std::map<std::string, int>();
    (*$$)[$1] = 0;
  }
  | identifier_list ',' IDENTIFIER {
    $$ = $1;
    (*$$)[$3] = $$->size();
  }
  ;

list
  : arg { $$ = new std::vector<std::string>(); $$->push_back(*$1); delete $1; }
  | list ',' arg { $$ = $1; $$->push_back(*$3); delete $3; }
  ;

arg
  : parg { $$ = $1; }
  | ARG { $$ = new std::string($1); }
  | arg ARG { $$ = $1; (*$$) += $2; }
  | arg parg { $$ = $1; (*$$) += *$2; delete $2; }
  ;

parg
  : '(' arg2 ')' {
    $$ = new std::string;
    (*$$) += '(';
    (*$$) += *$2;
    (*$$) += ')';
    delete $2;
  }
  ;

arg2
  : parg { $$ = $1; }
  | ARG { $$ = new std::string($1); }
  | ',' { $$ = new std::string(","); }
  | arg2 ARG { $$ = $1; (*$$) += $2; }
  | arg2 parg { $$ = $1; (*$$) += *$2; delete $2; }
  | arg2 ',' { $$ = $1; (*$$) += ","; }
  ;

data
  : CHUNK {
    $$ = new TokenList();
    $$->push_back(Token($1, CHUNK));
  }
  | IDENTIFIER {
    $$ = new TokenList();
    $$->push_back(Token($1, IDENTIFIER));
  }
  | data PASTE { $$ = $1; $$->push_back(Token("", PASTE)); }
  | data CHUNK { $$ = $1; $$->push_back(Token($2, CHUNK)); }
  | data IDENTIFIER { $$ = $1; $$->push_back(Token($2, IDENTIFIER)); }
  ;

obj_data
  : /* empty */ { $$ = 0; }
  | obj_data2 obj_data { $$ = $1; if ($2) (*$$) += *$2; }
  | CHUNK obj_data { $$ = new std::string($1); if ($2) (*$$) += *$2; }
  ;

obj_data2
  : obj_data2 PASTE IDENTIFIER {
    $$ = $1;
    (*$$) += $3;
  }
  | IDENTIFIER { $$ = new std::string($1); }
  ;

/**
 * This rule is based on the similar one in Mesa. Thanks.
 */
expression
  : DECIMAL { $$ = $1; }
  | expression OR expression { $$ = $1 || $3; }
  | expression AND expression { $$ = $1 && $3; }
  | expression '|' expression { $$ = $1 | $3; }
  | expression '^' expression { $$ = $1 ^ $3; }
  | expression '&' expression { $$ = $1 & $3; }
  | expression NOT_EQUAL expression { $$ = $1 != $3; }
  | expression EQUAL expression { $$ = $1 == $3; }
  | expression GREATER_OR_EQUAL expression { $$ = $1 >= $3; }
  | expression LESS_OR_EQUAL expression { $$ = $1 <= $3; }
  | expression '>' expression { $$ = $1 > $3; }
  | expression '<' expression { $$ = $1 < $3; }
  | expression RIGHT_SHIFT expression { $$ = $1 >> $3; }
  | expression LEFT_SHIFT expression { $$ = $1 << $3; }
  | expression '-' expression { $$ = $1 - $3; }
  | expression '+' expression { $$ = $1 + $3; }
  | expression '%' expression {
    if ($3 == 0) {
      yyerror(parser, "zero modulus in preprocessor directive");
    } else {
      $$ = $1 % $3;
    }
  }
  | expression '/' expression {
    if ($3 == 0) {
      yyerror(parser, "division by 0 in preprocessor directive");
    } else {
      $$ = $1 / $3;
    }
  }
  | expression '*' expression { $$ = $1 * $3; }
  | '!' expression %prec UNARY { $$ = ! $2; }
  | '~' expression %prec UNARY { $$ = ~ $2; }
  | '-' expression %prec UNARY { $$ = - $2; }
  | '+' expression %prec UNARY { $$ = + $2; }
  | '(' expression ')' { $$ = $2; }
  ;

%%
