#include "utils.h"
namespace util {
bool cmp(Key &x, Key &y) {
  if (x.val != y.val)
    return x.val < y.val;
  return x.type > y.type; // tombstone must be in front of the normal key
}

uint64_t str_t_uint64(std::string str) {
  uint64_t ans = 0;
  for (int i = 0; i < str.length(); i++) {
    ans *= 10;
    ans += str[i] - '0';
  }
  return ans;
}

void read_workload(std::string path, std::vector<Key> &keys, bool is_insert) {
  std::ifstream infile(path);
  std::string line;
  // int cnt = 0;
  while (std::getline(infile, line)) {
    //  cnt++;
    //  if(cnt==999)
    //      cnt = 999;
    std::istringstream iss;
    iss.str(line);
    std::string op, table_name, usr_key;
    iss >> op >> table_name >> usr_key;
    Key temp;
    temp.val = str_t_uint64(usr_key.substr(4));
    if (op == "UPDATE")
      temp.type = 1;
    else
      temp.type = 0;
    keys.emplace_back(temp);
  }
  if (is_insert)
    std::sort(keys.begin(), keys.end(), cmp);
}
} // namespace util