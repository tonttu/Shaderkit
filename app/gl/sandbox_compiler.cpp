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

#include "gl/sandbox_compiler.hpp"
#include "gl/shader.hpp"
#include "gl/opengl.hpp"
#include "gl/wrap_glext.h"

#include <QApplication>
#include <QTime>
#include <QMap>
#if QT_VERSION >= 0x050100
#include <QOpenGLContext>
#include <QOffscreenSurface>
#else
#include <QGLContext>
#endif

#include <poll.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include <cassert>
#include <cstring>

#include <unistd.h>

namespace
{

  struct Header {
    size_t length;
    int type;
  };

  int mainloop(QMap<GLenum, GLuint>& shaders, int readfd, int writefd)
  {
    Header header;
    std::vector<char> str;

    for (;;) {
      size_t got = 0;

      do {
        int r = read(readfd, (char*)&header + got, sizeof(header)-got);
        if (r == 0) return 0;
        if (r < 0 && (errno != EAGAIN && errno != EWOULDBLOCK)) return 1;
        if (r > 0) got += r;
      } while (got < sizeof(header));

      str.resize(header.length+1);
      char* data = &str.front();
      data[header.length] = '\0';

      got = 0;
      do {
        int r = read(readfd, data + got, header.length-got);
        if (r == 0) return 0;
        if (r < 0 && (errno != EAGAIN && errno != EWOULDBLOCK)) return 1;
        if (r > 0) got += r;
      } while (got < header.length);

      int s;
      if (!shaders.contains(header.type)) {
        s = glCreateShader(header.type);
        if (s == 0) {
          Shaderkit::Log::error("Sandbox Compiler: Failed to create shader of type %d", s);
          return 1;
        }
        shaders[header.type] = s;
      } else s = shaders[header.type];

      int len = header.length;
      const char* cdata = data;
      glShaderSource(s, 1, &cdata, &len);

      if (write(writefd, "W", 1) != 1) return 1;

      glCompileShader(s);
      /*
          char buffer[1024];
          len = 1024;
          glGetShaderInfoLog(s, len, &len, buffer);
          Log::info("Sandbox Compiler log: %s\n", buffer);*/

      if (write(writefd, "C", 1) != 1) return 1;
    }
  }

  bool timeoutWrite(int fd, double timeout, const char* data, int len)
  {
    QTime t;
    t.start();

    int timeleft = timeout * 1000;

    do {
      struct pollfd fds;
      fds.fd = fd;
      fds.events = POLLOUT;
      fds.revents = 0;
      if (poll(&fds, 1, timeleft) != 1) return false;

      int ret = write(fd, data, len);
      if (ret == len) return true;
      if (ret > 0) len -= ret, data += ret;
      if (ret < 0 && (errno != EAGAIN && errno != EWOULDBLOCK)) return false;

      timeleft = timeout * 1000 - t.elapsed();
    } while (timeleft > 0);

    return false;
  }

  int readResponse(int fd, double timeout)
  {
    QTime t;
    t.start();

    int timeleft = timeout * 1000;

    int got = 0, need = 2;
    do {
      struct pollfd fds;
      fds.fd = fd;
      fds.events = POLLIN;
      fds.revents = 0;
      if (poll(&fds, 1, timeleft) != 1) return got;

      char buffer[64];
      int ret = read(fd, buffer, sizeof(buffer));
      if (ret > 0) got += ret;
      if (got >= need || ret == 0) return got;
      if (ret < 0 && (errno != EAGAIN && errno != EWOULDBLOCK)) return got;

      timeleft = timeout * 1000 - t.elapsed();
    } while (timeleft > 0);

    return got;
  }

}

namespace Shaderkit
{

  SandboxCompiler* SandboxCompiler::s_instance = 0;

  SandboxCompiler::SandboxCompiler(const QString& binary)
    : m_read(-1), m_write(-1), m_pid(-1), m_binary(binary)
  {
    assert(!s_instance);
    s_instance = this;
  }

  SandboxCompiler::~SandboxCompiler()
  {
    killSandbox();
    s_instance = 0;
  }

  bool SandboxCompiler::check(ShaderPtr shader, QByteArray src, ShaderErrorList& errors)
  {
    assert(s_instance);
    return s_instance->doCheck(shader, src, errors);
  }

  void SandboxCompiler::close()
  {
    assert(s_instance);
    s_instance->killSandbox();
  }

