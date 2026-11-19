#pragma once

#include <stdio.h>

typedef enum LogLevel {
  LOG_LVL_TRACE = 0,
  LOG_LVL_DEBUG,
  LOG_LVL_INFO,
  LOG_LVL_WARN,
  LOG_LVL_ERROR,
  LOG_LVL_OFF
} LogLevel;

/* Уровень на этапе компиляции: поставь LOG_LVL_OFF в release —
   все LOG_* ниже превратятся в пустоту. */
#ifndef FORTH_LOG_COMPILE_LEVEL
#define FORTH_LOG_COMPILE_LEVEL LOG_LVL_TRACE
#endif

void logger_set_level(LogLevel level);
LogLevel logger_get_level(void);
void logger_set_stream(FILE *stream);

void logger_log(LogLevel level, const char *file, int line, const char *func,
                const char *category, const char *fmt, ...);

#define LOG_ENABLED(lvl)                                                       \
  ((lvl) >= FORTH_LOG_COMPILE_LEVEL && (lvl) >= logger_get_level())

#define LOG_AT(lvl, cat, ...)                                                  \
  do {                                                                         \
    if (LOG_ENABLED(lvl))                                                      \
      logger_log((lvl), __FILE__, __LINE__, __func__, (cat), __VA_ARGS__);     \
  } while (0)

#define LOG_TRACE(cat, ...) LOG_AT(LOG_LVL_TRACE, cat, __VA_ARGS__)
#define LOG_DEBUG(cat, ...) LOG_AT(LOG_LVL_DEBUG, cat, __VA_ARGS__)
#define LOG_INFO(cat, ...) LOG_AT(LOG_LVL_INFO, cat, __VA_ARGS__)
#define LOG_WARN(cat, ...) LOG_AT(LOG_LVL_WARN, cat, __VA_ARGS__)
#define LOG_ERROR(cat, ...) LOG_AT(LOG_LVL_ERROR, cat, __VA_ARGS__)
