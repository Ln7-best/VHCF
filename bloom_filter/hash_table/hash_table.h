#pragma once
#include <iostream>
// #include<stdlib.h>
#include "murmur3.h"
#include <cstring>
#include <ctime>
#include <random>

namespace ht {
class hash_table {
public:
  hash_table();
  ~hash_table() {}
  void init(uint64_t values_num, uint64_t block_size);
  void insert(uint64_t key);
  bool lookup(uint64_t key);

private:
  uint64_t *values;
  uint64_t values_num;
  uint64_t block_size;
};
hash_table::hash_table() {
  values = nullptr;
  values_num = 0;
  block_size = 0;
}
void hash_table::init(uint64_t values_num, uint64_t block_size) {
  values = new uint64_t[values_num + 8];
  memset(values, 0, (values_num + 8) * 8);
  this->values_num = values_num;
  this->block_size = block_size;
}
// the last elemnt in the block is the latest visited
void hash_table::insert(uint64_t key) {
  uint32_t pos;
  MurmurHash3_x86_32(&key, 8, 0x1234ABCD, &pos);
  pos %= values_num;
  pos /= block_size;
  pos *= block_size;
  for (int i = 1; i < block_size; i++)
    values[pos + i - 1] = values[pos + i];
  values[pos + block_size - 1] = key;
}
bool hash_table::lookup(uint64_t key) {
  uint32_t pos;
  MurmurHash3_x86_32(&key, 8, 0x1234ABCD, &pos);
  pos %= values_num;
  pos /= block_size;
  pos *= block_size;
  // bool flag = 0;
  int inner_pos = block_size - 1;
  for (inner_pos; inner_pos >= 0; inner_pos--) {
    if (values[pos + inner_pos] == key)
      break;
    if (values[pos + inner_pos] == 0)
      return false;
  }
  if (inner_pos < 0)
    return false;
  uint64_t temp_val = values[pos + inner_pos];
  for (int i = inner_pos + 1; i < block_size; i++) {
    values[pos + i - 1] = values[pos + i];
  }
  values[pos + block_size - 1] = temp_val;
  return true;
}
} // namespace ht