  bool SandboxCompiler::doCheck(ShaderPtr shader, QByteArray src, ShaderErrorList& errors)
  {
    Header header;
    // Use memset instead of more c++ way of initializing the Header, since Header
    // isn't tightly packed, and there are some parts of Header that are left
    // uinitialized if we don't do this manually, and then valgrind will cry.
    memset(&header, 0, sizeof(header));
    header.length = src.length();
    header.type = shader->type();

    for (int t = 0; t < 2; ++t) {
      // If the initialization fails, we take the risk and run things without the sandbox.
      // This really isn't meant to be any kind of security thingy.
      if (m_pid <= 0 && !start()) {
        errors << ShaderError("Failed to initialize Sandbox Compiler", "warning", 0);
        return true;
      }

      bool writeok = timeoutWrite(m_write, 1.0, (const char*)&header, sizeof(header));
      if (writeok) writeok = timeoutWrite(m_write, 1.0, src.data(), src.length());
      if (writeok) {
        int r = readResponse(m_read, 3.0);
        if (r < 2) killSandbox();
        if (r == 0) {
          errors << ShaderError("Sandbox Compiler communication problem", "warning", 0);
        } else if (r == 1) {
          errors << ShaderError("Sandbox Compiler crashed when trying to compile this code", "error", 0);
          errors << ShaderError("Not compiling the shader because of possible driver bug", "warning", 0);
        }
        return r != 1;
      }
      killSandbox();
    }
    return true;
  }

  bool SandboxCompiler::start()
  {
    int fd1[2], fd2[2];
    if (pipe(fd1)) {
      Log::error("pipe(fd) failed: %s", strerror(errno));
      return false;
    }

    if (pipe(fd2)) {
      Log::error("pipe(fd) failed: %s", strerror(errno));
      ::close(fd1[0]);
      ::close(fd1[1]);
      return false;
    }

    Log::info("Starting a sandbox compiler");
    m_pid = fork();
    if (m_pid < 0) {
      Log::error("fork() failed: %s", strerror(errno));
      ::close(fd1[0]);
      ::close(fd1[1]);
      ::close(fd2[0]);
      ::close(fd2[1]);
      return false;
    }
    if (m_pid == 0) {
      ::close(fd1[1]);
      ::close(fd2[0]);
      char a[32], b[32];
      sprintf(a, "%d", fd1[0]);
      sprintf(b, "%d", fd2[1]);
      Log::info("Launching sandbox compiler");
      execl(m_binary.toUtf8().data(), m_binary.toUtf8().data(), "--sandbox-compiler", a, b, NULL);
      Log::error("Failed to start the sandbox compiler (%s): %s", m_binary.toUtf8().data(), strerror(errno));
      Log::info("Trying searching %s from PATH...", QCoreApplication::applicationFilePath().toUtf8().data());
      execlp(QCoreApplication::applicationFilePath().toUtf8().data(), m_binary.toUtf8().data(), "--sandbox-compiler", a, b, NULL);
      Log::error("Failed to start the sandbox compiler: %s", strerror(errno));
      exit(1);
    } else {
      ::close(fd1[0]);
      ::close(fd2[1]);
      m_read = fd2[0];
      m_write = fd1[1];
    }
    return true;
  }

  void SandboxCompiler::killSandbox()
  {
    if (m_pid <= 0) return;

    ::close(m_read);
    ::close(m_write);
    m_read = m_write = -1;

    int status;
    pid_t r = waitpid(m_pid, &status, WNOHANG);
    if (r < 0) {
      Log::error("waitpid failed: %s", strerror(errno));
    } else if (r != m_pid) {
      Log::info("Terminating Sandbox Compiler");
      kill(m_pid, SIGTERM);
      usleep(5000);
      if (waitpid(m_pid, &status, WNOHANG) != m_pid) {
        Log::info("Killing Sandbox Compiler");
        kill(m_pid, SIGKILL);
        waitpid(m_pid, &status, 0);
      }
    }
    m_pid = -1;
  }

  int SandboxCompiler::run(int& argc, char* argv[])
  {
    int readfd = QString(argv[2]).toInt();
    int writefd = QString(argv[3]).toInt();

    QApplication app(argc, argv);
    Log log;

    Log::info("Sandbox Compiler: Running on pid %d", getpid());

#if QT_VERSION >= 0x050100
    QOffscreenSurface surface;
    surface.create();

    QOpenGLContext context;
    context.create();
    context.makeCurrent(&surface);
#else
    QPixmap p(10, 10);
    QGLContext context(QGLFormat::defaultFormat(), &p);
    context.create();
    context.makeCurrent();
#endif

    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
      Log::error("Sandbox Compiler, GLEW: %s\n", glewGetErrorString(err));
      return 1;
    }
    Log::info("Sandbox Compiler: Using GLEW %s\n", glewGetString(GLEW_VERSION));

    wrap_gl_extensions();

    QMap<GLenum, GLuint> shaders;
    int ret = mainloop(shaders, readfd, writefd);
    ::close(readfd);
    ::close(writefd);
    foreach (GLuint s, shaders)
      glDeleteShader(s);
    Log::info("Sandbox Compiler closing");
    return ret;
  }

} // namespace Shaderkit
