#include "util.h"
#include <cstdint>
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

void read_workload(std::string path, std::vector<Key> &keys, bool is_lookup) {
  std::ifstream infile(path);
  std::string line;
  // int cnt = 0;
  while (std::getline(infile, line)) {
    std::istringstream iss;
    iss.str(line);
    std::string op, table_name, usr_key;
    iss >> op >> table_name >> usr_key;
    Key temp;
    temp.val = str_t_uint64(usr_key.substr(4));
    if (op == "UPDATE")
      temp.type = true;
    else
      temp.type = false;
    keys.emplace_back(temp);
  }
  if (!is_lookup)
    std::sort(keys.begin(), keys.end(), cmp);
}
void print_table(std::string path, std::vector<uint64_t *> table,
                 uint64_t col_num) {
  std::ofstream outfile(path);
  for (uint64_t i = 0; i < table.size(); i++) {
    for (uint64_t j = 0; j < col_num; j++)
      outfile << table[i][j] << ",";
    outfile << std::endl;
  }
  outfile.close();
}

void read_file(std::string path, std::vector<uint64_t> &data,
               int64_t data_num) {
  std::ifstream infile(path);
  uint64_t val = 0;
  if (data_num == -1) {
    while (infile >> val)
      data.push_back(val);
    return;
  }
  for (int64_t i = 0; i < data_num; i++) {
    infile >> val;
    data.push_back(val);
  }
}
} // namespace util