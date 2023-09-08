//
// Created by Homing So on 2023/8/10.
//

#ifndef UTILITY_INCLUDE_UTILITY_DES_H_
#define UTILITY_INCLUDE_UTILITY_DES_H_

#include <cstddef>
#include <cstdint>

class Des {
 public:
  explicit Des(const char *key, size_t len);

  void encrypt(void *src, size_t len, void *dst);
  void decrypt(void *src, size_t len, void *dst);

 private:
  static uint32_t key_left_shift(uint32_t &k, const uint8_t &shift_num);
  static uint32_t round_func(const uint32_t &r, const uint64_t &k);

  void gen_sub_keys();
  uint64_t crypt(uint64_t &data, bool is_encrypt = true);

 private:
  uint64_t key_{};
  uint64_t sub_keys_[16]{};
};

#endif //UTILITY_INCLUDE_UTILITY_DES_H_
