//
// Created by Homing So on 2023/8/10.
//

#include <cstring>

#include "utility/des.h"
#include "utility/utility.h"

namespace {

// Initial Permutation
constexpr uint8_t kIP[] = {
    57, 49, 41, 33, 25, 17, 9, 1,
    59, 51, 43, 35, 27, 19, 11, 3,
    61, 53, 45, 37, 29, 21, 13, 5,
    63, 55, 47, 39, 31, 23, 15, 7,
    56, 48, 40, 32, 24, 16, 8, 0,
    58, 50, 42, 34, 26, 18, 10, 2,
    60, 52, 44, 36, 28, 20, 12, 4,
    62, 54, 46, 38, 30, 22, 14, 6
};

// Inverse Initial Permutation
constexpr uint8_t kIP_1[] = {
    39, 7, 47, 15, 55, 23, 63, 31,
    38, 6, 46, 14, 54, 22, 62, 30,
    37, 5, 45, 13, 53, 21, 61, 29,
    36, 4, 44, 12, 52, 20, 60, 28,
    35, 3, 43, 11, 51, 19, 59, 27,
    34, 2, 42, 10, 50, 18, 58, 26,
    33, 1, 41, 9, 49, 17, 57, 25,
    32, 0, 40, 8, 48, 16, 56, 24
};

// Permuted Choice 1, the 64-bit key is compressed to 56 bits
constexpr uint8_t kPC_1[] = {
    56, 48, 40, 32, 24, 16, 8,
    0, 57, 49, 41, 33, 25, 17,
    9, 1, 58, 50, 42, 34, 26,
    18, 10, 2, 59, 51, 43, 35,
    62, 54, 46, 38, 30, 22, 14,
    6, 61, 53, 45, 37, 29, 21,
    13, 5, 60, 52, 44, 36, 28,
    20, 12, 4, 27, 19, 11, 3
};

// Permuted Choice 2, the 56-bit key is compressed to 48 bits
constexpr uint8_t kPC_2[] = {
    13, 16, 10, 23, 0, 4,
    2, 27, 14, 5, 20, 9,
    22, 18, 11, 3, 25, 7,
    15, 6, 26, 19, 12, 1,
    40, 51, 30, 36, 46, 54,
    29, 39, 50, 44, 32, 47,
    43, 48, 38, 55, 33, 52,
    45, 41, 49, 35, 28, 31
};

// Number of left circular shift
constexpr uint8_t kShiftBits[] = {1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1};

// Extended Permutation，32-bit data extended to 48 bits
constexpr uint8_t kE[] = {
    31, 0, 1, 2, 3, 4,
    3, 4, 5, 6, 7, 8,
    7, 8, 9, 10, 11, 12,
    11, 12, 13, 14, 15, 16,
    15, 16, 17, 18, 19, 20,
    19, 20, 21, 22, 23, 24,
    23, 24, 25, 26, 27, 28,
    27, 28, 29, 30, 31, 0
};

// Substitution Boxes, eight S-boxes which map 6 to 4 bits
constexpr uint8_t kSBox[8][4][16] = {
    {
        {14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7},
        {0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8},
        {4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0},
        {15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13}
    },
    {
        {15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10},
        {3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5},
        {0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15},
        {13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9}
    },
    {
        {10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8},
        {13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1},
        {13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7},
        {1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12}
    },
    {
        {7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15},
        {13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9},
        {10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4},
        {3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14}
    },
    {
        {2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9},
        {14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6},
        {4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14},
        {11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3}
    },
    {
        {12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11},
        {10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8},
        {9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6},
        {4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13}
    },
    {
        {4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1},
        {13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6},
        {1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2},
        {6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12}
    },
    {
        {13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7},
        {1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2},
        {7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8},
        {2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11}
    }
};

// 32-bit Perm P
constexpr uint8_t kP[] = {
    15, 6, 19, 20,
    28, 11, 27, 16,
    0, 14, 22, 25,
    4, 17, 30, 9,
    1, 7, 23, 13,
    31, 26, 2, 8,
    18, 12, 29, 5,
    21, 10, 3, 24
};

} // namespace

