#include "bloom_filter.h"
#include "utils.h"
#include <cstdint>
#include <iostream>
int main() {

  // std::vector<util::Key> insert_keys;
  std::vector<util::Key> lookup_keys;
  std::vector<uint64_t *> table;
  std::vector<util::Key> insert_keys;
  std::random_device rd;
  std::unordered_map<uint64_t, uint64_t> negatives_map;
  // std::unordered_map<uint64_t, uint64_t> fpr_map;
  std::mt19937 gen(rd());
  std::vector<uint64_t> negatives;
  std::vector<uint64_t> positives;
  std::vector<uint64_t> weights;
  std::unordered_map<uint64_t, uint64_t> is_fp;
  // util::read_workload(
  //     "C:\\study\\papercode\\infocom24\\operation_insert_ycsb.txt",
  //     lookup_keys, true);
  util::read_file(
      "C:\\study\\papercode\\jounal_of_software\\nodes_file\\positive_k_14.txt",
      positives);
  util::read_file(
      "C:\\study\\papercode\\jounal_of_software\\nodes_file\\negative_k_14.txt",
      negatives);
  std::cout << "reading complete" << std::endl;
  uint64_t bpk = 9;
  std::cout << bpk * positives.size() / 8 << std::endl;
  bloomfilter::BloomFilter<uint64_t> bf(positives.size(), bpk, true);
  for (uint64_t i = 0; i < positives.size(); i++)
    bf.Add(positives[i]);
  uint64_t tot_cost = 0;
  for (uint64_t i = 0; i < negatives.size(); i++)
    if (bf.Contain(negatives[i]) == bloomfilter::Ok)
      tot_cost += 1;
  std::cout << tot_cost / negatives.size() << ",";
}