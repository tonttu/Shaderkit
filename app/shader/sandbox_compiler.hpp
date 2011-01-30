/**
 * Copyright 2010,2011 Riku Palomäki.
 * This file is part of GLSL Lab.
 *
 * GLSL Lab is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License Version 3 as
 * published by the Free Software Foundation.
 *
 * GLSL Lab is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GLSL Lab.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SHADER_SANDBOX_COMPILER_HPP
#define SHADER_SANDBOX_COMPILER_HPP

#include "forward.hpp"
#include "error.hpp"

/// @todo implement something on windows too
class SandboxCompiler {
public:
  SandboxCompiler(const char* argv0);
  ~SandboxCompiler();

  static bool check(ShaderPtr shader, QByteArray src, ShaderError::List& errors);
  static void close();

  static void init(const char* argv0);
  static int run(int readfd, int writefd);

private:
  static SandboxCompiler* s_instance;

  int m_read, m_write;
  pid_t m_pid;

  std::string m_argv0, m_cwd;

  bool doCheck(ShaderPtr shader, QByteArray src, ShaderError::List& errors);
  bool start();
  void killSandbox();
};

#endif
