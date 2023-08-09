//
// Created by Homing So on 2023/8/7.
//

#include "utility/filesystem.h"

int main(int argc, char *argv[]) {
  (void) argc, (void) argv;

  auto file = utility::filesystem::File("./test", "rb");
  auto map = utility::filesystem::MMap(&file);
  auto map_lock = utility::filesystem::MLock();
  map_lock.init(map.addr);
  map_lock.grow_to(map.size);

  return 0;
}
