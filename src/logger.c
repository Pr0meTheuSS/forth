#include "logger.h"

#include <stdarg.h>
#include <time.h>

static LogLevel g_level = LOG_LVL_WARN;
static FILE *g_stream = NULL;

void logger_set_level(LogLevel level) { g_level = level; }
LogLevel logger_get_level(void) { return g_level; }
void logger_set_stream(FILE *stream) { g_stream = stream; }

static const char *level_name(LogLevel l) {
  switch (l) {
  case LOG_LVL_TRACE:
    return "TRACE";
  case LOG_LVL_DEBUG:
    return "DEBUG";
  case LOG_LVL_INFO:
    return "INFO";
  case LOG_LVL_WARN:
    return "WARN";
  case LOG_LVL_ERROR:
    return "ERROR";
  default:
    return "?";
  }
}

void logger_log(LogLevel level, const char *file, int line, const char *func,
                const char *category, const char *fmt, ...) {
  FILE *out = g_stream ? g_stream : stderr;

  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  struct tm tm;
  localtime_r(&ts.tv_sec, &tm);
  char timebuf[16];
  strftime(timebuf, sizeof(timebuf), "%H:%M:%S", &tm);

  fprintf(out, "%s.%03ld %-5s [%s] %s:%d %s(): ", timebuf,
          ts.tv_nsec / 1000000L, level_name(level), category, file, line, func);

  va_list ap;
  va_start(ap, fmt);
  vfprintf(out, fmt, ap);
  va_end(ap);

  fputc('\n', out);
  fflush(out);
}
