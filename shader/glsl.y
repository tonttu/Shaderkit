/**
 * Copyright 2010 Riku Palomäki.
 * This file is part of GLSL Lab.
 *
 * GLSL Lab is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GLSL Lab is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GLSL Lab.  If not, see <http://www.gnu.org/licenses/>.
 */

%{
  #include <cstdio>
  #include <string>
  #include <map>
  #include <set>
  #include <cassert>
  #include <iostream>

  extern "C" {
  	int yylex(void);
  	int yyparse(void);
  	void yyerror(const char* str);
  }

  struct Node {
    Node() : prev(0), next(0) {}
    virtual void print(int i = 0) = 0;
    std::ostream& indent(int level) {
      while (level-- > 0) std::cout << "\t";
      return std::cout;
    }
    Node *prev, *next;
  };
  template <typename T>
  T* link(T* a, T* b) {
    Node* tmp = b;
    assert(a);
    assert(b);
    while (a->next) a = static_cast<T*>(a->next);
    while (b->next) b = static_cast<T*>(b->next);
    while (tmp->prev) tmp = tmp->prev;
    a->next = tmp;
    tmp->prev = a;
    return b;
  }

  struct Jump : public Node {
    int type;
    Node* node;
    Jump(int t, Node* n = 0) : type(t), node(n) {}
    virtual void print(int i = 0) {
      //indent(i) << "Jump " << token_name(type) << std::endl;
      node->print(i+1);
    }
  };
  struct If : public Node {
    Node *expr, *stmt, *else_stmt;
    If(Node* e, Node* s, Node* el = 0) : expr(e), stmt(s), else_stmt(el) {}
    virtual void print(int i = 0) {
      indent(i) << "If" << std::endl;
      expr->print(i+1);
      indent(i) << "then" << std::endl;
      stmt->print(i+1);
      if (else_stmt) {
        indent(i) << "else" << std::endl;
        else_stmt->print(i+1);
      }
    }
  };
  struct Conditional : public Node {
    Node *test, *a, *b;
    Conditional(Node* t, Node* a_, Node* b_) : test(t), a(a_), b(b_) {}
    virtual void print(int i = 0) {
      indent(i) << "Conditional" << std::endl;
      a->print(i+1);
      b->print(i+1);
    }
  };
  struct Struct : public Node {
    virtual void print(int i = 0) {
      indent(i) << "Struct" << std::endl;
    }
  };
  struct Operator : public Node {
    int type;
    Node *a, *b;
    Operator(int t, Node* a_, Node* b_ = 0) : type(t), a(a_), b(b_) {}
    virtual void print(int i = 0) {
      //indent(i) << "Operator " << token_name(type) << std::endl;
      a->print(i+1);
      if (b) b->print(i+1);
    }
  };
  struct ArrayField : public Node {
    Node *node, *item;
    ArrayField(Node* n, Node* i) : node(n), item(i) {}
    virtual void print(int i = 0) {
      indent(i) << "ArrayField" << std::endl;
      node->print(i+1);
      indent(i) << "->" << std::endl;
      item->print(i+1);
    }
  };
  struct Field : public Node {
    Node* node;
    char* item;
    Field(Node* n, char* i) : node(n), item(i) {}
    virtual void print(int i = 0) {
      indent(i) << "Field '" << item << '\'' << std::endl;
      node->print(i+1);
    }
  };
  struct Variable : public Node {
    virtual void print(int i = 0) {
      indent(i) << "Variable" << std::endl;
    }
  };
  struct IntConstant : public Node {
    int type;
    long value;
    IntConstant(int t, long v) : type(t), value(v) {}
    virtual void print(int i = 0) {
      //indent(i) << "IntConstant " << token_name(type) << " '" << value << '\'' << std::endl;
    }
  };
  struct FloatConstant : public Node {
    float value;
    FloatConstant(float v) : value(v) {}
    virtual void print(int i = 0) {
      indent(i) << "FloatConstant '" << value << '\'' << std::endl;
    }
  };
  struct Qualifier : public Node {
    Qualifier() {}
    virtual void print(int i = 0) {
      indent(i) << "Qualifier" << std::endl;
    }
  };
  struct Type : public Node {
    std::string name;
    int precision;
    Qualifier* qualifier;
    Node* initializer;
    Node* array;
    bool is_array;
    Type() : precision(0), qualifier(0), initializer(0), array(0), is_array(false) {}
    virtual void print(int i = 0) {
      indent(i) << "Type" << std::endl;
    }
    void setPrecision(int p) { precision = p; }
    void setName(const char* n) { name = n; }
    void setIsArray(bool b = true) { is_array = b; }
    void setArray(Node* n) { is_array = true; array = n; }
    void setInitializer(Node* n) { initializer = n; }
    void setQualifier(Qualifier* q) { qualifier = q; }
  };
  struct Parameter : public Node {
    Type* type;
    std::string name;
    Node* array;
    int type_qualifier, qualifier;
    Parameter(Type* t, const char* str, Node* a = 0) : type(t), name(str), array(a), type_qualifier(0), qualifier(0) {}
    Parameter(Type* t) : type(t), array(0), type_qualifier(0), qualifier(0) {}
    virtual void print(int i = 0) {
      indent(i) << "Parameter" << std::endl;
    }
    void setTypeQualifier(int tq) { type_qualifier = tq; }
    void setQualifier(int pq) { qualifier = pq; }
  };
  struct Function : public Node {
    Type* type;
    std::string name;
    Parameter* params;
    Function(Type* t, const char* n) : type(t), name(n), params(0) {
    }
    virtual void print(int i = 0) {
      indent(i) << "Function" << std::endl;
    }
    void addParam(Parameter* p) {
      /* if (params == 0) params = p;
      else link(params, p); */
    }
  };
  struct Constructor : public Node {
    Type* type;
    Constructor(Type* t) : type(t) {}
    virtual void print(int i = 0) {
      indent(i) << "Constructor" << std::endl;
    }
  };
  struct Call: public Node {
    Call() {}
    virtual void print(int i = 0) {
      indent(i) << "Call" << std::endl;
    }
  };
  struct Layout : public Node {
    virtual void print(int i = 0) {
      indent(i) << "Layout" << std::endl;
    }
  };

  class SymTable {
  public:
    void add(const std::string &str, Variable*v) {
      assert(values.find(str) == values.end());
      values[str] = v;
    }
    Variable* get(const std::string &str) {
      std::map<std::string, Variable*>::iterator it = values.find(str);
      if (it != values.end()) return it->second;
      if (parent) return parent->get(str);
      assert(false);
      return 0;
    }
  protected:
    std::map<std::string, Variable*> values;
    SymTable* parent;
  };

  std::set<int>* makeSet(int a) {
    std::set<int>* set = new std::set<int>();
    set->insert(a);
    return set;
  }
  std::set<int>* makeSet(int a, int b) {
    std::set<int>* set = new std::set<int>();
    set->insert(a);
    set->insert(b);
    return set;
  }
  static Node* tree;
  typedef std::set<int> Set;
%}

