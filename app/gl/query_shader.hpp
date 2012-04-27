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

#ifndef QUERY_SHADER_HPP
#define QUERY_SHADER_HPP

#include "gl/shader.hpp"
#include "gl/transform_feedback.hpp"

#include <QString>

namespace Shaderkit {

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

} // namespace Shaderkit

#endif // QUERY_SHADER_HPP
