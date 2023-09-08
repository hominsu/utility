//
// Created by Homing So on 2023/8/12.
//

#include <cstdio>

#include "utility/des.h"

int main(int argc, char *argv[]) {
  (void) argc, (void) argv;

  char plain_text[8]{1, 2, 3, 4, 5, 6, 7, 8};
  char cipher_text[8]{};

  fprintf(stdout, "plain_text: [");
  for (auto &c : plain_text) {
    fprintf(stdout, "%d", c);
  }
  fprintf(stdout, "]\n");

  Des des("12345678", 8);
  des.encrypt(plain_text, 8, cipher_text);
  fprintf(stdout, "cipher_text: [");
  for (auto &c : cipher_text) {
    fprintf(stdout, "%d", c);
  }
  fprintf(stdout, "]\n");

  des.decrypt(cipher_text, 8, plain_text);
  fprintf(stdout, "plain_text: [");
  for (auto &c : plain_text) {
    fprintf(stdout, "%d", c);
  }
  fprintf(stdout, "]\n");

  return 0;
}