%defines
%token-table
%union {
  void* sset;
  int integer;
  float f;
	char* string;
  struct Node* node;
  struct Function* func;
  struct Parameter* param;
  struct Type* type;
  struct Qualifier* qualifier;
  struct Layout* layout;
}
%token <integer> ATTRIBUTE CONST BOOL FLOAT INT UINT
%token <integer> BREAK CONTINUE DO ELSE FOR IF DISCARD RETURN SWITCH CASE DEFAULT
%token <integer> BVEC2 BVEC3 BVEC4 IVEC2 IVEC3 IVEC4 UVEC2 UVEC3 UVEC4 VEC2 VEC3 VEC4
%token <integer> MAT2 MAT3 MAT4 CENTROID IN OUT INOUT UNIFORM VARYING

%token <integer> NOPERSPECTIVE FLAT SMOOTH LAYOUT
%token <integer> MAT2X2 MAT2X3 MAT2X4
%token <integer> MAT3X2 MAT3X3 MAT3X4
%token <integer> MAT4X2 MAT4X3 MAT4X4
%token <integer> SAMPLER1D SAMPLER2D SAMPLER3D SAMPLERCUBE SAMPLER1DSHADOW SAMPLER2DSHADOW
%token <integer> SAMPLERCUBESHADOW SAMPLER1DARRAY SAMPLER2DARRAY SAMPLER1DARRAYSHADOW
%token <integer> SAMPLER2DARRAYSHADOW ISAMPLER1D ISAMPLER2D ISAMPLER3D ISAMPLERCUBE
%token <integer> ISAMPLER1DARRAY ISAMPLER2DARRAY USAMPLER1D USAMPLER2D USAMPLER3D
%token <integer> USAMPLERCUBE USAMPLER1DARRAY USAMPLER2DARRAY
%token <integer> SAMPLER2DRECT SAMPLER2DRECTSHADOW ISAMPLER2DRECT USAMPLER2DRECT
%token <integer> SAMPLERBUFFER ISAMPLERBUFFER USAMPLERBUFFER
%token <integer> SAMPLER2DMS ISAMPLER2DMS USAMPLER2DMS
%token <integer> SAMPLER2DMSARRAY ISAMPLER2DMSARRAY USAMPLER2DMSARRAY

