/**
 * Copyright 2010-2012 Riku Palomäki.
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

#include "parser/glpp.hpp"

#include <list>

#include "pp_yacc.h"
#include "pp_lex.hpp"

int ppparse(GLpp& parser);

GLpp::GLpp() : yyscanner(0), m_version(0), m_current_macro("", ""), m_macro_line(0) {}

void GLpp::scan(QByteArray data) {
  m_version = 0;
  m_profile = "";
  m_extensions.clear();
  m_pragmas.clear();

  pplex_init(&yyscanner);
  pp_scan_bytes(data.data(), data.length(), yyscanner);
  ppset_lineno(1, yyscanner);
  ppparse(*this);
  pplex_destroy(yyscanner);
}

void GLpp::error(GLpp& /*parser*/, const char* str) {
  fprintf(stderr, "error - %s\n", str);
}
