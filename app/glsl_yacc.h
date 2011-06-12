/* A Bison parser, made by GNU Bison 2.4.2.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
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

/* Line 1685 of yacc.c  */
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



/* Line 1685 of yacc.c  */
#line 220 "glsl.tab.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE glsllval;


