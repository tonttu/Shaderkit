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
#define YYPURE 1

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0

/* Substitute the variable and function names.  */
#define yyparse         ppparse
#define yylex           pplex
#define yyerror         pperror
#define yylval          pplval
#define yychar          ppchar
#define yydebug         ppdebug
#define yynerrs         ppnerrs


/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 1 "shader/pp.y"

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



/* Line 189 of yacc.c  */
#line 104 "pp.tab.c"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 1
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     HASH_ERROR = 258,
     DEFINE_OBJ = 259,
     DEFINE_FUNC = 260,
     DATA = 261,
     IDENTIFIER = 262,
     EXPAND_FUNC = 263,
     ARG = 264,
     CHUNK = 265,
     PASTE = 266,
     UNDEF = 267,
     DECIMAL = 268,
     NL = 269,
     HASH_VERSION = 270,
     HASH_EXTENSION = 271,
     HASH_PRAGMA = 272,
     HASH_INCLUDE = 273,
     HASH_LINE = 274,
     HASH_IFNDEF = 275,
     HASH_IFDEF = 276,
     HASH_IF = 277,
     HASH_ELIF = 278,
     REQUIRE = 279,
     DISABLE = 280,
     WARN = 281,
     ENABLE = 282,
     OR = 283,
     AND = 284,
     NOT_EQUAL = 285,
     EQUAL = 286,
     GREATER_OR_EQUAL = 287,
     LESS_OR_EQUAL = 288,
     RIGHT_SHIFT = 289,
     LEFT_SHIFT = 290,
     DEFINED = 291,
     UNARY = 292
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 32 "shader/pp.y"

  const char* string;
  std::string* stdstr;
  long integer;
  std::vector<std::string>* strlst;
  std::map<std::string, int>* tokenmap;
  std::list<std::pair<std::string, int> >* tokenlst;



/* Line 214 of yacc.c  */
#line 188 "pp.tab.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */

/* Line 264 of yacc.c  */
#line 44 "shader/pp.y"

#undef yylex
#undef yyerror
#define yylex parser.lex
#define yyerror parser.error

typedef std::pair<std::string, int> Token;
typedef std::list<Token> TokenList;



