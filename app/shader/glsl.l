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
#include "glsl_yacc.h"
static int s_column, s_line, s_pos;
%}

%option noyywrap

%x IN_COMMENT

%%

"/*" { BEGIN(IN_COMMENT); return COMMENT_BEGIN; }
^"#"[^\r\n]* { return PP; }
"//"[^\r\n]* { return COMMENT; }
"!="	{ return NE_OP; }
"&="	{ return AND_ASSIGN; }
"%="	{ return MOD_ASSIGN; }
"&&"	{ return AND_OP; }
"^^"	{ return XOR_OP; }
"*="	{ return MUL_ASSIGN; }
"++"	{ return INC_OP; }
"+="	{ return ADD_ASSIGN; }
"--"	{ return DEC_OP; }
"-="	{ return SUB_ASSIGN; }
"/="	{ return DIV_ASSIGN; }
"<<"	{ return LEFT_OP; }
"<<="	{ return LEFT_ASSIGN; }
"<="	{ return LE_OP; }
"=="	{ return EQ_OP; }
">="	{ return GE_OP; }
">>"	{ return RIGHT_OP; }
">>="	{ return RIGHT_ASSIGN; }
"^="	{ return XOR_ASSIGN; }
"|="	{ return OR_ASSIGN; }
"||"	{ return OR_OP; }
"attribute"	{ return ATTRIBUTE; }
"bool"	{ return BOOL; }
"break"	{ return BREAK; }
"bvec2"	{ return BVEC2; }
"bvec3"	{ return BVEC3; }
"bvec4"	{ return BVEC4; }
"case"	{ return CASE; }
"centroid"	{ return CENTROID; }
"const"	{ return CONST; }
"continue"	{ return CONTINUE; }
"default"	{ return DEFAULT; }
"discard"	{ return DISCARD; }
  /* "dmat2"	{ return DMAT2; }
"dmat2x2"	{ return DMAT2X2; }
"dmat2x3"	{ return DMAT2X3; }
"dmat2x4"	{ return DMAT2X4; }
"dmat3"	{ return DMAT3; }
"dmat3x2"	{ return DMAT3X2; }
"dmat3x3"	{ return DMAT3X3; }
"dmat3x4"	{ return DMAT3X4; }
"dmat4"	{ return DMAT4; }
"dmat4x2"	{ return DMAT4X2; }
"dmat4x3"	{ return DMAT4X3; }
"dmat4x4"	{ return DMAT4X4; } */
"do"	{ return DO; }
  /*"double"	{ return DOUBLE; }
"dvec2"	{ return DVEC2; }
"dvec3"	{ return DVEC3; }
"dvec4"	{ return DVEC4; } */
"else"	{ return ELSE; }
"flat"	{ return FLAT; }
"float"	{ return FLOAT; }
"for"	{ return FOR; }
"highp"	{ return HIGH_PRECISION; }
"if"	{ return IF; }
"in"	{ return IN; }
"inout"	{ return INOUT; }
"int"	{ return INT; }
"invariant"	{ return INVARIANT; }
"isampler1D"	{ return ISAMPLER1D; }
"isampler1DArray"	{ return ISAMPLER1DARRAY; }
"isampler2D"	{ return ISAMPLER2D; }
"isampler2DArray"	{ return ISAMPLER2DARRAY; }
"isampler2DMS"	{ return ISAMPLER2DMS; }
 /* "isampler2DMSArray"	{ return ISAMPLER2DMSArray; } */
"isampler2DRect"	{ return ISAMPLER2DRECT; }
"isampler3D"	{ return ISAMPLER3D; }
"isamplerBuffer"	{ return ISAMPLERBUFFER; }
"isamplerCube"	{ return ISAMPLERCUBE; }
 /* "isamplerCubeArray"	{ return ISAMPLERCUBEARRAY; } */
"ivec2"	{ return IVEC2; }
"ivec3"	{ return IVEC3; }
"ivec4"	{ return IVEC4; }
"layout"	{ return LAYOUT; }
"lowp"	{ return LOW_PRECISION; }
"mat2"	{ return MAT2; }
"mat2x2"	{ return MAT2X2; }
"mat2x3"	{ return MAT2X3; }
"mat2x4"	{ return MAT2X4; }
"mat3"	{ return MAT3; }
"mat3x2"	{ return MAT3X2; }
"mat3x3"	{ return MAT3X3; }
"mat3x4"	{ return MAT3X4; }
"mat4"	{ return MAT4; }
"mat4x2"	{ return MAT4X2; }
"mat4x3"	{ return MAT4X3; }
"mat4x4"	{ return MAT4X4; }
"mediump"	{ return MEDIUM_PRECISION; }
"noperspective"	{ return NOPERSPECTIVE; }
"out"	{ return OUT; }
 /* "patch"	{ return PATCH; } */
"precision"	{ return PRECISION; }
"return"	{ return RETURN; }
 /* "sample"	{ return SAMPLE; } */