%token <integer> STRUCT VOID WHILE

%token <string> IDENTIFIER
%token <f> FLOATCONSTANT
%token <integer> INTCONSTANT
%token <integer> UINTCONSTANT
%token <integer> LEFT_OP RIGHT_OP
%token <integer> INC_OP DEC_OP LE_OP GE_OP EQ_OP NE_OP
%token <integer> AND_OP OR_OP XOR_OP MUL_ASSIGN DIV_ASSIGN ADD_ASSIGN
%token <integer> LEFT_ASSIGN RIGHT_ASSIGN AND_ASSIGN XOR_ASSIGN OR_ASSIGN
%token <integer> SUB_ASSIGN MOD_ASSIGN

%token <integer> LEFT_PAREN RIGHT_PAREN LEFT_BRACKET RIGHT_BRACKET LEFT_BRACE RIGHT_BRACE DOT
%token <integer> COMMA COLON EQUAL SEMICOLON BANG DASH TILDE PLUS STAR SLASH PERCENT
%token <integer> LEFT_ANGLE RIGHT_ANGLE VERTICAL_BAR CARET AMPERSAND QUESTION

%token <integer> INVARIANT
%token <integer> HIGH_PRECISION MEDIUM_PRECISION LOW_PRECISION PRECISION

%token TRUECONSTANT FALSECONSTANT

%type <node> jump_statement selection_statement selection_rest_statement expression statement assignment_expression conditional_expression logical_or_expression logical_xor_expression logical_and_expression inclusive_or_expression exclusive_or_expression and_expression equality_expression relational_expression shift_expression additive_expression multiplicative_expression unary_expression postfix_expression translation_unit primary_expression integer_expression variable_identifier external_declaration function_definition declaration compound_statement statement_list compound_statement_no_new_scope constant_expression expression_statement initializer declaration_statement simple_statement function_call_header switch_statement switch_statement_list case_label iteration_statement expressionopt conditionopt condition statement_no_new_scope for_init_statement


%type <func> function_declarator function_header_with_parameters function_header function_prototype
%type <param> parameter_declaration parameter_declarator
%type <type> fully_specified_type type_specifier parameter_type_specifier type_specifier_nonarray struct_specifier type_specifier_no_prec struct_declarator struct_declarator_list struct_declaration struct_declaration_list single_declaration init_declarator_list
%type <sset> storage_qualifier
%type <layout> layout_qualifier layout_qualifier_id_list layout_qualifier_id
%type <qualifier> type_qualifier

%type <integer> unary_operator assignment_operator parameter_qualifier parameter_type_qualifier interpolation_qualifier invariant_qualifier precision_qualifier type_specifier_nonarray_simple

