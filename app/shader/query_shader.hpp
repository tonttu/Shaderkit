#ifndef QUERY_SHADER_HPP
#define QUERY_SHADER_HPP

#include "shader/shader.hpp"
#include "transform_feedback.hpp"

#include <QString>

class QueryShader {
public:
  static QueryShader& instance();

  bool compile(Shader::Type type, QString src);
  bool bind(const char* name);
  bool unbind(float& v);

private:
  QueryShader();

  static QueryShader* s_instance;

  TransformFeedback m_feedback;
  GLuint m_shader, m_prog;
  bool m_attached;
};

#endif // QUERY_SHADER_HPP