"sampler1D"	{ return SAMPLER1D; }
"sampler1DArray"	{ return SAMPLER1DARRAY; }
"sampler1DArrayShadow"	{ return SAMPLER1DARRAYSHADOW; }
"sampler1DShadow"	{ return SAMPLER1DSHADOW; }
"sampler2D"	{ return SAMPLER2D; }
"sampler2DArray"	{ return SAMPLER2DARRAY; }
"sampler2DArrayShadow"	{ return SAMPLER2DARRAYSHADOW; }
"sampler2DMS"	{ return SAMPLER2DMS; }
 /* "sampler2DMSArray"	{ return SAMPLER2DMSArray; } */
"sampler2DRect"	{ return SAMPLER2DRECT; }
"sampler2DRectShadow"	{ return SAMPLER2DRECTSHADOW; }
"sampler2DShadow"	{ return SAMPLER2DSHADOW; }
"sampler3D"	{ return SAMPLER3D; }
"samplerBuffer"	{ return SAMPLERBUFFER; }
"samplerCube"	{ return SAMPLERCUBE; }
 /* "samplerCubeArray"	{ return SAMPLERCUBEARRAY; }
"samplerCubeArrayShadow"	{ return SAMPLERCUBEARRAYSHADOW; } */
"samplerCubeShadow"	{ return SAMPLERCUBESHADOW; }
"smooth"	{ return SMOOTH; }
"struct"	{ return STRUCT; }
 /* "subroutine"	{ return SUBROUTINE; } */
"switch"	{ return SWITCH; }
"uint"	{ return UINT; }
"uniform"	{ return UNIFORM; }
"usampler1D"	{ return USAMPLER1D; }
"usampler1DArray"	{ return USAMPLER1DARRAY; }
"usampler2D"	{ return USAMPLER2D; }
"usampler2DArray"	{ return USAMPLER2DARRAY; }
"usampler2DMS"	{ return USAMPLER2DMS; }
 /* "usampler2DMSArray"	{ return USAMPLER2DMSArray; } */
"usampler2DRect"	{ return USAMPLER2DRECT; }
"usampler3D"	{ return USAMPLER3D; }
"usamplerBuffer"	{ return USAMPLERBUFFER; }
"usamplerCube"	{ return USAMPLERCUBE; }
 /* "usamplerCubeArray"	{ return USAMPLERCUBEARRAY; } */
"uvec2"	{ return UVEC2; }
"uvec3"	{ return UVEC3; }
"uvec4"	{ return UVEC4; }
"varying"	{ return VARYING; }
"vec2"	{ return VEC2; }
"vec3"	{ return VEC3; }
"vec4"	{ return VEC4; }
"void"	{ return VOID; }
"while"	{ return WHILE; }

"true"	{ return TRUECONSTANT; }
"false"	{ return FALSECONSTANT; }

"!"	{ return '!'; }
"%"	{ return '%'; }
"&"	{ return '&'; }
"("	{ return '('; }
")"	{ return ')'; }
"*"	{ return '*'; }
"+"	{ return '+'; }
","	{ return ','; }
"-"	{ return '-'; }
"."	{ return '.'; }
"/"	{ return '/'; }
":"	{ return ':'; }
";"	{ return ';'; }
"<"	{ return '<'; }
"="	{ return '='; }
">"	{ return '>'; }
"?"	{ return '?'; }
"["	{ return '['; }
"]"	{ return ']'; }
"^"	{ return '^'; }
"{"	{ return '{'; }
"|"	{ return '|'; }
"}"	{ return '}'; }
"~"	{ return '~'; }

[_a-zA-Z][_a-zA-Z0-9]* { return IDENTIFIER; }

([1-9][0-9]*|0)[uU] { return UINTCONSTANT; }
[1-9][0-9]*|0 { return INTCONSTANT; }

0[0-7]+[uU] { return UINTCONSTANT; }
0[0-7]+ { return INTCONSTANT; }

0[xX][0-9a-fA-F]+[uU] { return UINTCONSTANT; }
0[xX][0-9a-fA-F]+ { return INTCONSTANT; }

([0-9]+"."[0-9]*|"."[0-9]+)([eE][+-]?[0-9]+)?[fF]? { return FLOATCONSTANT; }
[0-9]+[eE][+-]?[0-9]+[fF]? { return FLOATCONSTANT; }

[ \t\v\f]+ { s_column += yyleng; }

<IN_COMMENT>{
"*/" { BEGIN(INITIAL); return COMMENT_END; }
[^*\r\n]+ { s_column += yyleng; }
"*" { s_column += yyleng; }
}

<INITIAL,IN_COMMENT>[\r\n]|\r\n|\n\r { s_pos += s_column + yyleng; s_column = 0; s_line++; }

<INITIAL,IN_COMMENT>. { return ERROR; }

%%

int glsllex_wrapper(void) {
  int tmp = yylex();
  s_column += yyleng;
  return tmp;
}

void glslset_scan_string(const char *str, int len) {
  yy_scan_bytes(str, len);
  s_column = 0;
  s_line = 0;
  s_pos = 0;
}

int glslget_column(void) {
  return s_column;
}

int glslget_line(void) {
  return s_line;
}

int glslget_pos(void) {
  return s_pos+s_column;
}
