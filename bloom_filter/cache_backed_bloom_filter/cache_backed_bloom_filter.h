#pragma once
#include "../bloom_filter/bloom_filter.h"
#include "../hash_table/hash_table.h"
#include <iostream>

namespace cbbf {
class cache_backed_bloom_filter {
public:
  cache_backed_bloom_filter(uint64_t bloom_filter_elements_num,
                            size_t bloom_filter_bits_per_item,
                            size_t bloom_filter_bits_per_item_float_num,
                            bool branchless);
  ~cache_backed_bloom_filter() {}
  void init(uint64_t hash_table_elements_num, uint64_t hash_table_block_size);
  void insert_bloom_filter(uint64_t key);
  void insert_hash_table(uint64_t key);
  bool lookup(uint64_t key);

private:
  uint64_t bloom_filter_hash_num;
  uint64_t bloom_filter_size;
  uint64_t hash_table_elements_num;
  uint64_t hash_table_block_size;
  bloomfilter::BloomFilter<uint64_t> bf;
  ht::hash_table ht;
};

cache_backed_bloom_filter::cache_backed_bloom_filter(
    uint64_t bloom_filter_elements_num, size_t bloom_filter_bits_per_item,
    size_t bloom_filter_bits_per_item_float_num, bool branchless)
    : bf(bloom_filter_elements_num, bloom_filter_bits_per_item,
         bloom_filter_bits_per_item_float_num, branchless) {
  this->bloom_filter_hash_num = 0;
  this->bloom_filter_size = 0;
  this->hash_table_elements_num = 0;
  this->hash_table_block_size = 0;
}

void cache_backed_bloom_filter::init(uint64_t hash_table_elements_num,
                                     uint64_t hash_table_block_size) {
  this->hash_table_elements_num = hash_table_elements_num;
  this->hash_table_block_size = hash_table_block_size;
  this->bloom_filter_size = bloom_filter_size;
  this->bloom_filter_hash_num = bloom_filter_hash_num;
  ht.init(hash_table_elements_num, hash_table_block_size);
}

void cache_backed_bloom_filter::insert_bloom_filter(uint64_t key) {
  bf.Add(key);
}

void cache_backed_bloom_filter::insert_hash_table(uint64_t key) {
  ht.insert(key);
}

bool cache_backed_bloom_filter::lookup(uint64_t key) {
  int ans = bf.Contain(key);
  if (ans == 0)
    return !ht.lookup(key);
  return false;
}

} // namespace cbbf