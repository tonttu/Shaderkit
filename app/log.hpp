#ifndef LOG_HPP
#define LOG_HPP

#include <stdio.h>

#ifdef __GNUC__
#define PRINTF_CHECK __attribute__ ((format (printf, 1, 2)))
#else
#define PRINTF_CHECK
#endif

class QString;
class Log {
public:
  enum Level { LOG_NONE = 0, LOG_ERROR, LOG_WARNING, LOG_INFO, LOG_DEBUG } level;
  FILE* target;

  Log();
  ~Log();

  static void error(const char* fmt, ...) PRINTF_CHECK;
  static void warn(const char* fmt, ...) PRINTF_CHECK;
  static void info(const char* fmt, ...) PRINTF_CHECK;
  static void debug(const char* fmt, ...) PRINTF_CHECK;

  static void error(const QString& str, ...);
  static void warn(const QString& str, ...);
  static void info(const QString& str, ...);
  static void debug(const QString& str, ...);

private:
  static Log* s_log;
};

#undef PRINTF_CHECK

#endif // LOG_HPP
