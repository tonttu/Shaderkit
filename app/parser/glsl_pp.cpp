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

#include "parser/glsl_pp.hpp"

#include <list>

#include "pp_yacc.h"
#include "pp_lex.hpp"

int ppparse(GLSLpp& parser);

GLSLpp::GLSLpp()
  : yyscanner(0), m_version(0), m_current_macro("", "", ""),
    m_macro_line(0), m_last_status(true), m_depth(0) {}

void GLSLpp::reset() {
  m_out.clear();

  m_objs.clear();
  m_funcs.clear();

  m_version = 0;
  m_profile.clear();

  m_extensions.clear();
  m_pragmas.clear();
  m_macros.clear();

  m_current_macro = MacroValue("", "", "");
  m_macro_line = 0;
  while (!m_macro_stack.empty())
    m_macro_stack.pop();

  m_undefs.clear();
  m_require.clear();

  m_line_values.clear();
  m_last_status = true;

  m_func.clear();
  m_args.clear();
  m_txt.clear();
  m_depth = 0;
}

bool GLSLpp::parse(QByteArray data) {
  pplex_init(&yyscanner);
  pp_scan_bytes(data.data(), data.length(), yyscanner);
  ppset_lineno(0, yyscanner);
  int r = ppparse(*this);
  fillLineValues();
  pplex_destroy(yyscanner);
  yyscanner = 0;

  return r == 0;
}

void GLSLpp::error(GLSLpp& /*parser*/, const char* str) {
  fprintf(stderr, "%d: error - %s\n", line()+1, str);
}
