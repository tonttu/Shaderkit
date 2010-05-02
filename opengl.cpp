#include "opengl.hpp"

void _check_gl(const char* str, const char* file, int line) {
  int e = glGetError();
  if (e != GL_NO_ERROR) {
    fprintf(stderr, "%s:%d\t%s (%s)\n", file, line, gluErrorString(e), str);
    //abort();
  }
}
