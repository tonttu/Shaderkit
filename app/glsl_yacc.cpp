/* A Bison parser, made by GNU Bison 2.4.2.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2006, 2009-2010 Free Software
   Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.4.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0

/* Substitute the variable and function names.  */
#define yyparse         glslparse
#define yylex           glsllex
#define yyerror         glslerror
#define yylval          glsllval
#define yychar          glslchar
#define yydebug         glsldebug
#define yynerrs         glslnerrs


/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 18 "shader/glsl.y"

  #include <cstdio>
  #include <string>
  #include <map>
  #include <set>
  #include <cassert>
  #include <iostream>

  int yylex(void);
  int yyparse(void);
  void yyerror(const char* str);

  namespace Parser {

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
  }


/* Line 189 of yacc.c  */
#line 307 "glsl.tab.c"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 1
#endif


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     ATTRIBUTE = 258,
     CONST = 259,
     BOOL = 260,
     FLOAT = 261,
     INT = 262,
     UINT = 263,
     BREAK = 264,
     CONTINUE = 265,
     DO = 266,
     ELSE = 267,
     FOR = 268,
     IF = 269,
     DISCARD = 270,
     RETURN = 271,
     SWITCH = 272,
     CASE = 273,
     DEFAULT = 274,
     BVEC2 = 275,
     BVEC3 = 276,
     BVEC4 = 277,
     IVEC2 = 278,
     IVEC3 = 279,
     IVEC4 = 280,
     UVEC2 = 281,
     UVEC3 = 282,
     UVEC4 = 283,
     VEC2 = 284,
     VEC3 = 285,
     VEC4 = 286,
     MAT2 = 287,
     MAT3 = 288,
     MAT4 = 289,
     CENTROID = 290,
     IN = 291,
     OUT = 292,
     INOUT = 293,
     UNIFORM = 294,
     VARYING = 295,
     NOPERSPECTIVE = 296,
     FLAT = 297,
     SMOOTH = 298,
     LAYOUT = 299,
     MAT2X2 = 300,
     MAT2X3 = 301,
     MAT2X4 = 302,
     MAT3X2 = 303,
     MAT3X3 = 304,
     MAT3X4 = 305,
     MAT4X2 = 306,
     MAT4X3 = 307,
     MAT4X4 = 308,
     SAMPLER1D = 309,
     SAMPLER2D = 310,
     SAMPLER3D = 311,
     SAMPLERCUBE = 312,
     SAMPLER1DSHADOW = 313,
     SAMPLER2DSHADOW = 314,
     SAMPLERCUBESHADOW = 315,
     SAMPLER1DARRAY = 316,
     SAMPLER2DARRAY = 317,
     SAMPLER1DARRAYSHADOW = 318,
     SAMPLER2DARRAYSHADOW = 319,
     ISAMPLER1D = 320,
     ISAMPLER2D = 321,
     ISAMPLER3D = 322,
     ISAMPLERCUBE = 323,
     ISAMPLER1DARRAY = 324,
     ISAMPLER2DARRAY = 325,
     USAMPLER1D = 326,
     USAMPLER2D = 327,
     USAMPLER3D = 328,
     USAMPLERCUBE = 329,
     USAMPLER1DARRAY = 330,
     USAMPLER2DARRAY = 331,
     SAMPLER2DRECT = 332,
     SAMPLER2DRECTSHADOW = 333,
     ISAMPLER2DRECT = 334,
     USAMPLER2DRECT = 335,
     SAMPLERBUFFER = 336,
     ISAMPLERBUFFER = 337,
     USAMPLERBUFFER = 338,
     SAMPLER2DMS = 339,
     ISAMPLER2DMS = 340,
     USAMPLER2DMS = 341,
     SAMPLER2DMSARRAY = 342,
     ISAMPLER2DMSARRAY = 343,
     USAMPLER2DMSARRAY = 344,
     STRUCT = 345,
     VOID = 346,
     WHILE = 347,
     IDENTIFIER = 348,
     FLOATCONSTANT = 349,
     INTCONSTANT = 350,
     UINTCONSTANT = 351,
     LEFT_OP = 352,
     RIGHT_OP = 353,
     INC_OP = 354,
     DEC_OP = 355,
     LE_OP = 356,
     GE_OP = 357,
     EQ_OP = 358,
     NE_OP = 359,
     AND_OP = 360,
     OR_OP = 361,
     XOR_OP = 362,
     MUL_ASSIGN = 363,
     DIV_ASSIGN = 364,
     ADD_ASSIGN = 365,
     LEFT_ASSIGN = 366,
     RIGHT_ASSIGN = 367,
     AND_ASSIGN = 368,
     XOR_ASSIGN = 369,
     OR_ASSIGN = 370,
     SUB_ASSIGN = 371,
     MOD_ASSIGN = 372,
     LEFT_PAREN = 373,
     RIGHT_PAREN = 374,
     LEFT_BRACKET = 375,
     RIGHT_BRACKET = 376,
     LEFT_BRACE = 377,
     RIGHT_BRACE = 378,
     DOT = 379,
     COMMA = 380,
     COLON = 381,
     EQUAL = 382,
     SEMICOLON = 383,
     BANG = 384,
     DASH = 385,
     TILDE = 386,
     PLUS = 387,
     STAR = 388,
     SLASH = 389,
     PERCENT = 390,
     LEFT_ANGLE = 391,
     RIGHT_ANGLE = 392,
     VERTICAL_BAR = 393,
     CARET = 394,
     AMPERSAND = 395,
     QUESTION = 396,
     INVARIANT = 397,
     HIGH_PRECISION = 398,
     MEDIUM_PRECISION = 399,
     LOW_PRECISION = 400,
     PRECISION = 401,
     TRUECONSTANT = 402,
     FALSECONSTANT = 403,
     BOOLCONSTANT = 404,
     PP = 405,
     COMMENT = 406,
     ERROR = 407,
     COMMENT_BEGIN = 408,
     COMMENT_END = 409
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 246 "shader/glsl.y"

  void* sset;
  int integer;
  float f;
  char* string;
  Parser::Node* node;
  Parser::Function* func;
  Parser::Parameter* param;
  Parser::Type* type;
  Parser::Qualifier* qualifier;
  Parser::Layout* layout;