// bison declarations
// %token TYPE_NAME FIELD_SELECTION
%token BOOLCONSTANT PP COMMENT ERROR COMMENT_BEGIN COMMENT_END

// else
%expect 1

%start translation_unit

%%

// grammar rules

// ??
variable_identifier
	: IDENTIFIER { /*$$ = new Variable($1);*/ }
	;

primary_expression
	: variable_identifier
	| INTCONSTANT { $$ = new IntConstant(INTCONSTANT, $1); }
	| UINTCONSTANT { $$ = new IntConstant(UINTCONSTANT, $1); }
	| FLOATCONSTANT { $$ = new FloatConstant($1); }
	| TRUECONSTANT { $$ = new IntConstant(BOOLCONSTANT, 1); }
	| FALSECONSTANT { $$ = new IntConstant(BOOLCONSTANT, 0); }
	| '(' expression ')' { $$ = $2; }
	;

// ??
postfix_expression
	: primary_expression
	| postfix_expression '[' integer_expression ']' { $$ = new ArrayField($1, $3); }
	| function_call { $$ = 0; }
	| postfix_expression '.' IDENTIFIER { $$ = new Field($1, $3); } // FIELD_SELECTION
	| postfix_expression INC_OP { $$ = new Operator($2, $1); }
	| postfix_expression DEC_OP { $$ = new Operator($2, $1); }
	;

integer_expression
	: expression
	;

// ??
function_call
	: function_call_or_method
	;

// ??
function_call_or_method
	: function_call_generic
	| postfix_expression '.' function_call_generic
	;

// ??
function_call_generic
	: function_call_header_with_parameters ')'
	| function_call_header_no_parameters ')'
	;

// ??
function_call_header_no_parameters
	: function_call_header VOID
	| function_call_header
	;

// ??
function_call_header_with_parameters
	: function_call_header assignment_expression
	| function_call_header_with_parameters ',' assignment_expression
	;

function_call_header
//	: function_identifier '('
  : type_specifier '(' { $$ = new Constructor($1); }
  | IDENTIFIER '(' { $$ = new Call( /*$1 */); }
	;

// Grammar Note: Constructors look like functions, but lexical analysis recognized most of them as
// keywords. They are now recognized through “type_specifier”.

/*
function_identifier
	: type_specifier { $$ = new Constructor($1); }
	| IDENTIFIER
//	| FIELD_SELECTION
	;
*/

unary_expression
	: postfix_expression
	| INC_OP unary_expression { $$ = new Operator($1, $2); }
	| DEC_OP unary_expression { $$ = new Operator($1, $2); }
	| unary_operator unary_expression { $$ = new Operator($1, $2); }
	;

// Grammar Note: No traditional style type casts.

unary_operator
	: '+' { $$ = '+'; }
	| '-' { $$ = '-'; }
	| '!' { $$ = '!'; }
	| '~' { $$ = '~'; }
	;

// Grammar Note: No '*' or '&' unary ops. Pointers are not supported.

multiplicative_expression
	: unary_expression
	| multiplicative_expression '*' unary_expression { $$ = new Operator('*', $1, $3); }
	| multiplicative_expression '/' unary_expression { $$ = new Operator('/', $1, $3); }
	| multiplicative_expression '%' unary_expression { $$ = new Operator('%', $1, $3); }
	;

additive_expression
	: multiplicative_expression
	| additive_expression '+' multiplicative_expression { $$ = new Operator('+', $1, $3); }
	| additive_expression '-' multiplicative_expression { $$ = new Operator('-', $1, $3); }
	;

shift_expression
	: additive_expression
	| shift_expression LEFT_OP additive_expression { $$ = new Operator($2, $1, $3); }
	| shift_expression RIGHT_OP additive_expression { $$ = new Operator($2, $1, $3); }
	;

relational_expression
	: shift_expression
	| relational_expression '<' shift_expression { $$ = new Operator('<', $1, $3); }
	| relational_expression '>' shift_expression { $$ = new Operator('>', $1, $3); }
	| relational_expression LE_OP shift_expression { $$ = new Operator($2, $1, $3); }
	| relational_expression GE_OP shift_expression { $$ = new Operator($2, $1, $3); }
	;