/* Line 264 of yacc.c  */
#line 212 "pp.tab.c"

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
#define YYFINAL  37
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   263

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  54
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  13
/* YYNRULES -- Number of rules.  */
#define YYNRULES  72
/* YYNRULES -- Number of states.  */
#define YYNSTATES  130

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   292

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    52,     2,     2,     2,    45,    32,     2,
      50,    49,    43,    41,    51,    42,     2,    44,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    48,     2,
      35,     2,    36,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    31,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    30,     2,    53,     2,     2,     2,
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
      25,    26,    27,    28,    29,    33,    34,    37,    38,    39,
      40,    46,    47
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint8 yyprhs[] =
{
       0,     0,     3,     4,     7,    12,    16,    22,    28,    34,
      40,    42,    44,    46,    50,    54,    58,    62,    64,    68,
      74,    79,    81,    82,    84,    86,    90,    92,    96,    98,
     100,   103,   106,   110,   112,   114,   116,   119,   122,   125,
     127,   129,   132,   135,   138,   139,   142,   145,   149,   151,
     153,   157,   161,   165,   169,   173,   177,   181,   185,   189,
     193,   197,   201,   205,   209,   213,   217,   221,   225,   228,
     231,   234,   237
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      55,     0,    -1,    -1,    56,    55,    -1,    15,    13,     7,
      14,    -1,    15,    13,    14,    -1,    16,     7,    48,    24,
      14,    -1,    16,     7,    48,    27,    14,    -1,    16,     7,
      48,    26,    14,    -1,    16,     7,    48,    25,    14,    -1,
      17,    -1,    18,    -1,    19,    -1,    22,    66,    14,    -1,
      23,    66,    14,    -1,    21,     7,    14,    -1,    20,     7,
      14,    -1,     3,    -1,     4,    64,    14,    -1,     5,    57,
      49,    63,    14,    -1,     8,    50,    59,    49,    -1,    12,
      -1,    -1,    58,    -1,     7,    -1,    57,    51,     7,    -1,
      60,    -1,    59,    51,    60,    -1,    61,    -1,     9,    -1,
      60,     9,    -1,    60,    61,    -1,    50,    62,    49,    -1,
      61,    -1,     9,    -1,    51,    -1,    62,     9,    -1,    62,
      61,    -1,    62,    51,    -1,    10,    -1,     7,    -1,    63,
      11,    -1,    63,    10,    -1,    63,     7,    -1,    -1,    65,
      64,    -1,    10,    64,    -1,    65,    11,     7,    -1,     7,
      -1,    13,    -1,    66,    28,    66,    -1,    66,    29,    66,
      -1,    66,    30,    66,    -1,    66,    31,    66,    -1,    66,
      32,    66,    -1,    66,    33,    66,    -1,    66,    34,    66,
      -1,    66,    37,    66,    -1,    66,    38,    66,    -1,    66,
      36,    66,    -1,    66,    35,    66,    -1,    66,    39,    66,
      -1,    66,    40,    66,    -1,    66,    42,    66,    -1,    66,
      41,    66,    -1,    66,    45,    66,    -1,    66,    44,    66,
      -1,    66,    43,    66,    -1,    52,    66,    -1,    53,    66,
      -1,    42,    66,    -1,    41,    66,    -1,    50,    66,    49,
      -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,    83,    83,    85,    89,    92,    95,    97,    99,   101,
     103,   104,   105,   106,   109,   112,   115,   118,   122,   126,
     145,   167,   174,   175,   179,   183,   190,   191,   195,   196,
     197,   198,   202,   212,   213,   214,   215,   216,   217,   221,
     225,   229,   230,   231,   235,   236,   237,   241,   245,   252,
     253,   254,   255,   256,   257,   258,   259,   260,   261,   262,
     263,   264,   265,   266,   267,   268,   275,   282,   283,   284,
     285,   286,   287
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "HASH_ERROR", "DEFINE_OBJ",
  "DEFINE_FUNC", "DATA", "IDENTIFIER", "EXPAND_FUNC", "ARG", "CHUNK",
  "PASTE", "UNDEF", "DECIMAL", "NL", "HASH_VERSION", "HASH_EXTENSION",
  "HASH_PRAGMA", "HASH_INCLUDE", "HASH_LINE", "HASH_IFNDEF", "HASH_IFDEF",
  "HASH_IF", "HASH_ELIF", "REQUIRE", "DISABLE", "WARN", "ENABLE", "OR",
  "AND", "'|'", "'^'", "'&'", "NOT_EQUAL", "EQUAL", "'<'", "'>'",
  "GREATER_OR_EQUAL", "LESS_OR_EQUAL", "RIGHT_SHIFT", "LEFT_SHIFT", "'+'",
  "'-'", "'*'", "'/'", "'%'", "DEFINED", "UNARY", "':'", "')'", "'('",
  "','", "'!'", "'~'", "$accept", "input", "stuff", "identifier_list",
  "identifier_list2", "list", "arg", "parg", "arg2", "data", "obj_data",
  "obj_data2", "expression", 0
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
     124,    94,    38,   285,   286,    60,    62,   287,   288,   289,
     290,    43,    45,    42,    47,    37,   291,   292,    58,    41,
      40,    44,    33,   126
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    54,    55,    55,    56,    56,    56,    56,    56,    56,
      56,    56,    56,    56,    56,    56,    56,    56,    56,    56,
      56,    56,    57,    57,    58,    58,    59,    59,    60,    60,
      60,    60,    61,    62,    62,    62,    62,    62,    62,    63,
      63,    63,    63,    63,    64,    64,    64,    65,    65,    66,
      66,    66,    66,    66,    66,    66,    66,    66,    66,    66,
      66,    66,    66,    66,    66,    66,    66,    66,    66,    66,
      66,    66,    66
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     2,     4,     3,     5,     5,     5,     5,
       1,     1,     1,     3,     3,     3,     3,     1,     3,     5,
       4,     1,     0,     1,     1,     3,     1,     3,     1,     1,
       2,     2,     3,     1,     1,     1,     2,     2,     2,     1,
       1,     2,     2,     2,     0,     2,     2,     3,     1,     1,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     2,     2,
       2,     2,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,    17,    44,    22,     0,    21,     0,     0,    10,    11,
      12,     0,     0,     0,     0,     0,     2,    48,    44,     0,
      44,    24,     0,    23,     0,     0,     0,     0,     0,    49,
       0,     0,     0,     0,     0,     0,     0,     1,     3,    46,
      18,     0,    45,     0,     0,    29,     0,     0,    26,    28,
       0,     5,     0,    16,    15,    71,    70,     0,    68,    69,
      13,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    14,
      47,    40,    39,     0,    25,    34,    35,    33,     0,    20,
       0,    30,    31,     4,     0,     0,     0,     0,    72,    50,
      51,    52,    53,    54,    55,    56,    60,    59,    57,    58,
      61,    62,    64,    63,    67,    66,    65,    43,    42,    41,
      19,    36,    32,    38,    37,    27,     6,     9,     8,     7
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,    15,    16,    22,    23,    47,    48,    49,    88,    83,
      19,    20,    35
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -47
static const yytype_int16 yypact[] =
{
     147,   -47,   214,    -3,   108,   -47,   225,   218,   -47,   -47,
     -47,   236,   238,    28,    28,   246,   147,   -47,   214,   233,
     150,   -47,   105,   -47,    16,   213,   200,   235,   237,   -47,
      28,    28,    28,    28,    28,    54,    86,   -47,   -47,   -47,
     -47,   243,   -47,   230,   245,   -47,    -4,   190,    17,   -47,
     239,   -47,   209,   -47,   -47,   -47,   -47,   104,   -47,   -47,
     -47,    28,    28,    28,    28,    28,    28,    28,    28,    28,
      28,    28,    28,    28,    28,    28,    28,    28,    28,   -47,
     -47,   -47,   -47,   212,   -47,   -47,   -47,   -47,    -6,   -47,
      16,   -47,   -47,   -47,   240,   241,   242,   244,   -47,   142,
     158,   173,    -5,    68,   -29,   -29,    32,    32,    32,    32,
     187,   187,   -21,   -21,   -47,   -47,   -47,   -47,   -47,   -47,
     -47,   -47,   -47,   -47,   -47,    17,   -47,   -47,   -47,   -47
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -47,   247,   -47,   -47,   -47,   -47,   167,   -46,   -47,   -47,
     224,   -47,   -13
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
      87,    36,    92,   121,    21,    85,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    55,    56,    57,
      58,    59,    76,    77,    78,    45,    91,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    29,   124,   122,    46,   123,    46,    86,    99,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,    46,    46,    60,    30,
      31,    72,    73,    74,    75,    76,    77,    78,    32,    92,
      33,    34,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
       1,     2,     3,    98,    43,     4,    44,    17,    24,     5,
      18,    41,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,   117,
      50,    17,   118,   119,    18,    26,   120,    51,    74,    75,
      76,    77,    78,    94,    95,    96,    97,    81,    25,    89,
      82,    90,    39,    27,    42,    28,    37,    40,    52,    53,
      80,    54,    84,    93,   126,   127,   128,   125,   129,     0,
       0,     0,     0,    38
};

static const yytype_int8 yycheck[] =
{
      46,    14,    48,     9,     7,     9,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    30,    31,    32,
      33,    34,    43,    44,    45,     9,     9,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    13,    88,    49,    50,    51,    50,    51,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    50,    50,    14,    41,
      42,    39,    40,    41,    42,    43,    44,    45,    50,   125,
      52,    53,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      14,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
       3,     4,     5,    49,    49,     8,    51,     7,    50,    12,
      10,    11,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,     7,
       7,     7,    10,    11,    10,     7,    14,    14,    41,    42,
      43,    44,    45,    24,    25,    26,    27,     7,    13,    49,
      10,    51,    18,     7,    20,     7,     0,    14,    48,    14,
       7,    14,     7,    14,    14,    14,    14,    90,    14,    -1,
      -1,    -1,    -1,    16
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,     4,     5,     8,    12,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    55,    56,     7,    10,    64,
      65,     7,    57,    58,    50,    13,     7,     7,     7,    13,
      41,    42,    50,    52,    53,    66,    66,     0,    55,    64,
      14,    11,    64,    49,    51,     9,    50,    59,    60,    61,
       7,    14,    48,    14,    14,    66,    66,    66,    66,    66,
      14,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    14,
       7,     7,    10,    63,     7,     9,    51,    61,    62,    49,
      51,     9,    61,    14,    24,    25,    26,    27,    49,    66,
      66,    66,    66,    66,    66,    66,    66,    66,    66,    66,
      66,    66,    66,    66,    66,    66,    66,     7,    10,    11,
      14,     9,    49,    51,    61,    60,    14,    14,    14,    14
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
      yyerror (parser, YY_("syntax error: cannot back up")); \
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
# define YYLEX yylex (&yylval, YYLEX_PARAM)
#else
# define YYLEX yylex (&yylval)
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
		  Type, Value, parser); \
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
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, GLpp& parser)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep, parser)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    GLpp& parser;
#endif
{
  if (!yyvaluep)
    return;
  YYUSE (parser);
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
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, GLpp& parser)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep, parser)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    GLpp& parser;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep, parser);
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
yy_reduce_print (YYSTYPE *yyvsp, int yyrule, GLpp& parser)
#else
static void
yy_reduce_print (yyvsp, yyrule, parser)
    YYSTYPE *yyvsp;
    int yyrule;
    GLpp& parser;
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
		       		       , parser);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule, parser); \
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
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, GLpp& parser)
#else
static void
yydestruct (yymsg, yytype, yyvaluep, parser)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
    GLpp& parser;
