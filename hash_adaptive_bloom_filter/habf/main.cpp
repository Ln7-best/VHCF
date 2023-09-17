#include "fasthabf.h"
#include "habf.h"
#include "utils.h"
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <random>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

bool cmp(Slice *a, Slice *b) { return a->cost > b->cost; }
int main() {
  // std::vector<util::Key> insert_keys;
  std::vector<util::Key> lookup_keys;
  std::vector<uint64_t *> table;
  std::vector<util::Key> insert_keys;
  std::unordered_map<uint64_t, uint64_t> negatives_map;
  // std::unordered_map<uint64_t, uint64_t> fpr_map;
  std::vector<uint64_t> negatives;
  std::vector<uint64_t> weights;
  std::unordered_map<uint64_t, uint64_t> is_fp;
  util::read_workload(
      "C:\\study\\papercode\\infocom24\\insert_opreation_uniform.txt",
      insert_keys);
  // util::read_workload(
  //     "C:\\study\\papercode\\infocom24\\operation_insert_ycsb.txt",
  //     lookup_keys, true);
  util::read_file("C:\\study\\papercode\\infocom24\\caida_raw.txt", negatives,
                  7000000);
  util::read_file("C:\\study\\papercode\\infocom24\\caida_weights_0.5.txt",
                  weights, 7000000);
  std::cout << "reading complete" << std::endl;
  std::vector<Slice *> pos;
  std::vector<Slice *> neg;
  std::vector<Slice *> top_neg;
  for (uint64_t i = 0; i < insert_keys.size(); i++) {
    Slice *data = new Slice;
    data->str = std::to_string(insert_keys[i].val);
    data->cost = 1;
    pos.push_back(data);
  }
  for (uint64_t i = 0; i < negatives.size(); i++) {
    Slice *data = new Slice;
    data->str = std::to_string(negatives[i]);
    data->cost = 1;
    neg.push_back(data);
  }
  /*
  sort(neg.begin(), neg.end(), cmp);
  for (uint64_t i = 0; i < 2 * pos.size(); i++) {
    top_neg.push_back(neg[i]);
  }
  */

  std::random_device rd;
  std::mt19937 rng(rd());
  std::shuffle(neg.begin(), neg.end(), rng);
  std::unordered_set<uint64_t> rand_nums;
  for (uint64_t i = 0; i < pos.size(); i++) {
    top_neg.push_back(neg[i]);
  }
  for (uint64_t bpk = 7; bpk <= 7; bpk++) {
    uint64_t weight = 0;
    // fasthabf::FastHABFilter f(bpk / 0.95, insert_keys.size());
    habf::HABFilter f(bpk / 0.95, insert_keys.size());
    f.AddAndOptimize(pos, top_neg);
    uint64_t tot_weight = 0;
    for (auto data : neg) {
      bool ans = f.Contain(*data);
      if (ans)
        weight += data->cost;
      tot_weight += data->cost;
    }
    std::cout << (double)weight << std::endl;
    std::cout << (double)weight / tot_weight << std::endl;
  }
}