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

#ifndef GLPP_HPP
#define GLPP_HPP

#include <QByteArray>

#include <string>
#include <map>
#include <list>
#include <vector>

#include "pp_yacc.h"

typedef void* yyscan_t;

class GLpp;
int ppparse(GLpp&);

class GLpp {
public:
  void scan(QByteArray data);
  const std::string & out() { return m_out; }

private:
  std::string m_out;
  yyscan_t yyscanner;

  typedef std::map<std::string, std::string> Objs;
  struct Func {
    std::vector<std::pair<std::string, int>> chunks;
  };
  typedef std::map<std::string, Func> Funcs;

  Objs m_objs;
  Funcs m_funcs;
  std::list<std::string> m_stack;

  int lex(YYSTYPE* lvalp);
  void error(GLpp& parser, const char* str);
  void pp_return(bool push, bool b);
  void push_string(const char* name, const char* str);
  void pop();


  friend int ppparse(GLpp&);
};

#endif // GLPP_HPP
