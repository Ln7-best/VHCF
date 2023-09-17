#pragma once
#include <algorithm>
#include <cstring>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace util {
class Key {
public:
  uint64_t val;
  bool type; // 1 for tombstone, 0 for normal key
};
bool cmp(Key &x, Key &y);
uint64_t str_t_uint64(std::string str);
void read_workload(std::string path, std::vector<Key> &keys, bool is_insert);
} // namespace util