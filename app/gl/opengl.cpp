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

#include "forward.hpp"
#include "gl/opengl.hpp"

#include <QMap>

void _check_gl(const char* str, const char* file, int line) {
  static QMap<QString, int> s_errors;
  const int limit = 3;
  int e = glGetError();
  if (e != GL_NO_ERROR) {
    QString out = "%1:%2\t%3 (%4)";
    out = out.arg(file).arg(line).arg((const char*)(gluErrorString(e))).arg(str);
    int num = ++s_errors[out];
    if (num == limit) out += " (starting to ignore)";
    if (num <= limit) Log::error(out);
    //abort();
  }
}
