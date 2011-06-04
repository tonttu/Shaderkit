/**
 * Copyright 2010,2011 Riku Palomäki.
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

#ifndef LEXER_HPP
#define LEXER_HPP

#include "glsl_yacc.h"

void glslset_scan_string(const char *str, int len, int state = 0);

int glslget_state();
int glsllex_wrapper(void);
int glslget_leng(void);
int glslget_column(void);
int glslget_line(void);
int glslget_pos(void);
char *glslget_text(void);

#endif // LEXER_HPP
