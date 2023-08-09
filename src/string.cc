//
// Created by Homing So on 2023/8/7.
//

#include <cassert>
#include <cstdarg>
#include <cstdio>

#include <vector>

#include "utility/string.h"
#include "utility/utility.h"

namespace utility::string {

std::string format(const char *format, ...) {
  va_list ap, ap_cp;
  va_start(ap, format);
  va_copy(ap_cp, ap);
  int size = vsnprintf(nullptr, 0, format, ap);
  UTILITY_ASSERT(size >= 0 && "out of range");
  ::std::vector<char> buf(size + 1);
  int size_cp = vsnprintf(buf.data(), size + 1, format, ap_cp);
  (void) size_cp;
  UTILITY_ASSERT(size_cp == size);
  va_end(ap_cp);
  va_end(ap);
  return {buf.data(), static_cast<size_t>(size)};
}

} // namespace utility::string
