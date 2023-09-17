#include "cuckoofilter.h"
#include "scrambled_zipfian_generator.h"
#include "utils.h"
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <time.h>
#include <unordered_set>
#include <windows.h>

int main() {
  // cuckoofilter::CuckooFilter<size_t, 10> cf(498073);
  std::vector<util::Key> lookup_keys;
  std::vector<util::Key> insert_keys;
  std::unordered_set<uint64_t> insert_set;

  util::read_workload(
      "C:\\study\\papercode\\infocom24\\insert_opreation_uniform.txt",
      insert_keys);
  util::read_workload("C:\\study\\papercode\\infocom24\\operations_bell.txt",
                      lookup_keys, true);
  std::cout << "reading complete!" << std::endl;
  /*
  for (uint64_t i = 0; i < 498073; i++)
    insert_set.insert(insert_keys[i].val);
  for (uint64_t i = 0; i < 498073; i++) {
    if (cf.Add(insert_keys[i].val) != cuckoofilter::Ok) {
      std::cout << "over flow!";
      assert(0);
    }
  }
  for (uint64_t i = 0; i < 498073; i++) {
    if (cf.Contain(insert_keys[i].val) != cuckoofilter::Ok) {
      std::cout << "lookup error!" << std::endl;
      assert(0);
    }
  }
  std::cout << "pass test!" << std::endl;
  */
  /*
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<uint64_t> distrib(0, MAXUINT64);
  std::vector<uint64_t> operation_seq;
  // for (uint64_t i = 0; i < 50000000; i++)
  //   operation_seq.push_back(distrib(gen) % 62259);
  std::cout << "operations generate complete!" << std::endl;
  uint64_t fp_count = 0;
  DWORD time_start, time_end;
    for (uint64_t i = 0; i < lookup_keys.size(); i++) {
      // if (i % 100000 == 0 && i != 0) {
      //    uint64_t *row;
      //    row = new uint64_t[2];
      //    row[0] = fpr_count;
      //    row[1] = success_count;
      //    table.push_back(row);
      //   delete[] row;
      //  std::cout << fp_count << std::endl;
      //}
      if (cf.Contain(lookup_keys[i].val) == cuckoofilter::Ok)
        fp_count++;
    }
    */
  /*
  uint64_t false_negative = 0;
  time_start = GetTickCount64();
  for (auto op_index : operation_seq) {
    if (cf.Contain(insert_keys[op_index].val) != cuckoofilter::Ok)
      false_negative++;
  }
  time_end = GetTickCount64();
  */
  // std::cout << (time_end - time_start) << "ms" << std::endl;
  // std::cout << false_negative;
  uint64_t count = 0;
  uint64_t fail_count = 0;
  uint64_t fp_count = 0;
  uint64_t _min = 0;
  uint64_t _max = 1999999;
  double theta = 0.25;
  for (uint64_t k = 0; k < 100; k++) {
    bool fail_flag = false;
    cuckoofilter::CuckooFilter<size_t, 10> cf(498073);
    for (uint64_t i = 0; i < 498073; i++) {
      if (cf.Add(insert_keys[i].val) != cuckoofilter::Ok) {
        std::cout << "over flow!";
        fail_count++;
        fail_flag = true;
        break;
        assert(0);
      }
    }
    if (fail_flag)
      continue;
    for (uint64_t i = 0; i < lookup_keys.size(); i++) {
      count++;
      uint64_t lookup_key = lookup_keys[i].val;
      // uint64_t lookup_key = distrib(gen);
      if (cf.Contain(lookup_key) == cuckoofilter::Ok) {
        if (!insert_set.contains(lookup_key)) {
          fp_count++;
          // break;
        }
      }
    }
  }
  // std::cout << count << std::endl;
  std::cout << (double)fp_count / (100 - fail_count) / lookup_keys.size()
            << ",";
  fp_count = 0;
  for (uint64_t k = 0; k < 100; k++) {
    bool fail_flag = false;
    cuckoofilter::CuckooFilter<size_t, 11> cf(498073);
    for (uint64_t i = 0; i < 498073; i++) {
      if (cf.Add(insert_keys[i].val) != cuckoofilter::Ok) {
        std::cout << "over flow!";
        fail_count++;
        fail_flag = true;
        break;
        assert(0);
      }
    }
    if (fail_flag)
      continue;
    for (uint64_t i = 0; i < lookup_keys.size(); i++) {
      count++;
      uint64_t lookup_key = lookup_keys[i].val;
      // uint64_t lookup_key = distrib(gen);
      if (cf.Contain(lookup_key) == cuckoofilter::Ok) {
        if (!insert_set.contains(lookup_key)) {
          fp_count++;
          // break;
        }
      }
    }
  }
  // std::cout << count << std::endl;
  std::cout << (double)fp_count / (100 - fail_count) / lookup_keys.size()
            << ",";
  fp_count = 0;
  for (uint64_t k = 0; k < 100; k++) {
    bool fail_flag = false;
    cuckoofilter::CuckooFilter<size_t, 12> cf(498073);
    for (uint64_t i = 0; i < 498073; i++) {
      if (cf.Add(insert_keys[i].val) != cuckoofilter::Ok) {
        std::cout << "over flow!";
        fail_count++;
        fail_flag = true;
        break;
        assert(0);
      }
    }
    if (fail_flag)
      continue;
    for (uint64_t i = 0; i < lookup_keys.size(); i++) {
      count++;
      uint64_t lookup_key = lookup_keys[i].val;
      // uint64_t lookup_key = distrib(gen);
      if (cf.Contain(lookup_key) == cuckoofilter::Ok) {
        if (!insert_set.contains(lookup_key)) {
          fp_count++;
          // break;
        }
      }
    }
  }
  // std::cout << count << std::endl;
  std::cout << (double)fp_count / (100 - fail_count) / lookup_keys.size()
            << ",";
  fp_count = 0;
  for (uint64_t k = 0; k < 100; k++) {
    bool fail_flag = false;
    cuckoofilter::CuckooFilter<size_t, 13> cf(498073);
    for (uint64_t i = 0; i < 498073; i++) {
      if (cf.Add(insert_keys[i].val) != cuckoofilter::Ok) {
        std::cout << "over flow!";
        fail_count++;
        fail_flag = true;
        break;
        assert(0);
      }
    }
    if (fail_flag)
      continue;
    for (uint64_t i = 0; i < lookup_keys.size(); i++) {
      count++;
      uint64_t lookup_key = lookup_keys[i].val;
      // uint64_t lookup_key = distrib(gen);
      if (cf.Contain(lookup_key) == cuckoofilter::Ok) {
        if (!insert_set.contains(lookup_key)) {
          fp_count++;
          // break;
        }
      }
    }
  }
  // std::cout << count << std::endl;
  std::cout << (double)fp_count / (100 - fail_count) / lookup_keys.size()
            << ",";
  fp_count = 0;
  for (uint64_t k = 0; k < 100; k++) {
    bool fail_flag = false;
    cuckoofilter::CuckooFilter<size_t, 14> cf(498073);
    for (uint64_t i = 0; i < 498073; i++) {
      if (cf.Add(insert_keys[i].val) != cuckoofilter::Ok) {
        std::cout << "over flow!";
        fail_count++;
        fail_flag = true;
        break;
        assert(0);
      }
    }
    if (fail_flag)
      continue;
    for (uint64_t i = 0; i < lookup_keys.size(); i++) {
      count++;
      uint64_t lookup_key = lookup_keys[i].val;
      // uint64_t lookup_key = distrib(gen);
      if (cf.Contain(lookup_key) == cuckoofilter::Ok) {
        if (!insert_set.contains(lookup_key)) {
          fp_count++;
          // break;
        }
      }
    }
  }
  // std::cout << count << std::endl;
  std::cout << (double)fp_count / (100 - fail_count) / lookup_keys.size()
            << ",";
  fp_count = 0;
  for (uint64_t k = 0; k < 100; k++) {
    bool fail_flag = false;
    cuckoofilter::CuckooFilter<size_t, 15> cf(498073);
    for (uint64_t i = 0; i < 498073; i++) {
      if (cf.Add(insert_keys[i].val) != cuckoofilter::Ok) {
        std::cout << "over flow!";
        fail_count++;
        fail_flag = true;
        break;
        assert(0);
      }
    }
    if (fail_flag)
      continue;
    for (uint64_t i = 0; i < lookup_keys.size(); i++) {
      count++;
      uint64_t lookup_key = lookup_keys[i].val;
      // uint64_t lookup_key = distrib(gen);
      if (cf.Contain(lookup_key) == cuckoofilter::Ok) {
        if (!insert_set.contains(lookup_key)) {
          fp_count++;
          // break;
        }
      }
    }
  }
  // std::cout << count << std::endl;
  std::cout << (double)fp_count / (100 - fail_count) / lookup_keys.size()
            << ",";
  fp_count = 0;
  for (uint64_t k = 0; k < 100; k++) {
    bool fail_flag = false;
    cuckoofilter::CuckooFilter<size_t, 16> cf(498073);
    for (uint64_t i = 0; i < 498073; i++) {
      if (cf.Add(insert_keys[i].val) != cuckoofilter::Ok) {
        std::cout << "over flow!";
        fail_count++;
        fail_flag = true;
        break;
        assert(0);
      }
    }
    if (fail_flag)
      continue;
    for (uint64_t i = 0; i < lookup_keys.size(); i++) {
      count++;
      uint64_t lookup_key = lookup_keys[i].val;
      // uint64_t lookup_key = distrib(gen);
      if (cf.Contain(lookup_key) == cuckoofilter::Ok) {
        if (!insert_set.contains(lookup_key)) {
          fp_count++;
          // break;
        }
      }
    }
  }
  // std::cout << count << std::endl;
  std::cout << (double)fp_count / (100 - fail_count) / lookup_keys.size()
            << ",";
  fp_count = 0;
  for (uint64_t k = 0; k < 100; k++) {
    bool fail_flag = false;
    cuckoofilter::CuckooFilter<size_t, 17> cf(498073);
    for (uint64_t i = 0; i < 498073; i++) {
      if (cf.Add(insert_keys[i].val) != cuckoofilter::Ok) {
        std::cout << "over flow!";
        fail_count++;
        fail_flag = true;
        break;
        assert(0);
      }
    }
    if (fail_flag)
      continue;
    for (uint64_t i = 0; i < lookup_keys.size(); i++) {
      count++;
      uint64_t lookup_key = lookup_keys[i].val;
      // uint64_t lookup_key = distrib(gen);
      if (cf.Contain(lookup_key) == cuckoofilter::Ok) {
        if (!insert_set.contains(lookup_key)) {
          fp_count++;
          // break;
        }
      }
    }
  }
  // std::cout << count << std::endl;
  std::cout << (double)fp_count / (100 - fail_count) / lookup_keys.size()
            << ",";
  fp_count = 0;
  for (uint64_t k = 0; k < 100; k++) {
    bool fail_flag = false;
    cuckoofilter::CuckooFilter<size_t, 18> cf(498073);
    for (uint64_t i = 0; i < 498073; i++) {
      if (cf.Add(insert_keys[i].val) != cuckoofilter::Ok) {
        std::cout << "over flow!";
        fail_count++;
        fail_flag = true;
        break;
        assert(0);
      }
    }
    if (fail_flag)
      continue;
    for (uint64_t i = 0; i < lookup_keys.size(); i++) {
      count++;
      uint64_t lookup_key = lookup_keys[i].val;
      // uint64_t lookup_key = distrib(gen);
      if (cf.Contain(lookup_key) == cuckoofilter::Ok) {
        if (!insert_set.contains(lookup_key)) {
          fp_count++;
          // break;
        }
      }
    }
  }
  // std::cout << count << std::endl;
  std::cout << (double)fp_count / (100 - fail_count) / lookup_keys.size()
            << ",";
  fp_count = 0;
  for (uint64_t k = 0; k < 100; k++) {
    bool fail_flag = false;
    cuckoofilter::CuckooFilter<size_t, 19> cf(498073);
    for (uint64_t i = 0; i < 498073; i++) {
      if (cf.Add(insert_keys[i].val) != cuckoofilter::Ok) {
        std::cout << "over flow!";
        fail_count++;
        fail_flag = true;
        break;
        assert(0);
      }
    }
    if (fail_flag)
      continue;
    for (uint64_t i = 0; i < lookup_keys.size(); i++) {
      count++;
      uint64_t lookup_key = lookup_keys[i].val;
      // uint64_t lookup_key = distrib(gen);
      if (cf.Contain(lookup_key) == cuckoofilter::Ok) {
        if (!insert_set.contains(lookup_key)) {
          fp_count++;
          // break;
        }
      }
    }
  }
  // std::cout << count << std::endl;
  std::cout << (double)fp_count / (100 - fail_count) / lookup_keys.size()
            << ",";
  fp_count = 0;
  for (uint64_t k = 0; k < 100; k++) {
    bool fail_flag = false;
    cuckoofilter::CuckooFilter<size_t, 20> cf(498073);
    for (uint64_t i = 0; i < 498073; i++) {
      if (cf.Add(insert_keys[i].val) != cuckoofilter::Ok) {
        std::cout << "over flow!";
        fail_count++;
        fail_flag = true;
        break;
        assert(0);
      }
    }
    if (fail_flag)
      continue;
    for (uint64_t i = 0; i < lookup_keys.size(); i++) {
      count++;
      uint64_t lookup_key = lookup_keys[i].val;
      // uint64_t lookup_key = distrib(gen);
      if (cf.Contain(lookup_key) == cuckoofilter::Ok) {
        if (!insert_set.contains(lookup_key)) {
          fp_count++;
          // break;
        }
      }
    }
  }
  // std::cout << count << std::endl;
  std::cout << (double)fp_count / (100 - fail_count) / lookup_keys.size()
            << ",";
}