/* Line 214 of yacc.c  */
#line 512 "glsl.tab.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 524 "glsl.tab.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  134
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   3696

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  179
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  82
/* YYNRULES -- Number of rules.  */
#define YYNRULES  282
/* YYNRULES -- Number of states.  */
#define YYNSTATES  411

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   409

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   163,     2,     2,     2,   167,   170,     2,
     155,   156,   165,   161,   160,   162,   159,   166,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   174,   176,
     168,   175,   169,   173,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   157,     2,   158,   171,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   177,   172,   178,   164,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
     145,   146,   147,   148,   149,   150,   151,   152,   153,   154
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     7,     9,    11,    13,    15,    17,
      21,    23,    28,    30,    34,    37,    40,    42,    44,    46,
      50,    53,    56,    59,    61,    64,    68,    71,    74,    76,
      79,    82,    85,    87,    89,    91,    93,    95,    99,   103,
     107,   109,   113,   117,   119,   123,   127,   129,   133,   137,
     141,   145,   147,   151,   155,   157,   161,   163,   167,   169,
     173,   175,   179,   181,   185,   187,   191,   193,   199,   201,
     205,   207,   209,   211,   213,   215,   217,   219,   221,   223,
     225,   227,   229,   233,   235,   238,   241,   246,   249,   251,
     253,   256,   260,   264,   267,   273,   277,   280,   284,   287,
     288,   290,   292,   294,   296,   298,   302,   308,   315,   323,
     332,   338,   340,   343,   348,   354,   361,   369,   374,   376,
     379,   381,   383,   385,   387,   392,   394,   398,   400,   404,
     406,   408,   410,   413,   416,   418,   421,   425,   427,   429,
     431,   433,   436,   438,   440,   443,   446,   448,   450,   453,
     455,   459,   464,   466,   468,   470,   472,   474,   476,   478,
     480,   482,   484,   486,   488,   490,   492,   494,   496,   498,
     500,   502,   504,   506,   508,   510,   512,   514,   516,   518,
     520,   522,   524,   526,   528,   530,   532,   534,   536,   538,
     540,   542,   544,   546,   548,   550,   552,   554,   556,   558,
     560,   562,   564,   566,   568,   570,   572,   574,   576,   578,
     580,   582,   584,   586,   588,   590,   592,   594,   596,   598,
     600,   602,   604,   610,   615,   617,   620,   624,   629,   631,
     635,   637,   641,   646,   648,   650,   652,   654,   656,   658,
     660,   662,   664,   666,   668,   671,   675,   677,   679,   682,
     686,   688,   691,   693,   696,   702,   706,   708,   710,   715,
     723,   724,   726,   730,   733,   739,   747,   756,   758,   760,
     761,   763,   764,   766,   769,   772,   775,   779,   782,   784,
     787,   789,   791
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     258,     0,    -1,    93,    -1,   180,    -1,    95,    -1,    96,
      -1,    94,    -1,   147,    -1,   148,    -1,   155,   206,   156,
      -1,   181,    -1,   182,   157,   183,   158,    -1,   184,    -1,
     182,   159,    93,    -1,   182,    99,    -1,   182,   100,    -1,
     206,    -1,   185,    -1,   186,    -1,   182,   159,   186,    -1,
     188,   156,    -1,   187,   156,    -1,   189,    91,    -1,   189,
      -1,   189,   204,    -1,   188,   160,   204,    -1,   228,   155,
      -1,    93,   155,    -1,   182,    -1,    99,   190,    -1,   100,
     190,    -1,   191,   190,    -1,   161,    -1,   162,    -1,   163,
      -1,   164,    -1,   190,    -1,   192,   165,   190,    -1,   192,
     166,   190,    -1,   192,   167,   190,    -1,   192,    -1,   193,
     161,   192,    -1,   193,   162,   192,    -1,   193,    -1,   194,
      97,   193,    -1,   194,    98,   193,    -1,   194,    -1,   195,
     168,   194,    -1,   195,   169,   194,    -1,   195,   101,   194,
      -1,   195,   102,   194,    -1,   195,    -1,   196,   103,   195,
      -1,   196,   104,   195,    -1,   196,    -1,   197,   170,   196,
      -1,   197,    -1,   198,   171,   197,    -1,   198,    -1,   199,
     172,   198,    -1,   199,    -1,   200,   105,   199,    -1,   200,
      -1,   201,   107,   200,    -1,   201,    -1,   202,   106,   201,
      -1,   202,    -1,   202,   173,   206,   174,   204,    -1,   203,
      -1,   190,   205,   204,    -1,   175,    -1,   108,    -1,   109,
      -1,   113,    -1,   110,    -1,   116,    -1,   111,    -1,   112,
      -1,   117,    -1,   114,    -1,   115,    -1,   204,    -1,   206,
     160,   204,    -1,   203,    -1,   209,   176,    -1,   217,   176,
      -1,   146,   232,   229,   176,    -1,   210,   156,    -1,   212,
      -1,   211,    -1,   212,   214,    -1,   211,   160,   214,    -1,
     219,    93,   155,    -1,   228,    93,    -1,   228,    93,   157,
     207,   158,    -1,   225,   215,   213,    -1,   215,   213,    -1,
     225,   215,   216,    -1,   215,   216,    -1,    -1,    36,    -1,
      37,    -1,    38,    -1,   228,    -1,   218,    -1,   217,   160,
      93,    -1,   217,   160,    93,   157,   158,    -1,   217,   160,
      93,   157,   207,   158,    -1,   217,   160,    93,   157,   158,
     175,   238,    -1,   217,   160,    93,   157,   207,   158,   175,
     238,    -1,   217,   160,    93,   175,   238,    -1,   219,    -1,
     219,    93,    -1,   219,    93,   157,   158,    -1,   219,    93,
     157,   207,   158,    -1,   219,    93,   157,   158,   175,   238,
      -1,   219,    93,   157,   207,   158,   175,   238,    -1,   219,
      93,   175,   238,    -1,   228,    -1,   226,   228,    -1,   142,
      -1,    43,    -1,    42,    -1,    41,    -1,    44,   155,   223,
     156,    -1,   224,    -1,   223,   160,   224,    -1,    93,    -1,
      93,   175,    95,    -1,     4,    -1,   227,    -1,   222,    -1,
     222,   227,    -1,   221,   227,    -1,   221,    -1,   220,   227,
      -1,   220,   221,   227,    -1,   220,    -1,     4,    -1,     3,
      -1,    40,    -1,    35,    40,    -1,    36,    -1,    37,    -1,
      35,    36,    -1,    35,    37,    -1,    39,    -1,   229,    -1,
     232,   229,    -1,   230,    -1,   230,   157,   158,    -1,   230,
     157,   207,   158,    -1,   231,    -1,   233,    -1,    91,    -1,
       6,    -1,     7,    -1,     8,    -1,     5,    -1,    29,    -1,
      30,    -1,    31,    -1,    20,    -1,    21,    -1,    22,    -1,
      23,    -1,    24,    -1,    25,    -1,    26,    -1,    27,    -1,
      28,    -1,    32,    -1,    33,    -1,    34,    -1,    45,    -1,
      46,    -1,    47,    -1,    48,    -1,    49,    -1,    50,    -1,
      51,    -1,    52,    -1,    53,    -1,    54,    -1,    55,    -1,
      56,    -1,    57,    -1,    58,    -1,    59,    -1,    60,    -1,
      61,    -1,    62,    -1,    63,    -1,    64,    -1,    65,    -1,
      66,    -1,    67,    -1,    68,    -1,    69,    -1,    70,    -1,
      71,    -1,    72,    -1,    73,    -1,    74,    -1,    75,    -1,
      76,    -1,    77,    -1,    78,    -1,    79,    -1,    80,    -1,
      81,    -1,    82,    -1,    83,    -1,    84,    -1,    85,    -1,
      86,    -1,    87,    -1,    88,    -1,    89,    -1,   143,    -1,
     144,    -1,   145,    -1,    90,    93,   177,   234,   178,    -1,
      90,   177,   234,   178,    -1,   235,    -1,   234,   235,    -1,
     228,   236,   176,    -1,   226,   228,   236,   176,    -1,   237,
      -1,   236,   160,   237,    -1,    93,    -1,    93,   157,   158,
      -1,    93,   157,   207,   158,    -1,   204,    -1,   208,    -1,
     242,    -1,   241,    -1,   239,    -1,   246,    -1,   247,    -1,
     250,    -1,   252,    -1,   253,    -1,   257,    -1,   177,   178,
      -1,   177,   245,   178,    -1,   244,    -1,   241,    -1,   177,
     178,    -1,   177,   245,   178,    -1,   240,    -1,   245,   240,
      -1,   176,    -1,   206,   176,    -1,    14,   155,   206,   156,
     248,    -1,   240,    12,   240,    -1,   240,    -1,   206,    -1,
     219,    93,   175,   238,    -1,    17,   155,   206,   156,   177,
     251,   178,    -1,    -1,   245,    -1,    18,   206,   174,    -1,
      19,   174,    -1,    92,   155,   249,   156,   243,    -1,    11,
     240,    92,   155,   206,   156,   176,    -1,    13,   155,   254,
     255,   176,   256,   156,   243,    -1,   246,    -1,   239,    -1,
      -1,   249,    -1,    -1,   206,    -1,    10,   176,    -1,     9,
     176,    -1,    16,   176,    -1,    16,   206,   176,    -1,    15,
     176,    -1,   259,    -1,   258,   259,    -1,   260,    -1,   208,
      -1,   209,   244,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   325,   325,   329,   330,   331,   332,   333,   334,   335,
     340,   341,   342,   343,   344,   345,   349,   354,   359,   360,
     365,   366,   371,   372,   377,   378,   383,   384,   399,   400,
     401,   402,   408,   409,   410,   411,   417,   418,   419,   420,
     424,   425,   426,   430,   431,   432,   436,   437,   438,   439,
     440,   444,   445,   446,   450,   451,   455,   456,   460,   461,
     465,   466,   470,   471,   475,   476,   480,   481,   485,   486,
     490,   491,   492,   493,   494,   495,   496,   497,   498,   499,
     500,   504,   505,   509,   514,   515,   516,   526,   530,   531,
     535,   536,   540,   544,   545,   549,   550,   551,   552,   556,
     557,   558,   559,   563,   567,   568,   569,   570,   571,   572,
     573,   577,   578,   579,   580,   581,   582,   583,   589,   590,
     594,   598,   599,   600,   604,   608,   609,   613,   614,   618,
     622,   623,   624,   625,   626,   627,   628,   629,   633,   634,
     635,   636,   637,   638,   639,   640,   641,   645,   646,   650,
     651,   652,   656,   657,   662,   663,   664,   665,   666,   667,
     668,   669,   670,   671,   672,   673,   674,   675,   676,   677,
     678,   679,   680,   681,   682,   683,   684,   685,   686,   687,
     688,   689,   690,   691,   692,   693,   694,   695,   696,   697,
     698,   699,   700,   701,   702,   703,   704,   705,   706,   707,
     708,   709,   710,   711,   712,   713,   714,   715,   716,   717,
     718,   719,   720,   721,   722,   723,   724,   725,   726,   730,
     731,   732,   736,   737,   741,   742,   746,   747,   751,   752,
     756,   757,   758,   762,   766,   770,   771,   776,   777,   778,
     779,   780,   781,   782,   786,   787,   791,   792,   796,   797,
     801,   802,   806,   807,   811,   815,   816,   820,   821,   825,
     829,   830,   834,   835,   839,   840,   842,   846,   847,   851,
     852,   856,   857,   867,   868,   869,   870,   871,   876,   877,
     881,   882,   886
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "ATTRIBUTE", "CONST", "BOOL", "FLOAT",
  "INT", "UINT", "BREAK", "CONTINUE", "DO", "ELSE", "FOR", "IF", "DISCARD",
  "RETURN", "SWITCH", "CASE", "DEFAULT", "BVEC2", "BVEC3", "BVEC4",
  "IVEC2", "IVEC3", "IVEC4", "UVEC2", "UVEC3", "UVEC4", "VEC2", "VEC3",
  "VEC4", "MAT2", "MAT3", "MAT4", "CENTROID", "IN", "OUT", "INOUT",
  "UNIFORM", "VARYING", "NOPERSPECTIVE", "FLAT", "SMOOTH", "LAYOUT",
  "MAT2X2", "MAT2X3", "MAT2X4", "MAT3X2", "MAT3X3", "MAT3X4", "MAT4X2",
  "MAT4X3", "MAT4X4", "SAMPLER1D", "SAMPLER2D", "SAMPLER3D", "SAMPLERCUBE",
  "SAMPLER1DSHADOW", "SAMPLER2DSHADOW", "SAMPLERCUBESHADOW",
  "SAMPLER1DARRAY", "SAMPLER2DARRAY", "SAMPLER1DARRAYSHADOW",
  "SAMPLER2DARRAYSHADOW", "ISAMPLER1D", "ISAMPLER2D", "ISAMPLER3D",
  "ISAMPLERCUBE", "ISAMPLER1DARRAY", "ISAMPLER2DARRAY", "USAMPLER1D",
  "USAMPLER2D", "USAMPLER3D", "USAMPLERCUBE", "USAMPLER1DARRAY",
  "USAMPLER2DARRAY", "SAMPLER2DRECT", "SAMPLER2DRECTSHADOW",
  "ISAMPLER2DRECT", "USAMPLER2DRECT", "SAMPLERBUFFER", "ISAMPLERBUFFER",
  "USAMPLERBUFFER", "SAMPLER2DMS", "ISAMPLER2DMS", "USAMPLER2DMS",
  "SAMPLER2DMSARRAY", "ISAMPLER2DMSARRAY", "USAMPLER2DMSARRAY", "STRUCT",
  "VOID", "WHILE", "IDENTIFIER", "FLOATCONSTANT", "INTCONSTANT",
  "UINTCONSTANT", "LEFT_OP", "RIGHT_OP", "INC_OP", "DEC_OP", "LE_OP",
  "GE_OP", "EQ_OP", "NE_OP", "AND_OP", "OR_OP", "XOR_OP", "MUL_ASSIGN",
  "DIV_ASSIGN", "ADD_ASSIGN", "LEFT_ASSIGN", "RIGHT_ASSIGN", "AND_ASSIGN",
  "XOR_ASSIGN", "OR_ASSIGN", "SUB_ASSIGN", "MOD_ASSIGN", "LEFT_PAREN",
  "RIGHT_PAREN", "LEFT_BRACKET", "RIGHT_BRACKET", "LEFT_BRACE",
  "RIGHT_BRACE", "DOT", "COMMA", "COLON", "EQUAL", "SEMICOLON", "BANG",
  "DASH", "TILDE", "PLUS", "STAR", "SLASH", "PERCENT", "LEFT_ANGLE",
  "RIGHT_ANGLE", "VERTICAL_BAR", "CARET", "AMPERSAND", "QUESTION",
  "INVARIANT", "HIGH_PRECISION", "MEDIUM_PRECISION", "LOW_PRECISION",
  "PRECISION", "TRUECONSTANT", "FALSECONSTANT", "BOOLCONSTANT", "PP",
  "COMMENT", "ERROR", "COMMENT_BEGIN", "COMMENT_END", "'('", "')'", "'['",
  "']'", "'.'", "','", "'+'", "'-'", "'!'", "'~'", "'*'", "'/'", "'%'",
  "'<'", "'>'", "'&'", "'^'", "'|'", "'?'", "':'", "'='", "';'", "'{'",
  "'}'", "$accept", "variable_identifier", "primary_expression",
  "postfix_expression", "integer_expression", "function_call",
  "function_call_or_method", "function_call_generic",
  "function_call_header_no_parameters",
  "function_call_header_with_parameters", "function_call_header",
  "unary_expression", "unary_operator", "multiplicative_expression",
  "additive_expression", "shift_expression", "relational_expression",
  "equality_expression", "and_expression", "exclusive_or_expression",
  "inclusive_or_expression", "logical_and_expression",
  "logical_xor_expression", "logical_or_expression",
  "conditional_expression", "assignment_expression", "assignment_operator",
  "expression", "constant_expression", "declaration", "function_prototype",
  "function_declarator", "function_header_with_parameters",
  "function_header", "parameter_declarator", "parameter_declaration",
  "parameter_qualifier", "parameter_type_specifier",
  "init_declarator_list", "single_declaration", "fully_specified_type",
  "invariant_qualifier", "interpolation_qualifier", "layout_qualifier",
  "layout_qualifier_id_list", "layout_qualifier_id",
  "parameter_type_qualifier", "type_qualifier", "storage_qualifier",
  "type_specifier", "type_specifier_no_prec", "type_specifier_nonarray",
  "type_specifier_nonarray_simple", "precision_qualifier",
  "struct_specifier", "struct_declaration_list", "struct_declaration",
  "struct_declarator_list", "struct_declarator", "initializer",
  "declaration_statement", "statement", "simple_statement",
  "compound_statement", "statement_no_new_scope",
  "compound_statement_no_new_scope", "statement_list",
  "expression_statement", "selection_statement",
  "selection_rest_statement", "condition", "switch_statement",
  "switch_statement_list", "case_label", "iteration_statement",
  "for_init_statement", "conditionopt", "expressionopt", "jump_statement",
  "translation_unit", "external_declaration", "function_definition", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,   357,   358,   359,   360,   361,   362,   363,   364,
     365,   366,   367,   368,   369,   370,   371,   372,   373,   374,
     375,   376,   377,   378,   379,   380,   381,   382,   383,   384,
     385,   386,   387,   388,   389,   390,   391,   392,   393,   394,
     395,   396,   397,   398,   399,   400,   401,   402,   403,   404,
     405,   406,   407,   408,   409,    40,    41,    91,    93,    46,
      44,    43,    45,    33,   126,    42,    47,    37,    60,    62,
      38,    94,   124,    63,    58,    61,    59,   123,   125
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint16 yyr1[] =
{
       0,   179,   180,   181,   181,   181,   181,   181,   181,   181,
     182,   182,   182,   182,   182,   182,   183,   184,   185,   185,
     186,   186,   187,   187,   188,   188,   189,   189,   190,   190,
     190,   190,   191,   191,   191,   191,   192,   192,   192,   192,
     193,   193,   193,   194,   194,   194,   195,   195,   195,   195,
     195,   196,   196,   196,   197,   197,   198,   198,   199,   199,
     200,   200,   201,   201,   202,   202,   203,   203,   204,   204,
     205,   205,   205,   205,   205,   205,   205,   205,   205,   205,
     205,   206,   206,   207,   208,   208,   208,   209,   210,   210,
     211,   211,   212,   213,   213,   214,   214,   214,   214,   215,
     215,   215,   215,   216,   217,   217,   217,   217,   217,   217,
     217,   218,   218,   218,   218,   218,   218,   218,   219,   219,
     220,   221,   221,   221,   222,   223,   223,   224,   224,   225,
     226,   226,   226,   226,   226,   226,   226,   226,   227,   227,
     227,   227,   227,   227,   227,   227,   227,   228,   228,   229,
     229,   229,   230,   230,   231,   231,   231,   231,   231,   231,
     231,   231,   231,   231,   231,   231,   231,   231,   231,   231,
     231,   231,   231,   231,   231,   231,   231,   231,   231,   231,
     231,   231,   231,   231,   231,   231,   231,   231,   231,   231,
     231,   231,   231,   231,   231,   231,   231,   231,   231,   231,
     231,   231,   231,   231,   231,   231,   231,   231,   231,   231,
     231,   231,   231,   231,   231,   231,   231,   231,   231,   232,
     232,   232,   233,   233,   234,   234,   235,   235,   236,   236,
     237,   237,   237,   238,   239,   240,   240,   241,   241,   241,
     241,   241,   241,   241,   242,   242,   243,   243,   244,   244,
     245,   245,   246,   246,   247,   248,   248,   249,   249,   250,
     251,   251,   252,   252,   253,   253,   253,   254,   254,   255,
     255,   256,   256,   257,   257,   257,   257,   257,   258,   258,
     259,   259,   260
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     1,     1,     1,     1,     1,     3,
       1,     4,     1,     3,     2,     2,     1,     1,     1,     3,
       2,     2,     2,     1,     2,     3,     2,     2,     1,     2,
       2,     2,     1,     1,     1,     1,     1,     3,     3,     3,
       1,     3,     3,     1,     3,     3,     1,     3,     3,     3,
       3,     1,     3,     3,     1,     3,     1,     3,     1,     3,
       1,     3,     1,     3,     1,     3,     1,     5,     1,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     3,     1,     2,     2,     4,     2,     1,     1,
       2,     3,     3,     2,     5,     3,     2,     3,     2,     0,
       1,     1,     1,     1,     1,     3,     5,     6,     7,     8,
       5,     1,     2,     4,     5,     6,     7,     4,     1,     2,
       1,     1,     1,     1,     4,     1,     3,     1,     3,     1,
       1,     1,     2,     2,     1,     2,     3,     1,     1,     1,
       1,     2,     1,     1,     2,     2,     1,     1,     2,     1,
       3,     4,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     5,     4,     1,     2,     3,     4,     1,     3,
       1,     3,     4,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     2,     3,     1,     1,     2,     3,
       1,     2,     1,     2,     5,     3,     1,     1,     4,     7,
       0,     1,     3,     2,     5,     7,     8,     1,     1,     0,
       1,     0,     1,     2,     2,     2,     3,     2,     1,     2,
       1,     1,     2
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       0,   139,   138,   158,   155,   156,   157,   162,   163,   164,
     165,   166,   167,   168,   169,   170,   159,   160,   161,   171,
     172,   173,     0,   142,   143,   146,   140,   123,   122,   121,
       0,   174,   175,   176,   177,   178,   179,   180,   181,   182,
     183,   184,   185,   186,   187,   188,   189,   190,   191,   192,
     193,   194,   195,   196,   197,   198,   199,   200,   201,   202,
     203,   204,   205,   206,   207,   208,   209,   210,   211,   212,
     213,   214,   215,   216,   217,   218,     0,   154,   120,   219,
     220,   221,     0,   281,     0,     0,    89,    99,     0,   104,
     111,   137,   134,   131,     0,   130,   118,   147,   149,   152,
       0,   153,     0,   278,   280,   144,   145,   141,     0,     0,
       0,     0,    84,     0,   282,    87,    99,   129,   100,   101,
     102,    90,     0,    99,     0,    85,   112,     0,   135,   133,
     132,   119,     0,   148,     1,   279,   127,     0,   125,     0,
       0,     0,     0,   224,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     2,     6,     4,     5,
       0,     0,     7,     8,     0,    32,    33,    34,    35,   252,
       0,   248,     3,    10,    28,    12,    17,    18,     0,     0,
      23,    36,     0,    40,    43,    46,    51,    54,    56,    58,
      60,    62,    64,    66,    68,    81,     0,   234,     0,   118,
     237,   250,   236,   235,     0,   238,   239,   240,   241,   242,
     243,    91,    96,    98,   103,     0,   105,    92,     0,     0,
     136,   150,    36,    83,     0,     0,     0,   124,     0,     0,
       0,   230,     0,   228,   223,   225,    86,   274,   273,     0,
       0,     0,   277,   275,     0,     0,     0,   263,     0,    27,
      29,    30,     0,   244,     0,    14,    15,     0,     0,    21,
      20,     0,   154,    24,    71,    72,    74,    76,    77,    73,
      79,    80,    75,    78,    70,     0,    31,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   253,    26,
     249,   251,    93,    95,    97,     0,     0,   113,     0,   233,
     117,   151,   128,   126,   222,     0,     0,     0,   226,     0,
     268,   267,   269,     0,   276,     0,   262,   257,     0,     0,
       9,   245,     0,    16,    13,    19,    25,    69,    37,    38,
      39,    41,    42,    44,    45,    49,    50,    47,    48,    52,
      53,    55,    57,    59,    61,    63,    65,     0,    82,     0,
     106,     0,   110,     0,   114,   227,   231,     0,   229,     0,
     270,     0,     0,     0,     0,     0,    11,     0,     0,     0,
     107,   115,     0,   232,     0,   271,   256,   254,   260,     0,
     247,   264,   246,    67,    94,   108,     0,   116,     0,   272,
       0,     0,   261,     0,   258,   109,   265,     0,   255,   259,
     266
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,   172,   173,   174,   332,   175,   176,   177,   178,   179,
     180,   181,   182,   183,   184,   185,   186,   187,   188,   189,
     190,   191,   192,   193,   194,   195,   275,   196,   224,   197,
     198,    85,    86,    87,   212,   121,   122,   213,    88,    89,
      90,    91,    92,    93,   137,   138,   123,    94,    95,   225,
      97,    98,    99,   100,   101,   142,   143,   232,   233,   310,
     200,   201,   202,   203,   391,   392,   204,   205,   206,   387,
     329,   207,   403,   208,   209,   322,   371,   400,   210,   102,
     103,   104
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -338
static const yytype_int16 yypact[] =
{
    3222,  -338,  -338,  -338,  -338,  -338,  -338,  -338,  -338,  -338,
    -338,  -338,  -338,  -338,  -338,  -338,  -338,  -338,  -338,  -338,
    -338,  -338,    93,  -338,  -338,  -338,  -338,  -338,  -338,  -338,
    -121,  -338,  -338,  -338,  -338,  -338,  -338,  -338,  -338,  -338,
    -338,  -338,  -338,  -338,  -338,  -338,  -338,  -338,  -338,  -338,
    -338,  -338,  -338,  -338,  -338,  -338,  -338,  -338,  -338,  -338,
    -338,  -338,  -338,  -338,  -338,  -338,  -338,  -338,  -338,  -338,
    -338,  -338,  -338,  -338,  -338,  -338,   -82,  -338,  -338,  -338,
    -338,  -338,    38,  -338,  -100,  -104,   -81,    13,  -133,  -338,
     -36,   210,    79,    79,  3551,  -338,  -338,  -338,   -73,  -338,
     278,  -338,  3095,  -338,  -338,  -338,  -338,  -338,     3,   -76,
    3349,   278,  -338,   405,  -338,  -338,    37,  -338,  -338,  -338,
    -338,  -338,  3551,   148,    30,  -338,  -129,    79,  -338,  -338,
    -338,  -338,  2224,  -338,  -338,  -338,   -69,  -125,  -338,  3349,
    3551,    45,  1095,  -338,   -45,    -3,    -1,  1420,    23,    32,
      25,  1917,    51,  2804,    15,    61,    63,  -338,  -338,  -338,
    2804,  2804,  -338,  -338,  2804,  -338,  -338,  -338,  -338,  -338,
     581,  -338,  -338,  -338,   -79,  -338,  -338,  -338,    66,  -112,
    2949,   -48,  2804,    27,    34,   100,   -83,    96,    49,    52,
      53,   122,   121,   -92,  -338,  -338,  -131,  -338,    54,    77,
    -338,  -338,  -338,  -338,   757,  -338,  -338,  -338,  -338,  -338,
    -338,  -338,  -338,  -338,   141,  3551,  -135,  -338,  2369,  2804,
    -338,  -338,  -338,  -338,    78,    77,   140,  -338,     3,  1254,
      45,    81,  -123,  -338,  -338,  -338,  -338,  -338,  -338,   147,
    1757,  2804,  -338,  -338,  -105,  2804,  -118,  -338,  2079,  -338,
    -338,  -338,   -24,  -338,   933,  -338,  -338,  2804,  3450,  -338,
    -338,  2804,    85,  -338,  -338,  -338,  -338,  -338,  -338,  -338,
    -338,  -338,  -338,  -338,  -338,  2804,  -338,  2804,  2804,  2804,
    2804,  2804,  2804,  2804,  2804,  2804,  2804,  2804,  2804,  2804,
    2804,  2804,  2804,  2804,  2804,  2804,  2804,  2804,  -338,  -338,
    -338,  -338,    86,  -338,  -338,  2514,  2804,    67,    99,  -338,
    -338,  -338,  -338,  -338,  -338,   -56,  2659,    45,  -338,    89,
    -338,  -338,  2079,   -19,  -338,    12,  -338,    98,   166,   104,
    -338,  -338,   103,    98,    63,  -338,  -338,  -338,  -338,  -338,
    -338,    27,    27,    34,    34,   100,   100,   100,   100,   -83,
     -83,    96,    49,    52,    53,   122,   121,   -53,  -338,  2804,
      87,   105,  -338,  2804,    90,  -338,  -338,   106,  -338,  2804,
    -338,    91,  1420,    92,    95,  1595,  -338,  2804,   108,  2804,
      97,  -338,  2804,  -338,    20,  2804,   256,  -338,  1420,  2804,
    -338,  -338,  -338,  -338,  -338,  -338,  2804,  -338,   101,    98,
     115,  1420,  1420,   102,  -338,  -338,  -338,  1595,  -338,  -338,
    -338
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -338,  -338,  -338,  -338,  -338,  -338,  -338,    16,  -338,  -338,
    -338,  -128,  -338,   -78,   -74,  -141,   -77,   -17,   -16,   -14,
     -12,   -18,   -13,  -338,  -126,  -172,  -338,  -148,  -188,     9,
      10,  -338,  -338,  -338,    64,   171,   165,    74,  -338,  -338,
    -235,  -338,   199,  -338,  -338,    65,  -338,  -103,   -68,     0,
      24,  -338,  -338,   209,  -338,   153,  -130,    83,   -23,  -291,
      55,  -146,  -337,  -338,  -111,   213,  -168,    75,  -338,  -338,
      -8,  -338,  -338,  -338,  -338,  -338,  -338,  -338,  -338,  -338,
     214,  -338
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -89
static const yytype_int16 yytable[] =
{
      96,   239,   254,   244,   222,   246,   223,   140,   263,    83,
      84,   109,   235,   328,   295,   362,   252,   117,   284,   285,
     255,   256,   305,   128,   129,   130,   217,   124,   218,   297,
     308,   227,   250,   251,   108,   228,   140,   317,   390,   140,
     306,   117,   297,   125,   260,   298,   219,   309,   261,   118,
     119,   120,   115,   318,   276,   297,   326,   126,   301,   220,
     264,   265,   266,   267,   268,   269,   270,   271,   272,   273,
     390,   324,   381,   118,   119,   120,   112,   113,   257,   116,
     258,   296,     1,     2,   132,   286,   287,   328,   395,   336,
     222,   397,   223,   323,   131,   110,   136,   325,   404,   235,
     327,   139,    96,   337,   317,   405,   226,   297,   301,   333,
     141,    83,    84,   199,    22,    23,    24,   361,    25,    26,
     365,   377,   214,   216,   133,   358,   140,   274,   367,   105,
     106,   236,   330,   107,   309,   144,   297,   372,   231,   141,
     230,   297,   141,   345,   346,   347,   348,   199,   357,   338,
     339,   340,   222,   222,   222,   222,   222,   222,   222,   222,
     222,   222,   222,   222,   222,   222,   222,   222,   373,   -88,
     199,   378,   297,   237,   327,   238,   398,   222,   240,   223,
     297,    79,    80,    81,   118,   119,   120,   241,   222,   247,
     223,   309,   277,   278,   279,   280,   281,   282,   283,   288,
     289,   242,   341,   342,   199,   393,   245,   309,   343,   344,
     309,   349,   350,     1,     2,   214,   248,   309,   249,   290,
     402,   384,   259,   291,   309,   292,   386,   293,   294,   141,
     112,   222,   299,   223,   302,   312,   311,   399,   316,   319,
     199,   -22,   363,   359,   369,    22,    23,    24,   199,    25,
      26,    27,    28,    29,   199,   408,   301,   364,   297,   374,
     375,   376,   379,   380,   383,   382,   394,   385,   401,   388,
     389,   407,   396,   351,   335,   352,   355,   406,   353,   303,
     409,   354,   356,     3,     4,     5,     6,   211,   215,   304,
     127,   111,   229,   313,   368,   320,   410,   114,     7,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,   315,   370,   321,   135,     0,     0,     0,
       0,     0,   199,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
       0,     0,   199,     0,     0,   199,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   199,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   199,   199,     0,     0,     0,     0,   199,     1,     2,
       3,     4,     5,     6,   145,   146,   147,     0,   148,   149,
     150,   151,   152,   153,   154,     7,     8,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,     0,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,   155,   156,   157,
     158,   159,     0,     0,   160,   161,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    78,    79,    80,
      81,    82,   162,   163,     0,     0,     0,     0,     0,     0,
     164,     0,     0,     0,     0,     0,   165,   166,   167,   168,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   169,   170,   171,     1,     2,     3,     4,     5,     6,
     145,   146,   147,     0,   148,   149,   150,   151,   152,   153,
     154,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,     0,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,   155,   156,   157,   158,   159,     0,     0,
     160,   161,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    78,    79,    80,    81,    82,   162,   163,
       0,     0,     0,     0,     0,     0,   164,     0,     0,     0,
       0,     0,   165,   166,   167,   168,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   169,   170,   253,
       1,     2,     3,     4,     5,     6,   145,   146,   147,     0,
     148,   149,   150,   151,   152,   153,   154,     7,     8,     9,
      10,    11,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,     0,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,   155,
     156,   157,   158,   159,     0,     0,   160,   161,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    78,
      79,    80,    81,    82,   162,   163,     0,     0,     0,     0,
       0,     0,   164,     0,     0,     0,     0,     0,   165,   166,
     167,   168,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   169,   170,   300,     1,     2,     3,     4,
       5,     6,   145,   146,   147,     0,   148,   149,   150,   151,
     152,   153,   154,     7,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,     0,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,   155,   156,   157,   158,   159,
       0,     0,   160,   161,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    78,    79,    80,    81,    82,
     162,   163,     0,     0,     0,     0,     0,     0,   164,     0,
       0,     0,     0,     0,   165,   166,   167,   168,     1,     2,
       3,     4,     5,     6,     0,     0,     0,     0,     0,   169,
     170,   331,     0,     0,     0,     7,     8,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,     0,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    78,    79,    80,
      81,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     1,     2,     3,
       4,     5,     6,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   234,     7,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,     0,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    78,    79,    80,    81,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     1,     2,     3,     4,     5,     6,   145,
     146,   147,   314,   148,   149,   150,   151,   152,   153,   154,
       7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,     0,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,   155,   156,   157,   158,   159,     0,     0,   160,
     161,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    78,    79,    80,    81,    82,   162,   163,     0,
       0,     0,     0,     0,     0,   164,     0,     0,     0,     0,
       0,   165,   166,   167,   168,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   169,   170,     1,     2,
       3,     4,     5,     6,   145,   146,   147,     0,   148,   149,
     150,   151,   152,   153,   154,     7,     8,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,     0,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,   155,   156,   157,
     158,   159,     0,     0,   160,   161,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    78,    79,    80,
      81,    82,   162,   163,     0,     0,     0,     0,     0,     0,
     164,     0,     0,     0,     0,     0,   165,   166,   167,   168,
       1,     2,     3,     4,     5,     6,     0,     0,     0,     0,
       0,   169,   113,     0,     0,     0,     0,     7,     8,     9,
      10,    11,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,     0,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,     0,
     156,   157,   158,   159,     0,     0,   160,   161,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    78,
      79,    80,    81,    82,   162,   163,     0,     0,     0,     0,
       0,     0,   164,     0,     0,     0,     0,     0,   165,   166,
     167,   168,     3,     4,     5,     6,     0,     0,     0,     0,
       0,     0,     0,   169,     0,     0,     0,     7,     8,     9,
      10,    11,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,     0,
     156,   157,   158,   159,     0,     0,   160,   161,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      79,    80,    81,     0,   162,   163,     0,     0,     0,     0,
       0,     0,   164,     0,     0,     0,     0,     0,   165,   166,
     167,   168,     1,     2,     3,     4,     5,     6,     0,     0,
       0,     0,     0,   243,     0,     0,     0,     0,     0,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,     0,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,     0,   156,   157,   158,   159,     0,     0,   160,   161,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    78,    79,    80,    81,     0,   162,   163,     0,     3,
       4,     5,     6,     0,   164,     0,     0,     0,     0,     0,
     165,   166,   167,   168,     7,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,     0,   156,   157,   158,
     159,     0,     0,   160,   161,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    79,    80,    81,
       0,   162,   163,     0,     3,     4,     5,     6,     0,   164,
       0,     0,   221,     0,     0,   165,   166,   167,   168,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,     0,   156,   157,   158,   159,     0,     0,   160,   161,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    79,    80,    81,     0,   162,   163,     0,     3,
       4,     5,     6,     0,   164,     0,     0,   307,     0,     0,
     165,   166,   167,   168,     7,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,     0,   156,   157,   158,
     159,     0,     0,   160,   161,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    79,    80,    81,
       0,   162,   163,     0,     3,     4,     5,     6,     0,   164,
       0,     0,   360,     0,     0,   165,   166,   167,   168,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,     0,   156,   157,   158,   159,     0,     0,   160,   161,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    79,    80,    81,     0,   162,   163,     0,     3,
       4,     5,     6,     0,   164,     0,     0,   366,     0,     0,
     165,   166,   167,   168,     7,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,     0,   156,   157,   158,
     159,     0,     0,   160,   161,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    79,    80,    81,
       0,   162,   163,     0,     3,     4,     5,     6,     0,   164,
       0,     0,     0,     0,     0,   165,   166,   167,   168,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
     262,     0,   156,   157,   158,   159,     0,     0,   160,   161,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    79,    80,    81,   134,   162,   163,     1,     2,
       3,     4,     5,     6,   164,     0,     0,     0,     0,     0,
     165,   166,   167,   168,     0,     7,     8,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,     0,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     1,     2,     3,     4,     5,
       6,     0,     0,     0,     0,     0,     0,    78,    79,    80,
      81,    82,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
       0,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     1,     2,     3,     4,     5,     6,     0,     0,
       0,     0,     0,     0,    78,    79,    80,    81,    82,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,     0,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     3,     4,     5,     6,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,     0,     0,     0,     0,     0,
       0,    78,    79,    80,    81,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,     0,   334,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     3,     4,     5,     6,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,     0,     0,     0,     0,
       0,     0,     0,    79,    80,    81,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    79,    80,    81
};

static const yytype_int16 yycheck[] =
{
       0,   147,   170,   151,   132,   153,   132,   110,   180,     0,
       0,    93,   142,   248,   106,   306,   164,     4,   101,   102,
      99,   100,   157,    91,    92,    93,   155,   160,   157,   160,
     218,   156,   160,   161,   155,   160,   139,   160,   375,   142,
     175,     4,   160,   176,   156,   176,   175,   219,   160,    36,
      37,    38,   156,   176,   182,   160,   174,    93,   204,   127,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     407,   176,   363,    36,    37,    38,   176,   177,   157,   160,
     159,   173,     3,     4,   157,   168,   169,   322,   379,   261,
     218,   382,   218,   241,    94,   177,    93,   245,   389,   229,
     248,   177,   102,   275,   160,   396,   175,   160,   254,   257,
     110,   102,   102,   113,    35,    36,    37,   305,    39,    40,
     176,   174,   122,    93,   100,   297,   229,   175,   316,    36,
      37,   176,   156,    40,   306,   111,   160,   156,    93,   139,
     140,   160,   142,   284,   285,   286,   287,   147,   296,   277,
     278,   279,   280,   281,   282,   283,   284,   285,   286,   287,
     288,   289,   290,   291,   292,   293,   294,   295,   156,   156,
     170,   359,   160,   176,   322,   176,   156,   305,   155,   305,
     160,   143,   144,   145,    36,    37,    38,   155,   316,   174,
     316,   363,   165,   166,   167,   161,   162,    97,    98,   103,
     104,   176,   280,   281,   204,   377,   155,   379,   282,   283,
     382,   288,   289,     3,     4,   215,   155,   389,   155,   170,
     388,   369,   156,   171,   396,   172,   372,   105,   107,   229,
     176,   359,   155,   359,    93,    95,   158,   385,   157,    92,
     240,   156,   175,   157,   155,    35,    36,    37,   248,    39,
      40,    41,    42,    43,   254,   401,   402,   158,   160,    93,
     156,   158,   175,   158,   158,   175,   158,   176,    12,   177,
     175,   156,   175,   290,   258,   291,   294,   176,   292,   215,
     178,   293,   295,     5,     6,     7,     8,   116,   123,   215,
      91,    82,   139,   228,   317,   240,   407,    84,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,   230,   322,   240,   102,    -1,    -1,    -1,
      -1,    -1,   322,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      -1,    -1,   372,    -1,    -1,   375,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   388,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   401,   402,    -1,    -1,    -1,    -1,   407,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    -1,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    -1,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    -1,    -1,    99,   100,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   142,   143,   144,
     145,   146,   147,   148,    -1,    -1,    -1,    -1,    -1,    -1,
     155,    -1,    -1,    -1,    -1,    -1,   161,   162,   163,   164,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   176,   177,   178,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    -1,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    -1,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    -1,    -1,
      99,   100,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   142,   143,   144,   145,   146,   147,   148,
      -1,    -1,    -1,    -1,    -1,    -1,   155,    -1,    -1,    -1,
      -1,    -1,   161,   162,   163,   164,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   176,   177,   178,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    -1,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    -1,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    -1,    -1,    99,   100,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   142,
     143,   144,   145,   146,   147,   148,    -1,    -1,    -1,    -1,
      -1,    -1,   155,    -1,    -1,    -1,    -1,    -1,   161,   162,
     163,   164,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   176,   177,   178,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    -1,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    -1,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      -1,    -1,    99,   100,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   142,   143,   144,   145,   146,
     147,   148,    -1,    -1,    -1,    -1,    -1,    -1,   155,    -1,
      -1,    -1,    -1,    -1,   161,   162,   163,   164,     3,     4,
       5,     6,     7,     8,    -1,    -1,    -1,    -1,    -1,   176,
     177,   178,    -1,    -1,    -1,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    -1,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   142,   143,   144,
     145,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,
       6,     7,     8,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   178,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    -1,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   142,   143,   144,   145,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     3,     4,     5,     6,     7,     8,     9,
      10,    11,   178,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    -1,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    -1,    -1,    99,
     100,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   142,   143,   144,   145,   146,   147,   148,    -1,
      -1,    -1,    -1,    -1,    -1,   155,    -1,    -1,    -1,    -1,
      -1,   161,   162,   163,   164,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   176,   177,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    -1,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    -1,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    -1,    -1,    99,   100,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   142,   143,   144,
     145,   146,   147,   148,    -1,    -1,    -1,    -1,    -1,    -1,
     155,    -1,    -1,    -1,    -1,    -1,   161,   162,   163,   164,
       3,     4,     5,     6,     7,     8,    -1,    -1,    -1,    -1,
      -1,   176,   177,    -1,    -1,    -1,    -1,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    -1,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    -1,
      93,    94,    95,    96,    -1,    -1,    99,   100,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   142,
     143,   144,   145,   146,   147,   148,    -1,    -1,    -1,    -1,
      -1,    -1,   155,    -1,    -1,    -1,    -1,    -1,   161,   162,
     163,   164,     5,     6,     7,     8,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   176,    -1,    -1,    -1,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    -1,
      93,    94,    95,    96,    -1,    -1,    99,   100,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     143,   144,   145,    -1,   147,   148,    -1,    -1,    -1,    -1,
      -1,    -1,   155,    -1,    -1,    -1,    -1,    -1,   161,   162,
     163,   164,     3,     4,     5,     6,     7,     8,    -1,    -1,
      -1,    -1,    -1,   176,    -1,    -1,    -1,    -1,    -1,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    -1,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    -1,    93,    94,    95,    96,    -1,    -1,    99,   100,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   142,   143,   144,   145,    -1,   147,   148,    -1,     5,
       6,     7,     8,    -1,   155,    -1,    -1,    -1,    -1,    -1,
     161,   162,   163,   164,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    -1,    93,    94,    95,
      96,    -1,    -1,    99,   100,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   143,   144,   145,
      -1,   147,   148,    -1,     5,     6,     7,     8,    -1,   155,
      -1,    -1,   158,    -1,    -1,   161,   162,   163,   164,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    -1,    93,    94,    95,    96,    -1,    -1,    99,   100,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   143,   144,   145,    -1,   147,   148,    -1,     5,
       6,     7,     8,    -1,   155,    -1,    -1,   158,    -1,    -1,
     161,   162,   163,   164,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    -1,    93,    94,    95,
      96,    -1,    -1,    99,   100,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   143,   144,   145,
      -1,   147,   148,    -1,     5,     6,     7,     8,    -1,   155,
      -1,    -1,   158,    -1,    -1,   161,   162,   163,   164,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    -1,    93,    94,    95,    96,    -1,    -1,    99,   100,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   143,   144,   145,    -1,   147,   148,    -1,     5,
       6,     7,     8,    -1,   155,    -1,    -1,   158,    -1,    -1,
     161,   162,   163,   164,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    -1,    93,    94,    95,
      96,    -1,    -1,    99,   100,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   143,   144,   145,
      -1,   147,   148,    -1,     5,     6,     7,     8,    -1,   155,
      -1,    -1,    -1,    -1,    -1,   161,   162,   163,   164,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    -1,    93,    94,    95,    96,    -1,    -1,    99,   100,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   143,   144,   145,     0,   147,   148,     3,     4,
       5,     6,     7,     8,   155,    -1,    -1,    -1,    -1,    -1,
     161,   162,   163,   164,    -1,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    -1,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,     3,     4,     5,     6,     7,
       8,    -1,    -1,    -1,    -1,    -1,    -1,   142,   143,   144,
     145,   146,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      -1,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     3,     4,     5,     6,     7,     8,    -1,    -1,
      -1,    -1,    -1,    -1,   142,   143,   144,   145,   146,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    -1,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,     5,     6,     7,     8,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    -1,    -1,    -1,    -1,    -1,
      -1,   142,   143,   144,   145,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    -1,    93,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     5,     6,     7,     8,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   143,   144,   145,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   143,   144,   145
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint16 yystos[] =
{
       0,     3,     4,     5,     6,     7,     8,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,   142,   143,
     144,   145,   146,   208,   209,   210,   211,   212,   217,   218,
     219,   220,   221,   222,   226,   227,   228,   229,   230,   231,
     232,   233,   258,   259,   260,    36,    37,    40,   155,    93,
     177,   232,   176,   177,   244,   156,   160,     4,    36,    37,
      38,   214,   215,   225,   160,   176,    93,   221,   227,   227,
     227,   228,   157,   229,     0,   259,    93,   223,   224,   177,
     226,   228,   234,   235,   229,     9,    10,    11,    13,    14,
      15,    16,    17,    18,    19,    92,    93,    94,    95,    96,
      99,   100,   147,   148,   155,   161,   162,   163,   164,   176,
     177,   178,   180,   181,   182,   184,   185,   186,   187,   188,
     189,   190,   191,   192,   193,   194,   195,   196,   197,   198,
     199,   200,   201,   202,   203,   204,   206,   208,   209,   228,
     239,   240,   241,   242,   245,   246,   247,   250,   252,   253,
     257,   214,   213,   216,   228,   215,    93,   155,   157,   175,
     227,   158,   190,   203,   207,   228,   175,   156,   160,   234,
     228,    93,   236,   237,   178,   235,   176,   176,   176,   240,
     155,   155,   176,   176,   206,   155,   206,   174,   155,   155,
     190,   190,   206,   178,   245,    99,   100,   157,   159,   156,
     156,   160,    91,   204,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   175,   205,   190,   165,   166,   167,
     161,   162,    97,    98,   101,   102,   168,   169,   103,   104,
     170,   171,   172,   105,   107,   106,   173,   160,   176,   155,
     178,   240,    93,   213,   216,   157,   175,   158,   207,   204,
     238,   158,    95,   224,   178,   236,   157,   160,   176,    92,
     239,   246,   254,   206,   176,   206,   174,   206,   219,   249,
     156,   178,   183,   206,    93,   186,   204,   204,   190,   190,
     190,   192,   192,   193,   193,   194,   194,   194,   194,   195,
     195,   196,   197,   198,   199,   200,   201,   206,   204,   157,
     158,   207,   238,   175,   158,   176,   158,   207,   237,   155,
     249,   255,   156,   156,    93,   156,   158,   174,   207,   175,
     158,   238,   175,   158,   206,   176,   240,   248,   177,   175,
     241,   243,   244,   204,   158,   238,   175,   238,   156,   206,
     256,    12,   245,   251,   238,   238,   176,   156,   240,   178,
     243
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  However,
   YYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define YYFAIL		goto yyerrlab
#if defined YYFAIL
  /* This is here to suppress warnings from the GCC cpp's
     -Wunused-macros.  Normally we don't worry about that warning, but
     some users do, and we want to make it easy for users to remove
     YYFAIL uses, which will produce warnings from Bison 2.5.  */
#endif

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}

/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{


    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:

/* Line 1464 of yacc.c  */
#line 325 "shader/glsl.y"
    { /*$$ = new Parser::Variable($1);*/ ;}
    break;

  case 4:

/* Line 1464 of yacc.c  */
#line 330 "shader/glsl.y"
    { (yyval.node) = new Parser::IntConstant(INTCONSTANT, (yyvsp[(1) - (1)].integer)); ;}
    break;

  case 5:

/* Line 1464 of yacc.c  */
#line 331 "shader/glsl.y"
    { (yyval.node) = new Parser::IntConstant(UINTCONSTANT, (yyvsp[(1) - (1)].integer)); ;}
    break;

  case 6:

/* Line 1464 of yacc.c  */
#line 332 "shader/glsl.y"
    { (yyval.node) = new Parser::FloatConstant((yyvsp[(1) - (1)].f)); ;}
    break;

  case 7:

/* Line 1464 of yacc.c  */
#line 333 "shader/glsl.y"
    { (yyval.node) = new Parser::IntConstant(BOOLCONSTANT, 1); ;}
    break;

  case 8:

/* Line 1464 of yacc.c  */
#line 334 "shader/glsl.y"
    { (yyval.node) = new Parser::IntConstant(BOOLCONSTANT, 0); ;}
    break;

  case 9:

/* Line 1464 of yacc.c  */
#line 335 "shader/glsl.y"
    { (yyval.node) = (yyvsp[(2) - (3)].node); ;}
    break;

  case 11:

/* Line 1464 of yacc.c  */
#line 341 "shader/glsl.y"
    { (yyval.node) = new Parser::ArrayField((yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].node)); ;}
    break;

  case 12:

/* Line 1464 of yacc.c  */
#line 342 "shader/glsl.y"
    { (yyval.node) = 0; ;}
    break;

  case 13:

/* Line 1464 of yacc.c  */
#line 343 "shader/glsl.y"
    { (yyval.node) = new Parser::Field((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].string)); ;}
    break;

  case 14:

/* Line 1464 of yacc.c  */
#line 344 "shader/glsl.y"
    { (yyval.node) = new Parser::Operator((yyvsp[(2) - (2)].integer), (yyvsp[(1) - (2)].node)); ;}
    break;

  case 15:

/* Line 1464 of yacc.c  */
#line 345 "shader/glsl.y"
    { (yyval.node) = new Parser::Operator((yyvsp[(2) - (2)].integer), (yyvsp[(1) - (2)].node)); ;}
    break;

  case 26:

/* Line 1464 of yacc.c  */
#line 383 "shader/glsl.y"
    { (yyval.node) = new Parser::Constructor((yyvsp[(1) - (2)].type)); ;}
    break;

  case 27:

/* Line 1464 of yacc.c  */
#line 384 "shader/glsl.y"
    { (yyval.node) = new Parser::Call( /*$1 */); ;}
    break;

  case 29:

/* Line 1464 of yacc.c  */
#line 400 "shader/glsl.y"
    { (yyval.node) = new Parser::Operator((yyvsp[(1) - (2)].integer), (yyvsp[(2) - (2)].node)); ;}
    break;

  case 30:

/* Line 1464 of yacc.c  */
#line 401 "shader/glsl.y"
    { (yyval.node) = new Parser::Operator((yyvsp[(1) - (2)].integer), (yyvsp[(2) - (2)].node)); ;}
    break;

  case 31:

/* Line 1464 of yacc.c  */
#line 402 "shader/glsl.y"
    { (yyval.node) = new Parser::Operator((yyvsp[(1) - (2)].integer), (yyvsp[(2) - (2)].node)); ;}
    break;

  case 32:

/* Line 1464 of yacc.c  */
#line 408 "shader/glsl.y"
    { (yyval.integer) = '+'; ;}
    break;

  case 33:

/* Line 1464 of yacc.c  */
#line 409 "shader/glsl.y"
    { (yyval.integer) = '-'; ;}
    break;

  case 34:

/* Line 1464 of yacc.c  */
#line 410 "shader/glsl.y"
    { (yyval.integer) = '!'; ;}
    break;

  case 35:

/* Line 1464 of yacc.c  */
#line 411 "shader/glsl.y"
    { (yyval.integer) = '~'; ;}
    break;

  case 37:

/* Line 1464 of yacc.c  */
#line 418 "shader/glsl.y"
    { (yyval.node) = new Parser::Operator('*', (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); ;}
    break;

  case 38:

/* Line 1464 of yacc.c  */
#line 419 "shader/glsl.y"
    { (yyval.node) = new Parser::Operator('/', (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); ;}
    break;

  case 39:

/* Line 1464 of yacc.c  */
#line 420 "shader/glsl.y"
    { (yyval.node) = new Parser::Operator('%', (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); ;}
    break;

  case 41:

/* Line 1464 of yacc.c  */
#line 425 "shader/glsl.y"
    { (yyval.node) = new Parser::Operator('+', (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); ;}
    break;

  case 42:

/* Line 1464 of yacc.c  */
#line 426 "shader/glsl.y"
    { (yyval.node) = new Parser::Operator('-', (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); ;}
    break;

  case 44:

/* Line 1464 of yacc.c  */
#line 431 "shader/glsl.y"
    { (yyval.node) = new Parser::Operator((yyvsp[(2) - (3)].integer), (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); ;}
    break;

  case 45:

/* Line 1464 of yacc.c  */
#line 432 "shader/glsl.y"
    { (yyval.node) = new Parser::Operator((yyvsp[(2) - (3)].integer), (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); ;}
    break;

  case 47:

/* Line 1464 of yacc.c  */
#line 437 "shader/glsl.y"
    { (yyval.node) = new Parser::Operator('<', (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); ;}
    break;

  case 48:

/* Line 1464 of yacc.c  */
#line 438 "shader/glsl.y"
    { (yyval.node) = new Parser::Operator('>', (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); ;}
    break;

  case 49:

/* Line 1464 of yacc.c  */
#line 439 "shader/glsl.y"
    { (yyval.node) = new Parser::Operator((yyvsp[(2) - (3)].integer), (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); ;}
    break;

  case 50:

/* Line 1464 of yacc.c  */
#line 440 "shader/glsl.y"
    { (yyval.node) = new Parser::Operator((yyvsp[(2) - (3)].integer), (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); ;}
    break;

  case 52:

/* Line 1464 of yacc.c  */
#line 445 "shader/glsl.y"
    { (yyval.node) = new Parser::Operator((yyvsp[(2) - (3)].integer), (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); ;}
    break;

  case 53:

/* Line 1464 of yacc.c  */
#line 446 "shader/glsl.y"
    { (yyval.node) = new Parser::Operator((yyvsp[(2) - (3)].integer), (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); ;}
    break;

  case 55:

/* Line 1464 of yacc.c  */
#line 451 "shader/glsl.y"
    { (yyval.node) = new Parser::Operator('&', (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); ;}
    break;

  case 57:

/* Line 1464 of yacc.c  */
#line 456 "shader/glsl.y"
    { (yyval.node) = new Parser::Operator('^', (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); ;}
    break;

  case 59:

/* Line 1464 of yacc.c  */
#line 461 "shader/glsl.y"
    { (yyval.node) = new Parser::Operator('|', (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); ;}
    break;

  case 61:

/* Line 1464 of yacc.c  */
#line 466 "shader/glsl.y"
    { (yyval.node) = new Parser::Operator((yyvsp[(2) - (3)].integer), (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); ;}
    break;

  case 63:

/* Line 1464 of yacc.c  */
#line 471 "shader/glsl.y"
    { (yyval.node) = new Parser::Operator((yyvsp[(2) - (3)].integer), (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); ;}
    break;

  case 65:

/* Line 1464 of yacc.c  */
#line 476 "shader/glsl.y"
    { (yyval.node) = new Parser::Operator((yyvsp[(2) - (3)].integer), (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); ;}
    break;

  case 67:

/* Line 1464 of yacc.c  */
#line 481 "shader/glsl.y"
    { (yyval.node) = new Parser::Conditional((yyvsp[(1) - (5)].node), (yyvsp[(3) - (5)].node), (yyvsp[(5) - (5)].node)); ;}
    break;

  case 69:

/* Line 1464 of yacc.c  */
#line 486 "shader/glsl.y"
    { (yyval.node) = new Parser::Operator((yyvsp[(2) - (3)].integer), (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); ;}
    break;

  case 70:

/* Line 1464 of yacc.c  */
#line 490 "shader/glsl.y"
    { (yyval.integer) = '='; ;}
    break;

  case 82:

/* Line 1464 of yacc.c  */
#line 505 "shader/glsl.y"
    { (yyval.node) = Parser::link((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); ;}
    break;

  case 84:

/* Line 1464 of yacc.c  */
#line 514 "shader/glsl.y"
    { (yyval.node) = (yyvsp[(1) - (2)].func); ;}
    break;

  case 85:

/* Line 1464 of yacc.c  */
#line 515 "shader/glsl.y"
    { (yyval.node) = (yyvsp[(1) - (2)].type); ;}
    break;

  case 86:

/* Line 1464 of yacc.c  */
#line 516 "shader/glsl.y"
    { (yyvsp[(3) - (4)].type)->setPrecision((yyvsp[(2) - (4)].integer)); (yyval.node) = (yyvsp[(3) - (4)].type); ;}
    break;

  case 90:

/* Line 1464 of yacc.c  */
#line 535 "shader/glsl.y"
    { (yyval.func) = (yyvsp[(1) - (2)].func); (yyval.func)->addParam((yyvsp[(2) - (2)].param)); ;}
    break;

  case 91:

/* Line 1464 of yacc.c  */
#line 536 "shader/glsl.y"
    { (yyval.func) = (yyvsp[(1) - (3)].func); (yyval.func)->addParam((yyvsp[(3) - (3)].param)); ;}
    break;

  case 92:

/* Line 1464 of yacc.c  */
#line 540 "shader/glsl.y"
    { (yyval.func) = new Parser::Function((yyvsp[(1) - (3)].type), (yyvsp[(2) - (3)].string)); ;}
    break;

  case 93:

/* Line 1464 of yacc.c  */
#line 544 "shader/glsl.y"
    { (yyval.param) = new Parser::Parameter((yyvsp[(1) - (2)].type), (yyvsp[(2) - (2)].string)); ;}
    break;

  case 94:

/* Line 1464 of yacc.c  */
#line 545 "shader/glsl.y"
    { (yyval.param) = new Parser::Parameter((yyvsp[(1) - (5)].type), (yyvsp[(2) - (5)].string), (yyvsp[(4) - (5)].node)); ;}
    break;

  case 95:

/* Line 1464 of yacc.c  */
#line 549 "shader/glsl.y"
    { (yyval.param) = (yyvsp[(3) - (3)].param); (yyval.param)->setTypeQualifier((yyvsp[(1) - (3)].integer)); (yyval.param)->setQualifier((yyvsp[(2) - (3)].integer)); ;}
    break;

  case 96:

/* Line 1464 of yacc.c  */
#line 550 "shader/glsl.y"
    { (yyval.param) = (yyvsp[(2) - (2)].param); (yyval.param)->setQualifier((yyvsp[(1) - (2)].integer)); ;}
    break;

  case 97:

/* Line 1464 of yacc.c  */
#line 551 "shader/glsl.y"
    { (yyval.param) = new Parser::Parameter((yyvsp[(3) - (3)].type)); (yyval.param)->setTypeQualifier((yyvsp[(1) - (3)].integer)); (yyval.param)->setQualifier((yyvsp[(2) - (3)].integer)); ;}
    break;

  case 98:

/* Line 1464 of yacc.c  */
#line 552 "shader/glsl.y"
    { (yyval.param) = new Parser::Parameter((yyvsp[(2) - (2)].type)); (yyval.param)->setQualifier((yyvsp[(1) - (2)].integer)); ;}
    break;

  case 99:

/* Line 1464 of yacc.c  */
#line 556 "shader/glsl.y"
    { (yyval.integer) = 0; ;}
    break;

  case 105:

/* Line 1464 of yacc.c  */
#line 568 "shader/glsl.y"
    { (yyval.type) = link((yyvsp[(1) - (3)].type), new Parser::Type(*(yyvsp[(1) - (3)].type))); (yyval.type)->setName((yyvsp[(3) - (3)].string)); ;}
    break;

  case 106:

/* Line 1464 of yacc.c  */
#line 569 "shader/glsl.y"
    { (yyval.type) = link((yyvsp[(1) - (5)].type), new Parser::Type(*(yyvsp[(1) - (5)].type))); (yyval.type)->setName((yyvsp[(3) - (5)].string)); (yyval.type)->setIsArray(); ;}
    break;

  case 107:

/* Line 1464 of yacc.c  */
#line 570 "shader/glsl.y"
    { (yyval.type) = link((yyvsp[(1) - (6)].type), new Parser::Type(*(yyvsp[(1) - (6)].type))); (yyval.type)->setName((yyvsp[(3) - (6)].string)); (yyval.type)->setArray((yyvsp[(5) - (6)].node)); ;}
    break;

  case 108:

/* Line 1464 of yacc.c  */
#line 571 "shader/glsl.y"
    { (yyval.type) = link((yyvsp[(1) - (7)].type), new Parser::Type(*(yyvsp[(1) - (7)].type))); (yyval.type)->setName((yyvsp[(3) - (7)].string)); (yyval.type)->setInitializer((yyvsp[(7) - (7)].node)); ;}
    break;

  case 109:

/* Line 1464 of yacc.c  */
#line 572 "shader/glsl.y"
    { (yyval.type) = Parser::link((yyvsp[(1) - (8)].type), new Parser::Type(*(yyvsp[(1) - (8)].type))); (yyval.type)->setName((yyvsp[(3) - (8)].string)); (yyval.type)->setArray((yyvsp[(5) - (8)].node)); (yyval.type)->setInitializer((yyvsp[(8) - (8)].node)); ;}
    break;

  case 110:

/* Line 1464 of yacc.c  */
#line 573 "shader/glsl.y"
    { (yyval.type) = Parser::link((yyvsp[(1) - (5)].type), new Parser::Type(*(yyvsp[(1) - (5)].type))); (yyval.type)->setName((yyvsp[(3) - (5)].string)); (yyval.type)->setInitializer((yyvsp[(5) - (5)].node)); ;}
    break;

  case 112:

/* Line 1464 of yacc.c  */
#line 578 "shader/glsl.y"
    { (yyval.type) = (yyvsp[(1) - (2)].type); (yyval.type)->setName((yyvsp[(2) - (2)].string)); ;}
    break;

  case 113:

/* Line 1464 of yacc.c  */
#line 579 "shader/glsl.y"
    { (yyval.type) = (yyvsp[(1) - (4)].type); (yyval.type)->setName((yyvsp[(2) - (4)].string)); (yyval.type)->setIsArray(); ;}
    break;

  case 114:

/* Line 1464 of yacc.c  */
#line 580 "shader/glsl.y"
    { (yyval.type) = (yyvsp[(1) - (5)].type); (yyval.type)->setName((yyvsp[(2) - (5)].string)); (yyval.type)->setArray((yyvsp[(4) - (5)].node)); ;}
    break;

  case 115:

/* Line 1464 of yacc.c  */
#line 581 "shader/glsl.y"
    { (yyval.type) = (yyvsp[(1) - (6)].type); (yyval.type)->setName((yyvsp[(2) - (6)].string)); (yyval.type)->setIsArray(); (yyval.type)->setInitializer((yyvsp[(6) - (6)].node)); ;}
    break;

  case 116:

/* Line 1464 of yacc.c  */
#line 582 "shader/glsl.y"
    { (yyval.type) = (yyvsp[(1) - (7)].type); (yyval.type)->setName((yyvsp[(2) - (7)].string)); (yyval.type)->setArray((yyvsp[(4) - (7)].node)); (yyval.type)->setInitializer((yyvsp[(7) - (7)].node)); ;}
    break;

  case 117:

/* Line 1464 of yacc.c  */
#line 583 "shader/glsl.y"
    { (yyval.type) = (yyvsp[(1) - (4)].type); (yyval.type)->setName((yyvsp[(2) - (4)].string)); (yyval.type)->setInitializer((yyvsp[(4) - (4)].node)); ;}
    break;

  case 119:

/* Line 1464 of yacc.c  */
#line 590 "shader/glsl.y"
    { (yyval.type) = (yyvsp[(2) - (2)].type); (yyval.type)->setQualifier((yyvsp[(1) - (2)].qualifier)); ;}
    break;

  case 124:

/* Line 1464 of yacc.c  */
#line 604 "shader/glsl.y"
    { (yyval.layout) = (yyvsp[(3) - (4)].layout); ;}
    break;

  case 126:

/* Line 1464 of yacc.c  */
#line 609 "shader/glsl.y"
    { (yyval.layout) = Parser::link((yyvsp[(1) - (3)].layout), (yyvsp[(3) - (3)].layout)); ;}
    break;

  case 127:

/* Line 1464 of yacc.c  */
#line 613 "shader/glsl.y"
    { /*$$ = new Layout($1);*/ ;}
    break;

  case 128:

/* Line 1464 of yacc.c  */
#line 614 "shader/glsl.y"
    { /*$$ = new Parser::Layout($1, $3);*/ ;}
    break;

  case 130:

/* Line 1464 of yacc.c  */
#line 622 "shader/glsl.y"
    { /* $$ = new Parser::Qualifier(); $$->add($1); */ ;}
    break;

  case 131:

/* Line 1464 of yacc.c  */
#line 623 "shader/glsl.y"
    { /* $$ = new Parser::Qualifier(); $$->setLayout($1); */ ;}
    break;

  case 132:

/* Line 1464 of yacc.c  */
#line 624 "shader/glsl.y"
    { /* $$ = new Parser::Qualifier(); $$->add($2); $$->setLayout($1); */ ;}
    break;

  case 133:

/* Line 1464 of yacc.c  */
#line 625 "shader/glsl.y"
    { /* $$ = new Parser::Qualifier(); $2->insert($1); $$->add($2); */ ;}
    break;

  case 134:

/* Line 1464 of yacc.c  */
#line 626 "shader/glsl.y"
    { /* $$ = new Parser::Qualifier(); $$->add(makeSet($1)); */ ;}
    break;

  case 135:

/* Line 1464 of yacc.c  */
#line 627 "shader/glsl.y"
    { /* $$ = new Parser::Qualifier(); $2->insert($1); $$->add($2); */ ;}
    break;

  case 136:

/* Line 1464 of yacc.c  */
#line 628 "shader/glsl.y"
    { /* $$ = new Parser::Qualifier(); $3->insert($1); $3->insert($2); $$->add($3); */ ;}
    break;

  case 137:

/* Line 1464 of yacc.c  */
#line 629 "shader/glsl.y"
    { /* $$ = new Parser::Qualifier(); $$->add(makeSet($1)); */ ;}
    break;

  case 138:

/* Line 1464 of yacc.c  */
#line 633 "shader/glsl.y"
    { (yyval.sset) = Parser::makeSet(CONST); ;}
    break;

  case 139:

/* Line 1464 of yacc.c  */
#line 634 "shader/glsl.y"
    { (yyval.sset) = Parser::makeSet(ATTRIBUTE); ;}
    break;

  case 140:

/* Line 1464 of yacc.c  */
#line 635 "shader/glsl.y"
    { (yyval.sset) = Parser::makeSet(VARYING); ;}
    break;

  case 141:

/* Line 1464 of yacc.c  */
#line 636 "shader/glsl.y"
    { (yyval.sset) = Parser::makeSet(CENTROID, VARYING); ;}
    break;

  case 142:

/* Line 1464 of yacc.c  */
#line 637 "shader/glsl.y"
    { (yyval.sset) = Parser::makeSet(IN); ;}
    break;

  case 143:

/* Line 1464 of yacc.c  */
#line 638 "shader/glsl.y"
    { (yyval.sset) = Parser::makeSet(OUT); ;}
    break;

  case 144:

/* Line 1464 of yacc.c  */
#line 639 "shader/glsl.y"
    { (yyval.sset) = Parser::makeSet(CENTROID, IN); ;}
    break;

  case 145:

/* Line 1464 of yacc.c  */
#line 640 "shader/glsl.y"
    { (yyval.sset) = Parser::makeSet(CENTROID, OUT); ;}
    break;

  case 146:

/* Line 1464 of yacc.c  */
#line 641 "shader/glsl.y"
    { (yyval.sset) = Parser::makeSet(UNIFORM); ;}
    break;

  case 148:

/* Line 1464 of yacc.c  */
#line 646 "shader/glsl.y"
    { /* $$ = $2; $$->setQualifier($1); */ ;}
    break;

  case 150:

/* Line 1464 of yacc.c  */
#line 651 "shader/glsl.y"
    { (yyval.type) = (yyvsp[(1) - (3)].type); (yyval.type)->setIsArray(); ;}
    break;

  case 151:

/* Line 1464 of yacc.c  */
#line 652 "shader/glsl.y"
    { (yyval.type) = (yyvsp[(1) - (4)].type); (yyval.type)->setArray((yyvsp[(3) - (4)].node)); ;}
    break;

  case 152:

/* Line 1464 of yacc.c  */
#line 656 "shader/glsl.y"
    { /* $$ = new Parser::Type($1); */ ;}
    break;

  case 222:

/* Line 1464 of yacc.c  */
#line 736 "shader/glsl.y"
    { /* $$ = new Parser::Struct($4, $2); */ ;}
    break;

  case 223:

/* Line 1464 of yacc.c  */
#line 737 "shader/glsl.y"
    { /* $$ = new Parser::Struct($3); */ ;}
    break;

  case 225:

/* Line 1464 of yacc.c  */
#line 742 "shader/glsl.y"
    { (yyval.type) = Parser::link((yyvsp[(1) - (2)].type), (yyvsp[(2) - (2)].type)); ;}
    break;

  case 226:

/* Line 1464 of yacc.c  */
#line 746 "shader/glsl.y"
    { /* FOR_EACH($2) { it->setType($1); } $$ = $2; */ ;}
    break;

  case 227:

/* Line 1464 of yacc.c  */
#line 747 "shader/glsl.y"
    { /* FOR_EACH($3) { $2->setQualifier($1); it->setType($2); } $$ = $3; */ ;}
    break;

  case 229:

/* Line 1464 of yacc.c  */
#line 752 "shader/glsl.y"
    { (yyval.type) = Parser::link((yyvsp[(1) - (3)].type), (yyvsp[(3) - (3)].type)); ;}
    break;

  case 230:

/* Line 1464 of yacc.c  */
#line 756 "shader/glsl.y"
    { (yyval.type) = new Parser::Type(); (yyval.type)->setName((yyvsp[(1) - (1)].string)); ;}
    break;

  case 231:

/* Line 1464 of yacc.c  */
#line 757 "shader/glsl.y"
    { (yyval.type) = new Parser::Type(); (yyval.type)->setName((yyvsp[(1) - (3)].string)); (yyval.type)->setIsArray(); ;}
    break;

  case 232:

/* Line 1464 of yacc.c  */
#line 758 "shader/glsl.y"
    { (yyval.type) = new Parser::Type(); (yyval.type)->setName((yyvsp[(1) - (4)].string)); (yyval.type)->setArray((yyvsp[(3) - (4)].node)); ;}
    break;

  case 244:

/* Line 1464 of yacc.c  */
#line 786 "shader/glsl.y"
    { (yyval.node) = 0; ;}
    break;

  case 245:

/* Line 1464 of yacc.c  */
#line 787 "shader/glsl.y"
    { (yyval.node) = (yyvsp[(2) - (3)].node); ;}
    break;

  case 248:

/* Line 1464 of yacc.c  */
#line 796 "shader/glsl.y"
    { (yyval.node) = 0; ;}
    break;

  case 249:

/* Line 1464 of yacc.c  */
#line 797 "shader/glsl.y"
    { (yyval.node) = (yyvsp[(2) - (3)].node); ;}
    break;

  case 251:

/* Line 1464 of yacc.c  */
#line 802 "shader/glsl.y"
    { (yyval.node) = Parser::link((yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].node)); ;}
    break;

  case 252:

/* Line 1464 of yacc.c  */
#line 806 "shader/glsl.y"
    { (yyval.node) = 0; ;}
    break;

  case 254:

/* Line 1464 of yacc.c  */
#line 811 "shader/glsl.y"
    { (yyval.node) = (yyvsp[(5) - (5)].node); ;}
    break;

  case 255:

/* Line 1464 of yacc.c  */
#line 815 "shader/glsl.y"
    { (yyval.node) = new Parser::If((yyvsp[(-1) - (3)].node), (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); ;}
    break;

  case 256:

/* Line 1464 of yacc.c  */
#line 816 "shader/glsl.y"
    { (yyval.node) = new Parser::If((yyvsp[(-1) - (1)].node), (yyvsp[(1) - (1)].node)); ;}
    break;

  case 258:

/* Line 1464 of yacc.c  */
#line 821 "shader/glsl.y"
    { (yyvsp[(1) - (4)].type)->setName((yyvsp[(2) - (4)].string)); (yyvsp[(1) - (4)].type)->setInitializer((yyvsp[(4) - (4)].node)); (yyval.node) = (yyvsp[(1) - (4)].type); ;}
    break;

  case 259:

/* Line 1464 of yacc.c  */
#line 825 "shader/glsl.y"
    { /* $$ = new Parser::Switch($3, $6); */ ;}
    break;

  case 260:

/* Line 1464 of yacc.c  */
#line 829 "shader/glsl.y"
    { (yyval.node) = 0; ;}
    break;

  case 262:

/* Line 1464 of yacc.c  */
#line 834 "shader/glsl.y"
    { /* $$ = new Parser::Label($2); */ ;}
    break;

  case 263:

/* Line 1464 of yacc.c  */
#line 835 "shader/glsl.y"
    { /* $$ = new Parser::Label(); */ ;}
    break;

  case 264:

/* Line 1464 of yacc.c  */
#line 839 "shader/glsl.y"
    { /* $$ = new Parser::While($3, $5); */ ;}
    break;

  case 265:

/* Line 1464 of yacc.c  */
#line 840 "shader/glsl.y"
    { /* $$ = new Parser::DoWhile($2, $5); */ ;}
    break;

  case 266:

/* Line 1464 of yacc.c  */
#line 842 "shader/glsl.y"
    { /* $$ = new Parser::For($3, $4, $6, $8); */ ;}
    break;

  case 269:

/* Line 1464 of yacc.c  */
#line 851 "shader/glsl.y"
    { (yyval.node) = 0; ;}
    break;

  case 271:

/* Line 1464 of yacc.c  */
#line 856 "shader/glsl.y"
    { (yyval.node) = 0; ;}
    break;

  case 273:

/* Line 1464 of yacc.c  */
#line 867 "shader/glsl.y"
    { (yyval.node) = new Parser::Jump((yyvsp[(1) - (2)].integer)); ;}
    break;

  case 274:

/* Line 1464 of yacc.c  */
#line 868 "shader/glsl.y"
    { (yyval.node) = new Parser::Jump((yyvsp[(1) - (2)].integer)); ;}
    break;

  case 275:

/* Line 1464 of yacc.c  */
#line 869 "shader/glsl.y"
    { (yyval.node) = new Parser::Jump((yyvsp[(1) - (2)].integer)); ;}
    break;

  case 276:

/* Line 1464 of yacc.c  */
#line 870 "shader/glsl.y"
    { (yyval.node) = new Parser::Jump((yyvsp[(1) - (3)].integer), (yyvsp[(2) - (3)].node)); ;}
    break;

  case 277:

/* Line 1464 of yacc.c  */
#line 871 "shader/glsl.y"
    { (yyval.node) = new Parser::Jump((yyvsp[(1) - (2)].integer)); ;}
    break;

  case 278:

/* Line 1464 of yacc.c  */
#line 876 "shader/glsl.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); if (!Parser::tree) { Parser::tree = (yyvsp[(1) - (1)].node); while (Parser::tree && Parser::tree->prev) Parser::tree = Parser::tree->prev; } ;}
    break;

  case 279:

/* Line 1464 of yacc.c  */
#line 877 "shader/glsl.y"
    { (yyval.node) = Parser::link((yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].node)); ;}
    break;

  case 282:

/* Line 1464 of yacc.c  */
#line 886 "shader/glsl.y"
    { /* $$ = $1; $1->setBody($2); */ ;}
    break;



/* Line 1464 of yacc.c  */
#line 3787 "glsl.tab.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 1684 of yacc.c  */
#line 889 "shader/glsl.y"


void yyerror(const char* str) {
  fprintf(stderr, "error - %s\n", str);
}

