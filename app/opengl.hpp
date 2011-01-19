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

#ifndef OPENGL_HPP
#define OPENGL_HPP

#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/glew.h>
#include <QtOpenGL>

void _check_gl(const char* str, const char* file, int line);

template <typename T>
T _check_gl2(const char* str, const T& t, const char* file, int line) {
  _check_gl(str, file, line);
  return t;
}

#define glCheck(str) _check_gl(str, __FILE__, __LINE__)
#define glRun(cmd) do { (void)cmd; glCheck(#cmd); } while (false)
#define glRun2(cmd) _check_gl2(#cmd, cmd, __FILE__, __LINE__)

#endif // OPENGL_HPP