equality_expression
	: relational_expression
	| equality_expression EQ_OP relational_expression { $$ = new Operator($2, $1, $3); }
	| equality_expression NE_OP relational_expression { $$ = new Operator($2, $1, $3); }
	;

and_expression
	: equality_expression
	| and_expression '&' equality_expression { $$ = new Operator('&', $1, $3); }
	;

exclusive_or_expression
	: and_expression
	| exclusive_or_expression '^' and_expression { $$ = new Operator('^', $1, $3); }
	;

inclusive_or_expression
	: exclusive_or_expression
	| inclusive_or_expression '|' exclusive_or_expression { $$ = new Operator('|', $1, $3); }
	;

logical_and_expression
	: inclusive_or_expression
	| logical_and_expression AND_OP inclusive_or_expression { $$ = new Operator($2, $1, $3); }
	;

logical_xor_expression
	: logical_and_expression
	| logical_xor_expression XOR_OP logical_and_expression { $$ = new Operator($2, $1, $3); }
	;

logical_or_expression
	: logical_xor_expression
	| logical_or_expression OR_OP logical_xor_expression { $$ = new Operator($2, $1, $3); }
	;

conditional_expression
	: logical_or_expression
	| logical_or_expression '?' expression ':' assignment_expression { $$ = new Conditional($1, $3, $5); }
	;

assignment_expression
	: conditional_expression
	| unary_expression assignment_operator assignment_expression { $$ = new Operator($2, $1, $3); }
	;

assignment_operator
	: '=' { $$ = '='; }
	| MUL_ASSIGN
	| DIV_ASSIGN
	| AND_ASSIGN
	| ADD_ASSIGN
	| SUB_ASSIGN
	| LEFT_ASSIGN
	| RIGHT_ASSIGN
 	| MOD_ASSIGN
	| XOR_ASSIGN
	| OR_ASSIGN
	;

expression
	: assignment_expression
	| expression ',' assignment_expression { $$ = link($1, $3); }
	;

constant_expression
	: conditional_expression
	;

// node
declaration
	: function_prototype ';' { $$ = $1; }
	| init_declarator_list ';' { $$ = $1; }
	| PRECISION precision_qualifier type_specifier_no_prec ';' { $3->setPrecision($2); $$ = $3; }
  // interface blocks: ?
/*	| type_qualifier IDENTIFIER '{' struct_declaration_list '}' ';'
	| type_qualifier IDENTIFIER '{' struct_declaration_list '}' IDENTIFIER ';'
	| type_qualifier IDENTIFIER '{' struct_declaration_list '}' IDENTIFIER '[' ']' ';'
	| type_qualifier IDENTIFIER '{' struct_declaration_list '}' IDENTIFIER '[' constant_expression ']' ';'
	| type_qualifier ';'*/
	;

function_prototype
	: function_declarator ')'
	;

function_declarator
	: function_header
	| function_header_with_parameters
	;

function_header_with_parameters
	: function_header parameter_declaration { $$ = $1; $$->addParam($2); }
	| function_header_with_parameters ',' parameter_declaration { $$ = $1; $$->addParam($3); }
	;

function_header
	: fully_specified_type IDENTIFIER '(' { $$ = new Function($1, $2); }
	;

parameter_declarator
	: type_specifier IDENTIFIER { $$ = new Parameter($1, $2); }
	| type_specifier IDENTIFIER '[' constant_expression ']' { $$ = new Parameter($1, $2, $4); }
	;

parameter_declaration
	: parameter_type_qualifier parameter_qualifier parameter_declarator { $$ = $3; $$->setTypeQualifier($1); $$->setQualifier($2); }
	| parameter_qualifier parameter_declarator { $$ = $2; $$->setQualifier($1); }
	| parameter_type_qualifier parameter_qualifier parameter_type_specifier { $$ = new Parameter($3); $$->setTypeQualifier($1); $$->setQualifier($2); }
	| parameter_qualifier parameter_type_specifier { $$ = new Parameter($2); $$->setQualifier($1); }
	;

