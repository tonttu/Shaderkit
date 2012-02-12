#ifndef GLSL_PARSER_HPP
#define GLSL_PARSER_HPP

#include "parser/glsl_pp.hpp"

class GLSLParser
{
public:
  GLSLParser();

  bool parse(QByteArray data);

  const GLSLpp& pp() const { return m_pp; }

private:
  GLSLpp m_pp;
};

#endif // GLSL_PARSER_HPP