#endif
{
  YYUSE (yyvaluep);
  YYUSE (parser);

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
int yyparse (GLpp& parser);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */





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
yyparse (GLpp& parser)
#else
int
yyparse (parser)
    GLpp& parser;
#endif
#endif
{
/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

    /* Number of syntax errors so far.  */
    int yynerrs;

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
        case 4:

/* Line 1464 of yacc.c  */
#line 89 "shader/pp.y"
    {
    fprintf(stderr, "Got version %ld with profile %s\n", (yyvsp[(2) - (4)].integer), (yyvsp[(3) - (4)].string));
  ;}
    break;

  case 5:

/* Line 1464 of yacc.c  */
#line 92 "shader/pp.y"
    {
    fprintf(stderr, "Got version %ld with no profile info\n", (yyvsp[(2) - (3)].integer));
  ;}
    break;

  case 6:

/* Line 1464 of yacc.c  */
#line 95 "shader/pp.y"
    {
  ;}
    break;

  case 7:

/* Line 1464 of yacc.c  */
#line 97 "shader/pp.y"
    {
  ;}
    break;

  case 8:

/* Line 1464 of yacc.c  */
#line 99 "shader/pp.y"
    {
  ;}
    break;

  case 9:

/* Line 1464 of yacc.c  */
#line 101 "shader/pp.y"
    {
  ;}
    break;

  case 13:

/* Line 1464 of yacc.c  */
#line 106 "shader/pp.y"
    {
    parser.pp_return(true, (yyvsp[(2) - (3)].integer));
  ;}
    break;

  case 14:

/* Line 1464 of yacc.c  */
#line 109 "shader/pp.y"
    {
    parser.pp_return(false, (yyvsp[(2) - (3)].integer));
  ;}
    break;

  case 15:

/* Line 1464 of yacc.c  */
#line 112 "shader/pp.y"
    {
    parser.pp_return(true, parser.m_objs.count((yyvsp[(2) - (3)].string)) > 0 || parser.m_funcs.count((yyvsp[(2) - (3)].string)) > 0);
  ;}
    break;

  case 16:

/* Line 1464 of yacc.c  */
#line 115 "shader/pp.y"
    {
    parser.pp_return(true, parser.m_objs.count((yyvsp[(2) - (3)].string)) == 0 && parser.m_funcs.count((yyvsp[(2) - (3)].string)) == 0);
  ;}
    break;

  case 17:

/* Line 1464 of yacc.c  */
#line 118 "shader/pp.y"
    {
    fprintf(stderr, "ERROR: %s\n", (yyvsp[(1) - (1)].string));
    return -1;
  ;}
    break;

  case 18:

/* Line 1464 of yacc.c  */
#line 122 "shader/pp.y"
    {
    parser.m_objs[(yyvsp[(1) - (3)].string)] = (yyvsp[(2) - (3)].stdstr) ? *(yyvsp[(2) - (3)].stdstr) : "";
    fprintf(stderr, "Defined '%s' as '%s'\n", (yyvsp[(1) - (3)].string), (yyvsp[(2) - (3)].stdstr) ? (yyvsp[(2) - (3)].stdstr)->c_str() : 0);
  ;}
    break;

  case 19:

/* Line 1464 of yacc.c  */
#line 126 "shader/pp.y"
    {
    GLpp::Func& f = parser.m_funcs[(yyvsp[(1) - (5)].string)];
    f.chunks.reserve((yyvsp[(4) - (5)].tokenlst)->size());
    for (TokenList::iterator it = (yyvsp[(4) - (5)].tokenlst)->begin(); it != (yyvsp[(4) - (5)].tokenlst)->end(); ++it) {
      if ((yyvsp[(2) - (5)].tokenmap) && it->second == IDENTIFIER) {
        std::map<std::string, int>::iterator it2 = (yyvsp[(2) - (5)].tokenmap)->find(it->first);
        if (it2 != (yyvsp[(2) - (5)].tokenmap)->end()) {
          fprintf(stderr, "ID #%d: '%s'\n", it2->second, it->first.c_str());
          f.chunks.push_back(std::make_pair(it->first, it2->second));
          continue;
        }
      }
      fprintf(stderr, "Chunk: '%s'\n", it->first.c_str());
      f.chunks.push_back(std::make_pair(it->first, -1));
    }
    fprintf(stderr, "Defined '%s', %lu parameters, %lu chunks\n", (yyvsp[(1) - (5)].string), (yyvsp[(2) - (5)].tokenmap) ? (yyvsp[(2) - (5)].tokenmap)->size() : 0, f.chunks.size());
    delete (yyvsp[(2) - (5)].tokenmap);
    delete (yyvsp[(4) - (5)].tokenlst);
  ;}
    break;

  case 20:

/* Line 1464 of yacc.c  */
#line 145 "shader/pp.y"
    {
    parser.pop();
    fprintf(stderr, "Macro call %s of %lu arguments\n", (yyvsp[(1) - (4)].string), (yyvsp[(3) - (4)].strlst)->size());

    GLpp::Func& f = parser.m_funcs[(yyvsp[(1) - (4)].string)];
    std::string str;
    str.reserve(128);
    for (size_t i = 0; i < f.chunks.size(); ++i) {
      std::pair<std::string, int>& p = f.chunks[i];
      if (p.second == -1) {
        str += p.first;
      } else {
        if ((yyvsp[(3) - (4)].strlst)->size() >= size_t(p.second+1)) {
          str += (*(yyvsp[(3) - (4)].strlst))[p.second];
        } else {
          fprintf(stderr, "ERROR ERROR\n");
        }
      }
    }
    parser.push_string((yyvsp[(1) - (4)].string), str.c_str());
    delete (yyvsp[(3) - (4)].strlst);
  ;}
    break;

  case 21:

/* Line 1464 of yacc.c  */
#line 167 "shader/pp.y"
    {
    parser.m_funcs.erase((yyvsp[(1) - (1)].string));
    parser.m_objs.erase((yyvsp[(1) - (1)].string));
  ;}
    break;

  case 22:

/* Line 1464 of yacc.c  */
#line 174 "shader/pp.y"
    { (yyval.tokenmap) = 0; ;}
    break;

  case 23:

/* Line 1464 of yacc.c  */
#line 175 "shader/pp.y"
    { (yyval.tokenmap) = (yyvsp[(1) - (1)].tokenmap); ;}
    break;

  case 24:

/* Line 1464 of yacc.c  */
#line 179 "shader/pp.y"
    {
    (yyval.tokenmap) = new std::map<std::string, int>();
    (*(yyval.tokenmap))[(yyvsp[(1) - (1)].string)] = 0;
  ;}
    break;

  case 25:

/* Line 1464 of yacc.c  */
#line 183 "shader/pp.y"
    {
    (yyval.tokenmap) = (yyvsp[(1) - (3)].tokenmap);
    (*(yyval.tokenmap))[(yyvsp[(3) - (3)].string)] = (yyval.tokenmap)->size();
  ;}
    break;

  case 26:

/* Line 1464 of yacc.c  */
#line 190 "shader/pp.y"
    { (yyval.strlst) = new std::vector<std::string>(); (yyval.strlst)->push_back(*(yyvsp[(1) - (1)].stdstr)); delete (yyvsp[(1) - (1)].stdstr); ;}
    break;

  case 27:

/* Line 1464 of yacc.c  */
#line 191 "shader/pp.y"
    { (yyval.strlst) = (yyvsp[(1) - (3)].strlst); (yyval.strlst)->push_back(*(yyvsp[(3) - (3)].stdstr)); delete (yyvsp[(3) - (3)].stdstr); ;}
    break;

  case 28:

/* Line 1464 of yacc.c  */
#line 195 "shader/pp.y"
    { (yyval.stdstr) = (yyvsp[(1) - (1)].stdstr); ;}
    break;

  case 29:

/* Line 1464 of yacc.c  */
#line 196 "shader/pp.y"
    { (yyval.stdstr) = new std::string((yyvsp[(1) - (1)].string)); ;}
    break;

  case 30:

/* Line 1464 of yacc.c  */
#line 197 "shader/pp.y"
    { (yyval.stdstr) = (yyvsp[(1) - (2)].stdstr); (*(yyval.stdstr)) += (yyvsp[(2) - (2)].string); ;}
    break;

  case 31:

/* Line 1464 of yacc.c  */
#line 198 "shader/pp.y"
    { (yyval.stdstr) = (yyvsp[(1) - (2)].stdstr); (*(yyval.stdstr)) += *(yyvsp[(2) - (2)].stdstr); delete (yyvsp[(2) - (2)].stdstr); ;}
    break;

  case 32:

/* Line 1464 of yacc.c  */
#line 202 "shader/pp.y"
    {
    (yyval.stdstr) = new std::string;
    (*(yyval.stdstr)) += '(';
    (*(yyval.stdstr)) += *(yyvsp[(2) - (3)].stdstr);
    (*(yyval.stdstr)) += ')';
    delete (yyvsp[(2) - (3)].stdstr);
  ;}
    break;

  case 33:

/* Line 1464 of yacc.c  */
#line 212 "shader/pp.y"
    { (yyval.stdstr) = (yyvsp[(1) - (1)].stdstr); ;}
    break;

  case 34:

/* Line 1464 of yacc.c  */
#line 213 "shader/pp.y"
    { (yyval.stdstr) = new std::string((yyvsp[(1) - (1)].string)); ;}
    break;

  case 35:

/* Line 1464 of yacc.c  */
#line 214 "shader/pp.y"
    { (yyval.stdstr) = new std::string(","); ;}
    break;

  case 36:

/* Line 1464 of yacc.c  */
#line 215 "shader/pp.y"
    { (yyval.stdstr) = (yyvsp[(1) - (2)].stdstr); (*(yyval.stdstr)) += (yyvsp[(2) - (2)].string); ;}
    break;

  case 37:

/* Line 1464 of yacc.c  */
#line 216 "shader/pp.y"
    { (yyval.stdstr) = (yyvsp[(1) - (2)].stdstr); (*(yyval.stdstr)) += *(yyvsp[(2) - (2)].stdstr); delete (yyvsp[(2) - (2)].stdstr); ;}
    break;

  case 38:

/* Line 1464 of yacc.c  */
#line 217 "shader/pp.y"
    { (yyval.stdstr) = (yyvsp[(1) - (2)].stdstr); (*(yyval.stdstr)) += ","; ;}
    break;

  case 39:

/* Line 1464 of yacc.c  */
#line 221 "shader/pp.y"
    {
    (yyval.tokenlst) = new TokenList();
    (yyval.tokenlst)->push_back(Token((yyvsp[(1) - (1)].string), CHUNK));
  ;}
    break;

  case 40:

/* Line 1464 of yacc.c  */
#line 225 "shader/pp.y"
    {
    (yyval.tokenlst) = new TokenList();
    (yyval.tokenlst)->push_back(Token((yyvsp[(1) - (1)].string), IDENTIFIER));
  ;}
    break;

  case 41:

/* Line 1464 of yacc.c  */
#line 229 "shader/pp.y"
    { (yyval.tokenlst) = (yyvsp[(1) - (2)].tokenlst); (yyval.tokenlst)->push_back(Token("", PASTE)); ;}
    break;

  case 42:

/* Line 1464 of yacc.c  */
#line 230 "shader/pp.y"
    { /** @todo merge */ (yyval.tokenlst) = (yyvsp[(1) - (2)].tokenlst); (yyval.tokenlst)->push_back(Token((yyvsp[(2) - (2)].string), CHUNK)); ;}
    break;

  case 43:

/* Line 1464 of yacc.c  */
#line 231 "shader/pp.y"
    { (yyval.tokenlst) = (yyvsp[(1) - (2)].tokenlst); (yyval.tokenlst)->push_back(Token((yyvsp[(2) - (2)].string), IDENTIFIER)); ;}
    break;

  case 44:

/* Line 1464 of yacc.c  */
#line 235 "shader/pp.y"
    { (yyval.stdstr) = 0; ;}
    break;

  case 45:

/* Line 1464 of yacc.c  */
#line 236 "shader/pp.y"
    { (yyval.stdstr) = (yyvsp[(1) - (2)].stdstr); if ((yyvsp[(2) - (2)].stdstr)) (*(yyval.stdstr)) += *(yyvsp[(2) - (2)].stdstr); ;}
    break;

  case 46:

/* Line 1464 of yacc.c  */
#line 237 "shader/pp.y"
    { (yyval.stdstr) = new std::string((yyvsp[(1) - (2)].string)); if ((yyvsp[(2) - (2)].stdstr)) (*(yyval.stdstr)) += *(yyvsp[(2) - (2)].stdstr); ;}
    break;

  case 47:

/* Line 1464 of yacc.c  */
#line 241 "shader/pp.y"
    {
    (yyval.stdstr) = (yyvsp[(1) - (3)].stdstr);
    (*(yyval.stdstr)) += (yyvsp[(3) - (3)].string);
  ;}
    break;

  case 48:

/* Line 1464 of yacc.c  */
#line 245 "shader/pp.y"
    { (yyval.stdstr) = new std::string((yyvsp[(1) - (1)].string)); ;}
    break;

  case 49:

/* Line 1464 of yacc.c  */
#line 252 "shader/pp.y"
    { (yyval.integer) = (yyvsp[(1) - (1)].integer); ;}
    break;

  case 50:

/* Line 1464 of yacc.c  */
#line 253 "shader/pp.y"
    { (yyval.integer) = (yyvsp[(1) - (3)].integer) || (yyvsp[(3) - (3)].integer); ;}
    break;

  case 51:

/* Line 1464 of yacc.c  */
#line 254 "shader/pp.y"
    { (yyval.integer) = (yyvsp[(1) - (3)].integer) && (yyvsp[(3) - (3)].integer); ;}
    break;

  case 52:

/* Line 1464 of yacc.c  */
#line 255 "shader/pp.y"
    { (yyval.integer) = (yyvsp[(1) - (3)].integer) | (yyvsp[(3) - (3)].integer); ;}
    break;

  case 53:

/* Line 1464 of yacc.c  */
#line 256 "shader/pp.y"
    { (yyval.integer) = (yyvsp[(1) - (3)].integer) ^ (yyvsp[(3) - (3)].integer); ;}
    break;

  case 54:

/* Line 1464 of yacc.c  */
#line 257 "shader/pp.y"
    { (yyval.integer) = (yyvsp[(1) - (3)].integer) & (yyvsp[(3) - (3)].integer); ;}
    break;

  case 55:

/* Line 1464 of yacc.c  */
#line 258 "shader/pp.y"
    { (yyval.integer) = (yyvsp[(1) - (3)].integer) != (yyvsp[(3) - (3)].integer); ;}
    break;

  case 56:

/* Line 1464 of yacc.c  */
#line 259 "shader/pp.y"
    { (yyval.integer) = (yyvsp[(1) - (3)].integer) == (yyvsp[(3) - (3)].integer); ;}
    break;

  case 57:

/* Line 1464 of yacc.c  */
#line 260 "shader/pp.y"
    { (yyval.integer) = (yyvsp[(1) - (3)].integer) >= (yyvsp[(3) - (3)].integer); ;}
    break;

  case 58:

/* Line 1464 of yacc.c  */
#line 261 "shader/pp.y"
    { (yyval.integer) = (yyvsp[(1) - (3)].integer) <= (yyvsp[(3) - (3)].integer); ;}
    break;

  case 59:

/* Line 1464 of yacc.c  */
#line 262 "shader/pp.y"
    { (yyval.integer) = (yyvsp[(1) - (3)].integer) > (yyvsp[(3) - (3)].integer); ;}
    break;

  case 60:

/* Line 1464 of yacc.c  */
#line 263 "shader/pp.y"
    { (yyval.integer) = (yyvsp[(1) - (3)].integer) < (yyvsp[(3) - (3)].integer); ;}
    break;

  case 61:

/* Line 1464 of yacc.c  */
#line 264 "shader/pp.y"
    { (yyval.integer) = (yyvsp[(1) - (3)].integer) >> (yyvsp[(3) - (3)].integer); ;}
    break;

  case 62:

/* Line 1464 of yacc.c  */
#line 265 "shader/pp.y"
    { (yyval.integer) = (yyvsp[(1) - (3)].integer) << (yyvsp[(3) - (3)].integer); ;}
    break;

  case 63:

/* Line 1464 of yacc.c  */
#line 266 "shader/pp.y"
    { (yyval.integer) = (yyvsp[(1) - (3)].integer) - (yyvsp[(3) - (3)].integer); ;}
    break;

  case 64:

/* Line 1464 of yacc.c  */
#line 267 "shader/pp.y"
    { (yyval.integer) = (yyvsp[(1) - (3)].integer) + (yyvsp[(3) - (3)].integer); ;}
    break;

  case 65:

/* Line 1464 of yacc.c  */
#line 268 "shader/pp.y"
    {
    if ((yyvsp[(3) - (3)].integer) == 0) {
      yyerror(parser, "zero modulus in preprocessor directive");
    } else {
      (yyval.integer) = (yyvsp[(1) - (3)].integer) % (yyvsp[(3) - (3)].integer);
    }
  ;}
    break;

  case 66:

/* Line 1464 of yacc.c  */
#line 275 "shader/pp.y"
    {
    if ((yyvsp[(3) - (3)].integer) == 0) {
      yyerror(parser, "division by 0 in preprocessor directive");
    } else {
      (yyval.integer) = (yyvsp[(1) - (3)].integer) / (yyvsp[(3) - (3)].integer);
    }
  ;}
    break;

  case 67:

/* Line 1464 of yacc.c  */
#line 282 "shader/pp.y"
    { (yyval.integer) = (yyvsp[(1) - (3)].integer) * (yyvsp[(3) - (3)].integer); ;}
    break;

  case 68:

/* Line 1464 of yacc.c  */
#line 283 "shader/pp.y"
    { (yyval.integer) = ! (yyvsp[(2) - (2)].integer); ;}
    break;

  case 69:

/* Line 1464 of yacc.c  */
#line 284 "shader/pp.y"
    { (yyval.integer) = ~ (yyvsp[(2) - (2)].integer); ;}
    break;

  case 70:

/* Line 1464 of yacc.c  */
#line 285 "shader/pp.y"
    { (yyval.integer) = - (yyvsp[(2) - (2)].integer); ;}
    break;

  case 71:

/* Line 1464 of yacc.c  */
#line 286 "shader/pp.y"
    { (yyval.integer) = + (yyvsp[(2) - (2)].integer); ;}
    break;

  case 72:

/* Line 1464 of yacc.c  */
#line 287 "shader/pp.y"
    { (yyval.integer) = (yyvsp[(2) - (3)].integer); ;}
    break;



/* Line 1464 of yacc.c  */
#line 2122 "pp.tab.c"
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
      yyerror (parser, YY_("syntax error"));
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
	    yyerror (parser, yymsg);
	  }
	else
	  {
	    yyerror (parser, YY_("syntax error"));
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
		      yytoken, &yylval, parser);
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
		  yystos[yystate], yyvsp, parser);
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
  yyerror (parser, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval, parser);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp, parser);
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
#line 290 "shader/pp.y"


