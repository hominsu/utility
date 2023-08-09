//
// Created by Homing So on 2023/8/7.
//

#ifndef UTILITY_INCLUDE_UTILITY_FILESYSTEM_H_
#define UTILITY_INCLUDE_UTILITY_FILESYSTEM_H_

#include <cstdarg>
#include <cstddef>
#include <cstdio>
#include <cstring>

#include <exception>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

#ifdef __has_include
#if __has_include(<unistd.h>)
#include <unistd.h>
#if defined(_POSIX_MAPPED_FILES)
#include <sys/mman.h>
#endif
#if defined(_POSIX_MEMLOCK_RANGE)
#include <sys/resource.h>
#endif
#endif
#endif

#include "utility/logger.h"
#include "utility/string.h"
#include "utility/utility.h"

namespace utility::filesystem {

struct File {
  FILE *fp;
  size_t size;

  File(const char *filename, const char *mode) {
    fp = std::fopen(filename, mode);
    if (fp == nullptr) {
      throw std::runtime_error(utility::string::format("failed to open %s: %s", filename, strerror(errno)));
    }
    seek(0, SEEK_END);
    size = tell();
    seek(0, SEEK_SET);
  }

  [[nodiscard]] size_t tell() const {
#if UTILITY_WINDOWS
    auto ret = _ftelli64(fp);
#else
    auto ret = ftell(fp);
#endif
    return static_cast<size_t>(ret);
  }

  void seek(size_t offset, int whence) const {
#if UTILITY_WINDOWS
    int ret = _fseeki64(fp, (__int64) offset, whence);
#else
    int ret = std::fseek(fp, (long) offset, whence);
#endif
    (void) ret;
    UTILITY_ASSERT(ret == 0);
  }
};

struct MMap {
  void *addr;
  size_t size;

  MMap(const MMap &) = delete;

#ifdef _POSIX_MAPPED_FILES
  static constexpr bool SUPPORTED = true;

  explicit MMap(File *f, size_t prefetch = std::numeric_limits<size_t>::max(), bool numa = false)
      : size(f->size) {
    int fd = fileno(f->fp);
    int flags = MAP_SHARED;
    if (numa) { prefetch = 0; }
#if UTILITY_LINUX
    if (prefetch) { flags |= MAP_POPULATE; }
#endif
    addr = ::mmap(nullptr, f->size, PROT_READ, flags, fd, 0);
    if (addr == MAP_FAILED) {
      throw std::runtime_error(utility::string::format("mmap failed: %s", strerror(errno)));
    }

    if (prefetch > 0) {
      // advise the kernel to preload the mapped memory
      if (madvise(addr, std::min(f->size, prefetch), MADV_WILLNEED)) {
        WARN("madvise(.., MADV_WILLNEED) failed: %s", strerror(errno));
      }
    }
    if (numa) {
      // advise the kernel not to use readahead (because the next page might not belong on the same node)
      if (madvise(addr, std::min(f->size, prefetch), MADV_RANDOM)) {
        WARN("madvise(.., MADV_RANDOM) failed: %s", strerror(errno));
      }
    }
  }

  ~MMap() {
    ::munmap(addr, size);
  }
#else
  static constexpr bool SUPPORTED = false;

  mmap(file *, bool prefetch = true, bool numa = false) {
    (void) prefetch;
    (void) numa;

    throw std::runtime_error(std::string("mmap not supported"));
  }
#endif
};

struct MLock {
  void *addr = nullptr;
  size_t size = 0;
  bool failed_already = false;

  MLock() = default;
  MLock(const MLock &) = delete;

  ~MLock() {
    if (size) {}
  }

  void init(void *ptr) {
    UTILITY_ASSERT(addr == nullptr && size == 0);
    addr = ptr;
  }

  void grow_to(size_t target_size) {
    UTILITY_ASSERT(addr != nullptr);
    if (failed_already) { return; }
    auto p_size = page_size();
    target_size = (target_size + p_size - 1) & ~(p_size - 1);
    if (target_size > size) {
      if (raw_lock(static_cast<uint8_t *>(addr) + size, target_size - size)) {
        size = target_size;
      } else {
        failed_already = true;
      }
    }
  }

#ifdef _POSIX_MEMLOCK_RANGE
  static constexpr bool SUPPORTED = true;

  size_t page_size() {
    return static_cast<size_t>(::sysconf(_SC_PAGESIZE));
  }

#if UTILITY_APPLE
#define MLOCK_SUGGESTION \
            "Try increasing the sysctl values 'vm.user_wire_limit' and 'vm.global_user_wire_limit' and/or " \
            "decreasing 'vm.global_no_user_wire_amount'.  Also try increasing RLIMIT_MLOCK (ulimit -l).\n"
#else
#define MLOCK_SUGGESTION \
            "Try increasing RLIMIT_MLOCK ('ulimit -l' as root).\n"
#endif

  bool raw_lock(const void *ptr, size_t len) {
    if (!::mlock(ptr, len)) { return true; }
    else {
      char *errmsg = strerror(errno);
      bool suggest = (errno == ENOMEM);

      rlimit lock_limit{};
      if (suggest && getrlimit(RLIMIT_MEMLOCK, &lock_limit)) { suggest = false; }
      if (suggest && (lock_limit.rlim_max > lock_limit.rlim_cur + len)) { suggest = false; }

      WARN("failed to mlock %zu-byte buffer (after previously locking %zu bytes): %s\n%s",
           len,
           size,
           errmsg,
           suggest ? MLOCK_SUGGESTION : "");

      return false;
    }
  }

#undef MLOCK_SUGGESTION

  void raw_unlock(void *ptr, size_t len) {
    if (::munlock(ptr, len)) {
      WARN("failed to munlock buffer: %s", strerror(errno));
    }
  }

#else
  static constexpr bool SUPPORTED = false;

  size_t page_size() {
    return static_cast<size_t>(65536);
  }

  bool raw_lock(const void *ptr, size_t len) {
    (void) ptr, (void) len;
    fprintf(stderr, "warning: mlock not supported on this system\n");
    return false;
  }

  void raw_unlock(const void *ptr, size_t len) {
    (void) ptr, (void) len;
  }
#endif
};

} // namespace utility::filesystem

#endif //UTILITY_INCLUDE_UTILITY_FILESYSTEM_H_