parameter_qualifier
	: { $$ = 0; } /* empty */
	| IN
	| OUT
	| INOUT
	;

parameter_type_specifier
	: type_specifier
	;

init_declarator_list
	: single_declaration
	| init_declarator_list ',' IDENTIFIER { $$ = link($1, new Type(*$1)); $$->setName($3); }
	| init_declarator_list ',' IDENTIFIER '[' ']' { $$ = link($1, new Type(*$1)); $$->setName($3); $$->setIsArray(); }
	| init_declarator_list ',' IDENTIFIER '[' constant_expression ']' { $$ = link($1, new Type(*$1)); $$->setName($3); $$->setArray($5); }
	| init_declarator_list ',' IDENTIFIER '[' ']' '=' initializer { $$ = link($1, new Type(*$1)); $$->setName($3); $$->setInitializer($7); }
	| init_declarator_list ',' IDENTIFIER '[' constant_expression ']' '=' initializer { $$ = link($1, new Type(*$1)); $$->setName($3); $$->setArray($5); $$->setInitializer($8); }
	| init_declarator_list ',' IDENTIFIER '=' initializer { $$ = link($1, new Type(*$1)); $$->setName($3); $$->setInitializer($5); }
	;

single_declaration
	: fully_specified_type
	| fully_specified_type IDENTIFIER { $$ = $1; $$->setName($2); }
	| fully_specified_type IDENTIFIER '[' ']' { $$ = $1; $$->setName($2); $$->setIsArray(); }
	| fully_specified_type IDENTIFIER '[' constant_expression ']' { $$ = $1; $$->setName($2); $$->setArray($4); }
	| fully_specified_type IDENTIFIER '[' ']' '=' initializer { $$ = $1; $$->setName($2); $$->setIsArray(); $$->setInitializer($6); }
	| fully_specified_type IDENTIFIER '[' constant_expression ']' '=' initializer { $$ = $1; $$->setName($2); $$->setArray($4); $$->setInitializer($7); }
	| fully_specified_type IDENTIFIER '=' initializer { $$ = $1; $$->setName($2); $$->setInitializer($4); }
//	| INVARIANT IDENTIFIER
	;
// Grammar Note: No 'enum', or 'typedef'.

fully_specified_type
	: type_specifier
	| type_qualifier type_specifier { $$ = $2; $$->setQualifier($1); }
	;

invariant_qualifier
	: INVARIANT
	;

interpolation_qualifier
	: SMOOTH
	| FLAT
	| NOPERSPECTIVE
	;

layout_qualifier
	: LAYOUT '(' layout_qualifier_id_list ')' { $$ = $3; }
	;

layout_qualifier_id_list
	: layout_qualifier_id
	| layout_qualifier_id_list ',' layout_qualifier_id { $$ = link($1, $3); }
	;

layout_qualifier_id
        : IDENTIFIER { /*$$ = new Layout($1);*/ }
        | IDENTIFIER '=' INTCONSTANT { /*$$ = new Layout($1, $3);*/ }
	;

parameter_type_qualifier
	: CONST
	;

type_qualifier
        : storage_qualifier { /* $$ = new Qualifier(); $$->add($1); */ }
        | layout_qualifier { /* $$ = new Qualifier(); $$->setLayout($1); */ }
        | layout_qualifier storage_qualifier { /* $$ = new Qualifier(); $$->add($2); $$->setLayout($1); */ }
        | interpolation_qualifier storage_qualifier { /* $$ = new Qualifier(); $2->insert($1); $$->add($2); */ }
        | interpolation_qualifier { /* $$ = new Qualifier(); $$->add(makeSet($1)); */ }
        | invariant_qualifier storage_qualifier { /* $$ = new Qualifier(); $2->insert($1); $$->add($2); */ }
        | invariant_qualifier interpolation_qualifier storage_qualifier { /* $$ = new Qualifier(); $3->insert($1); $3->insert($2); $$->add($3); */ }
        | invariant_qualifier { /* $$ = new Qualifier(); $$->add(makeSet($1)); */ }
	;

