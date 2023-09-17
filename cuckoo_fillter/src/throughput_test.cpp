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
  uint64_t loop_num = 1;
  std::vector<uint64_t> negative_set;
  std::vector<util::Key> op_seq;
  util::read_workload(
      "C:\\study\\papercode\\infocom24\\insert_opreation_uniform.txt",
      insert_keys);
  util::read_file("C:\\study\\papercode\\infocom24\\caida_raw.txt",
                  negative_set, 7000000);
  util::read_workload(
      "C:\\study\\papercode\\infocom24\\operation_zipfseq_theta_0.99.txt",
      op_seq, true);
  uint64_t count = 0;
  uint64_t fail_count = 0;
  uint64_t fp_count = 0;
  std::vector<util::Key> lookup_keys;
  std::vector<uint64_t> table;
  std::vector<uint64_t> queries;
  cuckoofilter::CuckooFilter<size_t, 12> cf(498073);
  for (uint64_t i = 0; i < 498073; i++) {
    if (cf.Add(insert_keys[i].val) != cuckoofilter::Ok) {
      std::cout << "over flow!";
    }
  }
  DWORD time_start, time_end;
  for (uint64_t i = 0; i < op_seq.size(); i++)
    queries.push_back(negative_set[op_seq[i].val]);
  time_start = GetTickCount64();
  for (uint64_t k = 0; k < loop_num; k++) {
    for (uint64_t i = 0; i < op_seq.size(); i++) {
      count++;
      uint64_t lookup_key = queries[i];
      // uint64_t lookup_key = distrib(gen);
      if (cf.Contain(lookup_key) == cuckoofilter::Ok) {
        fp_count++;
        // break;
      }
    }
  }
  time_end = GetTickCount64();
  // std::cout << count << std::endl;
  std::cout << fp_count << std::endl;
  std::cout << (double)(time_end - time_start) / loop_num << std::endl;
}