/**
 * Copyright 2010,2011 Riku Palomäki.
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

#include "log.hpp"

#include <cassert>
#include <cstdarg>
#include <sys/types.h>
#include <sys/stat.h>
#include <cstring>
#include <errno.h>

#include <QTime>

namespace {
  void writelog(Log::Level l, FILE* target, const char* fmt, va_list ap) {
    static QByteArray last;
    static int count = 0;
    static const char* prefixes[] = {"          ",
                                     " Fatal    ",
                                     " Error    ",
                                     " Warning  ",
                                     " Info     ",
                                     " Debug    "};
    QByteArray ts = QTime::currentTime().toString("HH:mm:ss.zzz").toAscii();

    QByteArray str(prefixes[l]);
    int p = str.size();
    str.resize(p + 513);
    p += vsnprintf(str.data() + p, 512, fmt, ap);

    str.resize(p);
    while (p > 0 && (str[p-1] == '\n' || str[p-1] == '\r'))
      str.resize(--p);

    if (str == last) {
      ++count;
    } else {
      if (count > 0) {
        fprintf(target, "                      (message repeated %d times)\n", count);
        count = 0;
      }
      fprintf(target, "%s%s\n", ts.data(), str.data());
      last = str;
    }
  }
}

Log* Log::s_log = 0;

Log::Log() : level(LOG_INFO), target(stderr) {
  if (!s_log)
    s_log = this;
}

Log::~Log() {
  if (s_log) {
    info("Bye");
  }
  if (s_log == this)
    s_log = 0;

  if (target && fileno(target) > 2) {
    fclose(target);
  }
}

#define LOG_IMPL(type) \
  assert(s_log); \
  if(s_log->level < type) return; \
  va_list ap; \
  va_start(ap, fmt); \
  writelog(type, s_log->target, fmt, ap); \
  va_end(ap)

#define LOG_IMPL2(type) \
  assert(s_log); \
  if(s_log->level < type) return; \
  va_list ap; \
  va_start(ap, str); \
  writelog(type, s_log->target, str.toUtf8().data(), ap); \
  va_end(ap)

void Log::fatal(const char* fmt, ...) { LOG_IMPL(LOG_FATAL); abort(); }
void Log::error(const char* fmt, ...) { LOG_IMPL(LOG_ERROR); }
void Log::warn(const char* fmt, ...) { LOG_IMPL(LOG_WARNING); }
void Log::info(const char* fmt, ...) { LOG_IMPL(LOG_INFO); }
void Log::debug(const char* fmt, ...) { LOG_IMPL(LOG_DEBUG); }

void Log::fatal(const QString& str, ...) { LOG_IMPL2(LOG_FATAL); abort(); }
void Log::error(const QString& str, ...) { LOG_IMPL2(LOG_ERROR); }
void Log::warn(const QString& str, ...) { LOG_IMPL2(LOG_WARNING); }
void Log::info(const QString& str, ...) { LOG_IMPL2(LOG_INFO); }
void Log::debug(const QString& str, ...) { LOG_IMPL2(LOG_DEBUG); }

void Log::log(Level level, const char* fmt, ...) {
  assert(s_log);
  va_list ap;
  va_start(ap, fmt);

  if (level < LOG_NONE || level > LOG_DEBUG) {
    level = LOG_ERROR;
    writelog(level, s_log->target, "Invalid log level", ap);
  }
  if (s_log->level >= level)
    writelog(level, s_log->target, fmt, ap);
  va_end(ap);
  if (level == LOG_FATAL) abort();
}

#undef LOG_IMPL2
#undef LOG_IMPL
