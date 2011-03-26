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
