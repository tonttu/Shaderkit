#include "glpp.hpp"

#include <list>

#include "pp_yacc.h"
#include "pp_lex.hpp"

int ppparse(GLpp& parser);

void GLpp::scan(QByteArray data) {
  pplex_init(&yyscanner);
  pp_scan_bytes(data.data(), data.length(), yyscanner);
  ppparse(*this);
  pplex_destroy(yyscanner);
}

void GLpp::error(GLpp& /*parser*/, const char* str) {
  fprintf(stderr, "error - %s\n", str);
}