storage_qualifier
	: CONST { $$ = makeSet(CONST); }
	| ATTRIBUTE { $$ = makeSet(ATTRIBUTE); } // Vertex only.
	| VARYING { $$ = makeSet(VARYING); }
	| CENTROID VARYING { $$ = makeSet(CENTROID, VARYING); }
	| IN { $$ = makeSet(IN); }
	| OUT { $$ = makeSet(OUT); }
	| CENTROID IN { $$ = makeSet(CENTROID, IN); }
	| CENTROID OUT { $$ = makeSet(CENTROID, OUT); }
	| UNIFORM { $$ = makeSet(UNIFORM); }
	;

type_specifier
	: type_specifier_no_prec
  | precision_qualifier type_specifier_no_prec { /* $$ = $2; $$->setQualifier($1); */ }
	;

type_specifier_no_prec
	: type_specifier_nonarray
	| type_specifier_nonarray '[' ']' { $$ = $1; $$->setIsArray(); }
	| type_specifier_nonarray '[' constant_expression ']' { $$ = $1; $$->setArray($3); }
	;

type_specifier_nonarray
  : type_specifier_nonarray_simple { /* $$ = new Type($1); */ }
	| struct_specifier
//	| TYPE_NAME
  ;

type_specifier_nonarray_simple
	: VOID
	| FLOAT
	| INT
	| UINT
	| BOOL
	| VEC2
	| VEC3
	| VEC4
	| BVEC2
	| BVEC3
	| BVEC4
	| IVEC2
	| IVEC3
	| IVEC4
	| UVEC2
	| UVEC3
	| UVEC4
	| MAT2
	| MAT3
	| MAT4
	| MAT2X2
	| MAT2X3
	| MAT2X4
	| MAT3X2
	| MAT3X3
	| MAT3X4
	| MAT4X2
	| MAT4X3
	| MAT4X4
	| SAMPLER1D
	| SAMPLER2D
	| SAMPLER3D
	| SAMPLERCUBE
	| SAMPLER1DSHADOW
	| SAMPLER2DSHADOW
	| SAMPLERCUBESHADOW
	| SAMPLER1DARRAY
	| SAMPLER2DARRAY
	| SAMPLER1DARRAYSHADOW
	| SAMPLER2DARRAYSHADOW
	| ISAMPLER1D
	| ISAMPLER2D
	| ISAMPLER3D
	| ISAMPLERCUBE
	| ISAMPLER1DARRAY
	| ISAMPLER2DARRAY
	| USAMPLER1D
	| USAMPLER2D
	| USAMPLER3D
	| USAMPLERCUBE
	| USAMPLER1DARRAY
	| USAMPLER2DARRAY
	| SAMPLER2DRECT
	| SAMPLER2DRECTSHADOW
	| ISAMPLER2DRECT
	| USAMPLER2DRECT
	| SAMPLERBUFFER
	| ISAMPLERBUFFER
	| USAMPLERBUFFER
	| SAMPLER2DMS
	| ISAMPLER2DMS
	| USAMPLER2DMS
	| SAMPLER2DMSARRAY
	| ISAMPLER2DMSARRAY
	| USAMPLER2DMSARRAY
	;

precision_qualifier
	: HIGH_PRECISION
	| MEDIUM_PRECISION
	| LOW_PRECISION
	;

struct_specifier
  : STRUCT IDENTIFIER '{' struct_declaration_list '}' { /* $$ = new Struct($4, $2); */ }
  | STRUCT '{' struct_declaration_list '}' { /* $$ = new Struct($3); */ }
	;

struct_declaration_list
	: struct_declaration
	| struct_declaration_list struct_declaration { $$ = link($1, $2); }
	;

struct_declaration
  : type_specifier struct_declarator_list ';' { /* FOR_EACH($2) { it->setType($1); } $$ = $2; */ }
  | type_qualifier type_specifier struct_declarator_list ';' { /* FOR_EACH($3) { $2->setQualifier($1); it->setType($2); } $$ = $3; */ }
	;

