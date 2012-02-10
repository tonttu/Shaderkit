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

#ifndef LOG_HPP
#define LOG_HPP

#include <stdio.h>

#ifdef __GNUC__
#define PRINTF_CHECK(n) __attribute__ ((format (printf, n, n+1)))
#else
#define PRINTF_CHECK(n)
#endif

class QString;
class Log {
public:
  enum Level { LOG_NONE = 0, LOG_FATAL, LOG_ERROR, LOG_WARNING, LOG_INFO, LOG_DEBUG } level;
  FILE* target;

  Log();
  ~Log();

  static void fatal(const char* fmt, ...) PRINTF_CHECK(1);
  static void error(const char* fmt, ...) PRINTF_CHECK(1);
  static void warn(const char* fmt, ...) PRINTF_CHECK(1);
  static void info(const char* fmt, ...) PRINTF_CHECK(1);
  static void debug(const char* fmt, ...) PRINTF_CHECK(1);

  static void fatal(const QString& str, ...);
  static void error(const QString& str, ...);
  static void warn(const QString& str, ...);
  static void info(const QString& str, ...);
  static void debug(const QString& str, ...);

  static void log(Level level, const char* fmt, ...) PRINTF_CHECK(2);

private:
  static Log* s_log;
};

#undef PRINTF_CHECK

#endif // LOG_HPP