uint32_t Des::key_left_shift(uint32_t &k, const uint8_t &shift_num) {
  return (k << shift_num | k >> (28 - shift_num)) & 0xFFFFFFF;
}

uint32_t Des::round_func(const uint32_t &r, const uint64_t &k) {
  uint64_t expend_e = 0x0;
  for (uint32_t value : kE) {
    expend_e <<= 1;
    expend_e |= r >> value & 0x1;
  }
  expend_e ^= k;

  uint32_t tmp = 0x0;
  for (size_t i = 0; i < 8; ++i) {
    tmp <<= 4;
    tmp |= kSBox[i][(expend_e & (0x3 << i)) >> i][(expend_e & (0x60 << i)) >> (i + 2)];
  }

  uint32_t res = 0x0;
  for (uint32_t value : kP) {
    res <<= 1;
    res |= tmp >> value & 0x1;
  }

  return res;
}

Des::Des(const char *key, size_t len) {
  char tmp[8]{0};
  memcpy(tmp, key, len > 8 ? 8 : len);
  key_ = *reinterpret_cast<uint64_t *>(tmp);

  gen_sub_keys();
}

void Des::gen_sub_keys() {
  uint64_t key_56 = 0x0;
  for (uint32_t value : kPC_1) {
    key_56 <<= 1;
    key_56 |= key_ >> value & 0x1;
  }

  uint32_t left_key = key_56 >> 28;
  uint32_t right_key = key_56 & 0xFFFFFFF;

  uint64_t key_48 = 0x0;

  for (size_t i = 0; i < 16; ++i) {
    left_key = key_left_shift(left_key, kShiftBits[i]);
    right_key = key_left_shift(right_key, kShiftBits[i]);

    key_56 = left_key;
    key_56 <<= 28;
    key_56 |= right_key;

    for (uint32_t value : kPC_2) {
      key_48 <<= 1;
      key_48 |= key_56 >> value & 0x1;
    }

    sub_keys_[i] = key_48;
  }
}

uint64_t Des::crypt(uint64_t &data, bool is_encrypt) {
  uint64_t tmp = 0x0;

  for (uint64_t value : kIP) {
    tmp <<= 1;
    tmp |= data >> value & 0x1;
  }

  uint32_t left = tmp >> 32;
  uint32_t right = tmp & 0xFFFFFFFF;

  if (is_encrypt) {
    for (auto &sub_key : sub_keys_) {
      uint32_t t = right;
      right = left ^ round_func(right, sub_key);
      left = t;
    }
  } else {
    for (size_t i = 0; i < 16; ++i) {
      uint32_t t = right;
      right = left ^ round_func(right, sub_keys_[15 - i]);
      left = t;
    }
  }

  tmp = right;
  tmp <<= 32;
  tmp |= left;

  uint64_t ret = 0x0;
  for (uint64_t value : kIP_1) {
    ret <<= 1;
    ret |= tmp >> value & 0x1;
  }

  return ret;
}

void Des::encrypt(void *src, size_t len, void *dst) {
  UTILITY_ASSERT(len % sizeof(uint64_t) == 0);
  auto src_ptr = reinterpret_cast<uint64_t *>(src);
  auto dst_ptr = reinterpret_cast<uint64_t *>(dst);
  for (size_t i = 0; i < len / sizeof(uint64_t); ++i) {
    dst_ptr[i] = crypt(src_ptr[i], true);
  }
}

void Des::decrypt(void *src, size_t len, void *dst) {
  UTILITY_ASSERT(len % sizeof(uint64_t) == 0);
  auto src_ptr = reinterpret_cast<uint64_t *>(src);
  auto dst_ptr = reinterpret_cast<uint64_t *>(dst);
  for (size_t i = 0; i < len / sizeof(uint64_t); ++i) {
    dst_ptr[i] = crypt(src_ptr[i], false);
  }
}
