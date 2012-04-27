#include "parser/glsl_parser.hpp"

namespace Shaderkit
{

  GLSLParser::GLSLParser()
  {
  }

  bool GLSLParser::parse(QByteArray data)
  {
    m_pp.reset();
    return m_pp.parse(data);
  }

  Shader::Type GLSLParser::detect(const QString& /*filename*/)
  {
    /// @todo Implement
    return Shader::Unknown;
  }

} // namespace Shaderkit