struct_declarator_list
	: struct_declarator
	| struct_declarator_list ',' struct_declarator { $$ = link($1, $3); }
	;

struct_declarator
	: IDENTIFIER { $$ = new Type(); $$->setName($1); }
	| IDENTIFIER '[' ']' { $$ = new Type(); $$->setName($1); $$->setIsArray(); }
  | IDENTIFIER '[' constant_expression ']' { $$ = new Type(); $$->setName($1); $$->setArray($3); }
	;

initializer
	: assignment_expression
	;

declaration_statement
	: declaration
	;

statement
	: compound_statement
	| simple_statement
// Grammar Note: labeled statements for SWITCH only; 'goto' is not supported.
	;

simple_statement
	: declaration_statement
	| expression_statement
	| selection_statement
	| switch_statement
	| case_label
	| iteration_statement
	| jump_statement
	;

compound_statement
	: '{' '}' { $$ = 0; }
	| '{' statement_list '}' { $$ = $2; }
	;

statement_no_new_scope
	: compound_statement_no_new_scope
	| simple_statement
	;

compound_statement_no_new_scope
	: '{' '}' { $$ = 0; }
	| '{' statement_list '}' { $$ = $2; }
	;

statement_list
	: statement
	| statement_list statement { $$ = link($1, $2); }
	;

expression_statement
	: ';' { $$ = 0; }
	| expression ';'
	;

selection_statement
	: IF '(' expression ')' selection_rest_statement { $$ = $5; }
	;

selection_rest_statement
	: statement ELSE statement { $$ = new If($<node>-1, $1, $3); }
	| statement { $$ = new If($<node>-1, $1); }
	;

condition
	: expression
	| fully_specified_type IDENTIFIER '=' initializer { $1->setName($2); $1->setInitializer($4); $$ = $1; }
	;

switch_statement
  : SWITCH '(' expression ')' '{' switch_statement_list '}' { /* $$ = new Switch($3, $6); */ }
	;

switch_statement_list
	: { $$ = 0; } /* nothing */
	| statement_list
	;

case_label
  : CASE expression ':' { /* $$ = new Label($2); */ }
  | DEFAULT ':' { /* $$ = new Label(); */ }
	;

iteration_statement
  : WHILE '(' condition ')' statement_no_new_scope { /* $$ = new While($3, $5); */ }
  | DO statement WHILE '(' expression ')' ';' { /* $$ = new DoWhile($2, $5); */ }
//	| FOR '(' for_init_statement for_rest_statement ')' statement_no_new_scope
  | FOR '(' for_init_statement conditionopt ';' expressionopt ')' statement_no_new_scope { /* $$ = new For($3, $4, $6, $8); */ }
	;

for_init_statement
	: expression_statement
	| declaration_statement
	;

conditionopt
	: { $$ = 0; } /* empty */
	| condition
	;

expressionopt
	: { $$ = 0; } /* empty */
	| expression
	;

/*
for_rest_statement
	: conditionopt ';'
	| conditionopt ';' expression
	;
*/
jump_statement
	: CONTINUE ';' { $$ = new Jump($1); }
	| BREAK ';' { $$ = new Jump($1); }
	| RETURN ';' { $$ = new Jump($1); }
	| RETURN expression ';' { $$ = new Jump($1, $2); }
	| DISCARD ';' { $$ = new Jump($1); } // Fragment shader only.
	;
// Grammar Note: No 'goto'. Gotos are not supported.

translation_unit
	: external_declaration { $$ = $1; if (!tree) { tree = $1; while (tree && tree->prev) tree = tree->prev; } }
	| translation_unit external_declaration { $$ = link($1, $2); }
	;

external_declaration
	: function_definition
	| declaration
	;

function_definition
  : function_prototype compound_statement_no_new_scope { /* $$ = $1; $1->setBody($2); */ }
	;

%%

void yyerror(const char* str) {
	fprintf(stderr, "error - %s\n", str);
}
