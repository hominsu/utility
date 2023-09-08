//
// Created by Homing So on 2023/8/11.
//

#include <cstdio>

#include <chrono>
#include <limits>
#include <vector>

#include "utility/des.h"

int main(int argc, char *argv[]) {
  (void) argc, (void) argv;

  auto size = 1024 * 1024 * 16;

  std::vector<uint64_t> src;
  src.resize(size);
  std::fill(src.begin(), src.end(), std::numeric_limits<uint64_t>::max());

  std::vector<uint64_t> dst;
  dst.resize(size);

  Des des("12345678", 8);

  std::chrono::time_point<std::chrono::system_clock> start, end;
  double usage;

  start = std::chrono::system_clock::now();
  des.encrypt(src.data(), src.size() * sizeof(uint64_t), dst.data());
  end = std::chrono::system_clock::now();

  usage = (end - start).count() / 1000.0;

  fprintf(stdout, "Encrypt:\n\t%fms\n", usage);
  fprintf(stdout, "\t%fMB/s\n",
          (static_cast<double>(sizeof(uint64_t)) * size / 1024 / 1024) / (usage / 1000)
  );

  start = std::chrono::system_clock::now();
  des.decrypt(dst.data(), dst.size() * sizeof(uint64_t), src.data());
  end = std::chrono::system_clock::now();

  usage = (end - start).count() / 1000.0;
  fprintf(stdout, "Decrypt:\n\t%fms\n", usage);
  fprintf(stdout, "\t%fMB/s\n",
          (static_cast<double>(sizeof(uint64_t)) * size / 1024 / 1024) / (usage / 1000)
  );

  return 0;
}
