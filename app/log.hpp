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
