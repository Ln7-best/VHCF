#include "cuckoofilter.h"
#include "utils.h"
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <time.h>
#include <unordered_set>
#include <windows.h>
int main() {
  // cuckoofilter::CuckooFilter<size_t, 10> cf(498073);
  // std::cout << "reading data complete!" << std::endl;
  // std::cout << negative_set.size() << std::endl;
  // std::cout << op_seq.size() << std::endl;
  std::vector<util::Key> insert_keys;
  uint64_t loop_num = 10;
  std::vector<uint64_t> negative_set;
  std::vector<util::Key> op_seq;
  // util::read_workload("/usr/infocom24/operation_insert_ycsb.txt.txt",
  //                     insert_keys, true);
  // std::cout << insert_keys.size();
  util::read_file("C:\\study\\papercode\\infocom24\\mawi_raw.txt", negative_set,
                  3984588);
  uint64_t count = 0;
  uint64_t fail_count = 0;
  uint64_t fp_count = 0;
  std::vector<util::Key> lookup_keys;
  std::vector<uint64_t> table;
  std::vector<uint64_t> queries;
  cuckoofilter::CuckooFilter<size_t, 16> cf(3984588);
  DWORD time_start, time_end;
  time_start = GetTickCount64();
  for (uint64_t i = 0; i < 3984588; i++) {
    cf.Add(negative_set[i]);
  }
  time_end = GetTickCount64();
  std::cout << time_end - time_start << std::endl;
  time_start = GetTickCount64();
  for (uint64_t i = 0; i < 3984588; i++) {
    cf.Delete(negative_set[i]);
  }
  time_end = GetTickCount64();
  std::cout << time_end - time_start << std::endl;
}