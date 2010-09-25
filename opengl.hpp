#ifndef OPENGL_HPP
#define OPENGL_HPP

#ifdef _WIN32
#include <windows.h>
#endif
//#include <GL/glew.h>
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
