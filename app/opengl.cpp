#include "opengl.hpp"

#include <QMap>
#include <iostream>

void _check_gl(const char* str, const char* file, int line) {
  static QMap<QString, int> s_errors;
  const int limit = 3;
  int e = glGetError();
  if (e != GL_NO_ERROR) {
    QString out = "%1:%2\t%3 (%4)";
    out = out.arg(file).arg(line).arg((const char*)(gluErrorString(e))).arg(str);
    int num = ++s_errors[out];
    if (num == limit) out += " (starting to ignore)";
    if (num <= limit) std::cerr << out.toStdString() << std::endl;
    //abort();
  }
}
