//
// Created by Homing So on 2023/8/7.
//

#include "utility/logger.h"
#include "utility/timestamp.h"

namespace utility::logger {

#ifndef NDEBUG
Level log_level = DEBUG;
#else
Level log_level = INFO;
#endif

FILE* log_file = stdout;

void set_log_level(Level _level) {
  UTILITY_ASSERT((_level >= TRACE && _level <= FATAL) && "log level out of range");
  log_level = _level;
}

void set_log_file(FILE *_file) {
  UTILITY_ASSERT(_file != nullptr && "file pointer should not be nullptr");
  log_file = _file;
}

void timestamp(char *_buf, std::size_t _size) {
  clock::to_string(_buf, _size);
}

} // namespace utility::logger
