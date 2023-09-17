#include "../bloom_filter/bloom_filter.h"
#include "../cache_backed_bloom_filter/cache_backed_bloom_filter.h"
#include "../hash_table/hash_table.h"
#include "../utils//utils.h"
#include <Windows.h>
#include <cstring>
#include <ctime>
#include <iostream>
#include <random>
#include <time.h>

#define TEST_FOR_BF
// #define TEST_FOR_CBBF
//  #define BASELINE_FOR_ATF
bool cmp(const util::Key &x, const util::Key &y) { return x.val < y.val; }
int main() {
  std::vector<util::Key> insert_keys;
  std::vector<util::Key> lookup_keys;
  std::vector<uint64_t> table;
  util::read_workload("C:\\study\\papercode\\infocom24\\insert_operation.txt",
                      insert_keys, true);
  util::read_workload("C:\\study\\papercode\\infocom24\\operations_uniform.txt",
                      lookup_keys, false);
  cbbf::cache_backed_bloom_filter cbbf(65536);
  cbbf.init(25, 8);
  bloomfilter::BloomFilter<uint64_t> bf(65536);
  uint64_t cnt = 0;
  struct timespec begin, end;
#ifdef BASELINE_FOR_ATF
  std::vector<util::Key> insert_keys;
  std::vector<util::Key> lookup_tombstone_keys;
  std::vector<util::Key> lookup_negative_keys;
  std::vector<uint64_t> tombstone_keys;
  std::vector<uint64_t> table;
  // std::unordered_set<uint64_t> tombstone_sets;
  // vqf_filter* vqf = vqf_init(65536 / 0.89);
  util::read_workload("insert_operations/insert_operation_tombstone30%.txt",
                      insert_keys, 1);
  util::read_workload("lookup_operations/lookup_tombsonte_operation30%.txt",
                      lookup_tombstone_keys, 0);
  util::read_workload("tombstone_operations/operations_zipf.txt",
                      lookup_negative_keys, 0);
  std::cout << "reading completed" << std::endl;
  for (uint64_t i = 0; i < insert_keys.size(); i++) {
    bf.Add(insert_keys[i].val);
  }

  for (uint64_t i = 0; i <= 100; i += 10) {
    bool is_warm_up = true;
    uint64_t count = 0;
    uint64_t tombstone_lookup_num = (double)1000000 * i / 100;
    uint64_t negative_lookup_num = 1000000 - tombstone_lookup_num;
    std::random_device rd;
    std::mt19937 rng(rd());
    std::shuffle(lookup_tombstone_keys.begin(), lookup_tombstone_keys.end(),
                 rng);
    std::shuffle(lookup_negative_keys.begin(), lookup_negative_keys.end(), rng);
    std::vector<uint64_t> lookup_keys;
    for (uint64_t j = 0; j < tombstone_lookup_num; j++)
      lookup_keys.push_back(lookup_tombstone_keys[j].val);
    for (uint64_t j = 0; j < negative_lookup_num; j++)
      lookup_keys.push_back(lookup_negative_keys[j].val);
    std::shuffle(lookup_keys.begin(), lookup_keys.end(), rng);
    for (uint64_t j = 0; j < lookup_keys.size(); j++) {
      int ans = bf.Contain(lookup_keys[j]);
      if (ans == 0)
        count++;
    }
    std::cout << count << std::endl;
  }
#endif
#ifdef TEST_FOR_CBBF
  for (uint64_t i = 0; i < insert_keys.size(); i++)
    cbbf.insert_bloom_filter(insert_keys[i].val);
  clock_gettime(CLOCK_REALTIME, &begin);
  for (uint64_t i = 0; i < lookup_keys.size(); i++) {
    bool ans = cbbf.lookup(lookup_keys[i].val);
    if (i % 100000 == 0 && i != 0) {
      std::cout << cnt << std::endl;
      table.push_back(cnt);
    }
    // uint64_t old_cnt;
    if (ans) {
      // bool ground_truth=std::binary_search(insert_keys.begin(),
      // insert_keys.end(), lookup_keys[i], cmp);
      if (ans) {
        cnt++;
        // old_cnt = cnt;
        cbbf.insert_hash_table(lookup_keys[i].val);
        // assert(old_cnt == cnt);
      }
    }
  }
  clock_gettime(CLOCK_REALTIME, &end);
  long seconds = end.tv_sec - begin.tv_sec;
  long nanoseconds = end.tv_nsec - begin.tv_nsec;
  double time_elapsed = seconds * (double)1000 + nanoseconds / (double)1000000;
  // for (uint64_t i = 0; i < insert_keys.size(); i++)
  //{
  //	bool ans = cbbf.lookup(insert_keys[i].val);
  //	if (!ans)
  //	{
  //		std::cout << i<<std::endl;
  // cnt++;
  // cbbf.insert_hash_table(lookup_keys[i].val);
  //	}
  //}
  ofstream outfile("false_positives");
  for (uint64_t i = 0; i < table.size(); i++)
    outfile << table[i] << std::endl;
  outfile.close();
  std::cout << cnt << std::endl;
  std::cout << time_elapsed << "ms" << std::endl;
#endif // TEST_FOR_CBBF
#ifdef TEST_FOR_BF
  for (uint64_t i = 0; i < insert_keys.size(); i++)
    bf.Add(insert_keys[i].val);
  // clock_gettime(CLOCK_REALTIME, &begin);
  for (uint64_t i = 0; i < lookup_keys.size(); i++) {
    int ans = bf.Contain(lookup_keys[i].val);
    if (i % 100000 == 0 && i != 0) {
      // std::cout << cnt << std::endl;
      table.push_back(cnt);
    }
    // uint64_t old_cnt;
    if (ans == 0) {
      // bool ground_truth=std::binary_search(insert_keys.begin(),
      // insert_keys.end(), lookup_keys[i], cmp); if(!ground_truth)
      cnt++;
      // old_cnt = cnt;
      // cbbf.insert_hash_table(lookup_keys[i].val);
      // assert(old_cnt == cnt);
    }
  }
  // clock_gettime(CLOCK_REALTIME, &end);
  long seconds = end.tv_sec - begin.tv_sec;
  long nanoseconds = end.tv_nsec - begin.tv_nsec;
  double time_elapsed = seconds * (double)1000 + nanoseconds / (double)1000000;
  std::cout << cnt << std::endl;
  // std::cout << seconds << "s" << std::endl;
  // std::cout << nanoseconds << "ns" << std::endl;
  // std::cout << time_elapsed << "ms" << std::endl;
#endif //  TEST_FOR_BF
}