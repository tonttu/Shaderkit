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

#ifndef SHADER_SANDBOX_COMPILER_HPP
#define SHADER_SANDBOX_COMPILER_HPP

#include "forward.hpp"
#include "gl/error.hpp"

namespace Shaderkit {

/// @todo implement something on windows too
class SandboxCompiler {
public:
  SandboxCompiler(const QString& binary);
  ~SandboxCompiler();

  static bool check(ShaderPtr shader, QByteArray src, ShaderErrorList& errors);
  static void close();

  static int run(int& argc, char* argv[]);

private:
  static SandboxCompiler* s_instance;

  int m_read, m_write;
  pid_t m_pid;

  const QString m_binary;

  bool doCheck(ShaderPtr shader, QByteArray src, ShaderErrorList& errors);
  bool start();
  void killSandbox();
};

} // namespace Shaderkit

#endif
