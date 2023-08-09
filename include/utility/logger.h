//
// Created by Homing So on 2023/8/7.
//

#ifndef UTILITY_INCLUDE_UTILITY_LOGGER_H_
#define UTILITY_INCLUDE_UTILITY_LOGGER_H_

#include <cstdio>

#include "utility.h"

#define LOG_LEVEL(UTILITY) \
  UTILITY(TRACE)           \
  UTILITY(DEBUG)           \
  UTILITY(INFO)            \
  UTILITY(WARN)            \
  UTILITY(ERROR)           \
  UTILITY(FATAL)           \
  //

namespace utility::logger {

enum Level {
#define LOG_NAME(_name_) _name_,
  LOG_LEVEL(LOG_NAME)
#undef LOG_NAME
};

extern Level log_level;
extern FILE *log_file;

void set_log_level(Level level);
void set_log_file(FILE *file);

void timestamp(char *buf, size_t size);

inline const char *level_str(Level level) {
  const static char *level_str_table[] = {
#define LOG_STR(_name_) #_name_,
      LOG_LEVEL(LOG_STR)
#undef LOG_STR
  };

  UTILITY_ASSERT(level >= 0 && level < UTILITY_LENGTH(level_str_table));
  return level_str_table[level];
}

} // namespace utility::logger

#undef LOG_LEVEL

#define LOG_BASE(_log_file_, _level_, _file_, _line_, _abort_, _formatter_, ...) \
  do {                                                                           \
    char buf[32]{0};                                                             \
    utility::logger::timestamp(buf, sizeof(buf));                                \
    int err = fprintf((_log_file_), "[%s] [%s] "#_formatter_" - %s:%d\n", utility::logger::level_str((_level_)), buf, ##__VA_ARGS__, strrchr((_file_), '/') + 1, (_line_)); \
    if (err == -1) {                                                             \
      fprintf(stderr, "log failed");                                             \
    }                                                                            \
    if ((_abort_)) {                                                             \
      abort();                                                                   \
    }                                                                            \
  } while (0)                                                                    \
  //

#define LOG_SYS(_log_file_, _file_, _line_, _abort_, _formatter_, ...) \
  do {                                                                 \
    char buf[32]{0};                                                   \
    utility::logger::timestamp(buf, sizeof(buf));                      \
    fprintf((_log_file_), "[%s] [%s] "#_formatter_": %s - %s:%d\n", (_abort_) ? "SYSFA" : "SYSER", buf, ##__VA_ARGS__, strerror(errno), strrchr((_file_), '/') + 1, (_line_)); \
    if ((_abort_)) {                                                   \
      abort();                                                         \
    }                                                                  \
  } while (0)                                                          \
  //

#define TRACE(_formatter_, ...) \
  do {                          \
    if (utility::logger::log_level <= utility::logger::Level::TRACE) { \
      LOG_BASE(utility::logger::log_file, utility::logger::Level::TRACE, __FILE__, __LINE__, false, _formatter_, ##__VA_ARGS__); \
    }                           \
  } while (0)                   \
  //

#define DEBUG(_formatter_, ...) \
  do {                          \
    if (utility::logger::log_level <= utility::logger::Level::DEBUG) { \
      LOG_BASE(utility::logger::log_file, utility::logger::Level::DEBUG, __FILE__, __LINE__, false, _formatter_, ##__VA_ARGS__); \
    }                           \
  } while (0)                   \
  //

#define INFO(_formatter_, ...) \
  do {                         \
    if (utility::logger::log_level <= utility::logger::Level::INFO) { \
      LOG_BASE(utility::logger::log_file, utility::logger::Level::INFO, __FILE__, __LINE__, false, _formatter_, ##__VA_ARGS__); \
    }                          \
  } while (0)                  \
  //

#define WARN(_formatter_, ...) \
  do {                         \
    if (utility::logger::log_level <= utility::logger::Level::WARN) { \
      LOG_BASE(utility::logger::log_file, utility::logger::Level::WARN, __FILE__, __LINE__, false, _formatter_, ##__VA_ARGS__); \
    }                          \
  } while (0)                  \
  //

#define ERROR(_formatter_, ...) LOG_BASE(utility::logger::log_file, utility::logger::Level::ERROR, __FILE__, __LINE__, false, _formatter_, ##__VA_ARGS__)

#define FATAL(_formatter_, ...) LOG_BASE(utility::logger::log_file, utility::logger::Level::FATAL, __FILE__, __LINE__, true, _formatter_, ##__VA_ARGS__)

#define SYSERR(_formatter_, ...) LOG_SYS(utility::logger::log_file, __FILE__, __LINE__, false, _formatter_, ##__VA_ARGS__)

#define SYSFATAL(_formatter_, ...) LOG_SYS(utility::logger::log_file, __FILE__, __LINE__, true, _formatter_, ##__VA_ARGS__)

#endif //UTILITY_INCLUDE_UTILITY_LOGGER_H_
