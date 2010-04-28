#include "shader/error.hpp"

ShaderError::ShaderError() : m_line(0), m_column(0), m_length(0) {}
ShaderError::ShaderError(QString msg, QString type, int line, int column, int length)
  : m_msg(msg), m_type(type),
    m_line(line), m_column(column), m_length(length) {}
