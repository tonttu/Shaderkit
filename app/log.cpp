#include "log.hpp"

#include <cassert>
#include <cstdarg>
#include <sys/types.h>
#include <sys/stat.h>
#include <cstring>
#include <errno.h>

#include <QTime>

namespace {
  void log(Log::Level l, FILE* target, const char* fmt, va_list ap) {
    static const char* prefixes[] = {"          ",
                                     " Error    ",
                                     " Warning  ",
                                     " Info     ",
                                     " Debug    "};
    QByteArray ts = QTime::currentTime().toString("HH:mm:ss.zzz").toAscii();
    fwrite(ts.data(), ts.length(), 1, target);
    fwrite(prefixes[l], 10, 1, target);
    vfprintf(target, fmt, ap);
    fputc('\n', target);
  }
}

Log* Log::s_log = 0;

Log::Log() : level(LOG_INFO), target(stderr) {
  if (!s_log) {
    s_log = this;
    info("Starting");
  }
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
  log(type, s_log->target, fmt, ap); \
  va_end(ap);

#define LOG_IMPL2(type) \
  assert(s_log); \
  if(s_log->level < type) return; \
  va_list ap; \
  va_start(ap, str); \
  log(type, s_log->target, str.toUtf8().data(), ap); \
  va_end(ap);

void Log::error(const char* fmt, ...) { LOG_IMPL(LOG_ERROR) }
void Log::warn(const char* fmt, ...) { LOG_IMPL(LOG_WARNING) }
void Log::info(const char* fmt, ...) { LOG_IMPL(LOG_INFO) }
void Log::debug(const char* fmt, ...) { LOG_IMPL(LOG_DEBUG) }

void Log::error(const QString& str, ...) { LOG_IMPL2(LOG_ERROR) }
void Log::warn(const QString& str, ...) { LOG_IMPL2(LOG_WARNING) }
void Log::info(const QString& str, ...) { LOG_IMPL2(LOG_INFO) }
void Log::debug(const QString& str, ...) { LOG_IMPL2(LOG_DEBUG) }

#undef LOG_IMPL2
#undef LOG_IMPL
