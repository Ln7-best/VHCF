#ifndef CUCKOO_FILTER_CUCKOO_FILTER_H_
#define CUCKOO_FILTER_CUCKOO_FILTER_H_

// #include "crc64.h"
#include "MurmurHash3.h"
#include "linear_congruential_hash.h"
#include <algorithm>
#include <assert.h>
#include <corecrt.h>
#include <cstddef>
#include <cstdint>
#include <vector>

// #include "debug.h"
#include "hashutil.h"
#include "singletable_fast.h"
std::vector<uint64_t> bucket_negatives[140937];
std::vector<uint8_t *> bucket_negatives_hash[140937];
std::vector<uint64_t> bucket_weights[140937];
linear_congruential_hash<uint64_t> _hasher;
namespace cuckoofilter {
// status returned by a cuckoo filter operation
enum Status {
  Ok = 0,
  NotFound = 1,
  NotEnoughSpace = 2,
  NotSupported = 3,
};

// maximum number of cuckoo kicks before claiming failure
const size_t kMaxCuckooCount = 500;

// A cuckoo filter class exposes a Bloomier filter interface,
// providing methods of Add, Delete, Contain. It takes three
// template parameters:
//   ItemType:  the type of item you want to insert
//   bits_per_item: how many bits each item is hashed into
//   TableType: the storage of table, SingleTable by default, and
// PackedTable to enable semi-sorting
template <typename ItemType, size_t bits_per_item,
          template <size_t> class TableType = SingleTable,
          typename HashFamily = TwoIndependentMultiplyShift>
class CuckooFilter {
  // Storage of items
  TableType<bits_per_item> *table_;
  uint64_t (*remote)[4];
  // Number of items stored
  size_t num_items_;
  uint32_t seed1;
  uint32_t seed2;
  typedef struct {
    size_t index;
    uint32_t tag;
    bool used;
  } VictimCache;
  uint64_t n_hashchooser;
  VictimCache victim_;
  HashFamily hasher_;
  HashFamily hasher1_;
  HashFamily hasher2_;

  inline size_t IndexHash(uint32_t hv) const {
    // table_->num_buckets is always a power of two, so modulo can be replaced
    // with
    // bitwise-and:
    return hv & (table_->NumBuckets() - 1);
  }

  inline uint32_t TagHash(const ItemType &item, uint32_t hash_chooser) const {
    uint64_t hv[2];
    uint32_t tag = 0;
    uint32_t n_val = 128 / bits_per_item;
    // uint64_t bits_index = hash_chooser * bits_per_item;
    if (hash_chooser >= n_val) {
      MurmurHash3_x64_128(&item, 8, seed2, hv);
      hash_chooser -= n_val;
    } else
      MurmurHash3_x64_128(&item, 8, seed1, hv);
    uint64_t bits_index = hash_chooser * bits_per_item;
    uint32_t tag_mask = ((1ULL << bits_per_item) - 1);
    if (bits_index > 63) {
      tag = hv[1] >> (bits_index - 64);
      tag &= tag_mask;
      if (tag == 0)
        tag += 1;
      return tag;
    }
    if (bits_index + bits_per_item <= 64) {
      tag = hv[0] >> bits_index;
      tag &= tag_mask;
      if (tag == 0)
        tag += 1;
      return tag;
    }
    uint32_t lower_tag = hv[0] >> bits_index;
    uint32_t higher_bits_len = bits_index + bits_per_item - 64;
    uint64_t higher_tag_mask = ((1ULL << higher_bits_len) - 1);
    uint32_t higher_tag = hv[1] & higher_tag_mask;
    tag = lower_tag + (higher_tag << (bits_per_item - higher_bits_len));
    if (tag == 0)
      tag += 1;
    return tag;
  }

  inline void GenerateIndexTagHash(const ItemType &item, size_t *index) const {
    const uint64_t hash = hasher_(item);
    // const uint64_t hash = crc64(&item, 8);
    *index = IndexHash(hash >> 32);
  }

  inline size_t AltIndex(const size_t index, const uint32_t tag) const {
    // NOTE(binfan): originally we use:
    // index ^ HashUtil::BobHash((const void*) (&tag), 4)) &
    // table_->INDEXMASK;
    // now doing a quick-n-dirty way:
    // 0x5bd1e995 is the hash constant from MurmurHash2
    return IndexHash((uint32_t)(index ^ (tag * 0x5bd1e995)));
  }

  Status AddImpl(const size_t i, const ItemType &item);

  // load factor is the fraction of occupancy
  double LoadFactor() const { return 1.0 * Size() / table_->SizeInTags(); }

  double BitsPerItem() const { return 8.0 * table_->SizeInBytes() / Size(); }

public:
  size_t num_buckets;
  explicit CuckooFilter(const size_t max_num_keys)
      : num_items_(0), victim_(), hasher_(), seed1(0xc6a4a793),
        seed2(0x5bd1e995), n_hashchooser(2) {
    size_t assoc = 4;
    // num_buckets = max_num_keys / 0.92 / assoc;
    num_buckets = upperpower2(std::max<uint64_t>(1, max_num_keys / assoc));
    double frac = (double)max_num_keys / num_buckets / assoc;
    if (frac > 0.96) {
      num_buckets <<= 1;
    }
    victim_.used = false;
    table_ = new TableType<bits_per_item>(num_buckets);
    remote = new uint64_t[num_buckets][4];
    for (size_t i = 0; i < num_buckets; i++) {
      for (uint64_t j = 0; j < 4; j++)
        remote[i][j] = 0;
    }
  }

  void FindBestHashChooser(std::vector<ItemType> negatives,
                           std::vector<uint64_t> weights);
  void FindBestHashChooser_FAST(std::vector<ItemType> negatives,
                                std::vector<uint64_t> weights);
  ~CuckooFilter() { delete table_; }

  void adjust_hash(size_t i, size_t hash_chooser);

  // Add an item to the filter.
  Status Add(const ItemType &item);

  // Report if the item is inserted, with false positive rate.
  Status Contain(const ItemType &item) const;

  // Delete an key from the filter
  Status Delete(const ItemType &item);

  /* methods for providing stats  */
  // summary infomation
  std::string Info() const;

  // number of current inserted items;
  size_t Size() const { return num_items_; }

  // size of the filter in bytes.
  size_t SizeInBytes() const { return table_->SizeInBytes(); }
};

template <typename ItemType, size_t bits_per_item,
          template <size_t> class TableType, typename HashFamily>
Status CuckooFilter<ItemType, bits_per_item, TableType, HashFamily>::Add(
    const ItemType &item) {
  size_t i;
  uint32_t tag;
  const uint64_t hash = hasher_(item);
  // GenerateIndexTagHash(item, &i);
  i = IndexHash(hash >> 32);
  //  if (i >= table_->NumBuckets())
  //    std::cout << "yes" << std::endl;
  return AddImpl(i, item);
}

template <typename ItemType, size_t bits_per_item,
          template <size_t> class TableType, typename HashFamily>

void CuckooFilter<ItemType, bits_per_item, TableType, HashFamily>::
    FindBestHashChooser(std::vector<ItemType> negatives,
                        std::vector<uint64_t> weights) {

  uint64_t cost = 0;
  for (uint64_t i = 0; i < negatives.size(); i++) {
    ItemType negative_key = negatives[i];
    const uint64_t hash = hasher_(negative_key);
    size_t index = IndexHash(hash >> 32);
    // if (bits_per_item == 8)
    // else
    uint32_t tag = TagHash(negative_key, 0);
    size_t alt_index = AltIndex(index, tag);
    bucket_negatives[index].push_back(negative_key);
    bucket_weights[index].push_back(weights[i]);
    bucket_negatives[alt_index].push_back(negative_key);
    bucket_weights[alt_index].push_back(weights[i]);
  }

  for (uint64_t i = 0; i < table_->NumBuckets(); i++) {
    uint64_t min_fp_cost = 0xffffffffffffffff;
    uint64_t best_hash_chooser = 0;
    // std::cout << i << ":";
    for (uint64_t hash_chooser = 0; hash_chooser < n_hashchooser;
         hash_chooser++) {
      adjust_hash(i, hash_chooser);
      uint64_t fp_cost = 0;
      for (uint64_t j = 0; j < bucket_negatives[i].size(); j++) {
        uint32_t tag = TagHash(bucket_negatives[i][j], hash_chooser);
        if (table_->FindTagInBucket(i, tag)) {
          fp_cost += bucket_weights[i][j];
        }
      }
      // std::cout << " " << fp_cost;
      if (fp_cost <= min_fp_cost) {
        min_fp_cost = fp_cost;
        best_hash_chooser = hash_chooser;
      }
      if (fp_cost == 0)
        break;
    }
    if (min_fp_cost != 0) {
      cost += min_fp_cost;
      adjust_hash(i, best_hash_chooser);
    }
    // std::cout << std::endl;
  }
  std::cout << cost << ",";
}
template <typename ItemType, size_t bits_per_item,
          template <size_t> class TableType, typename HashFamily>

void CuckooFilter<ItemType, bits_per_item, TableType, HashFamily>::
    FindBestHashChooser_FAST(std::vector<ItemType> negatives,
                             std::vector<uint64_t> weights) {

  uint64_t cost = 0;
  for (uint64_t i = 0; i < negatives.size(); i++) {
    ItemType negative_key = negatives[i];
    const uint64_t hash = hasher_(negative_key);
    uint8_t *hv = new uint8_t[16];
    uint64_t *p = (uint64_t *)hv;
#ifdef LC
    p[0] = _hasher(negative_key);
    p[1] = _hasher(p[0]);
#else
    p[0] = hasher1_(negative_key);
    p[1] = hasher2_(negative_key);
#endif
    // MurmurHash3_x64_128(&negative_key, 8, seed1, hv);
    size_t index = IndexHash(hash >> 32);
    uint32_t tag = hv[0];
    if (!tag)
      tag++;
    size_t alt_index = AltIndex(index, tag);
    bucket_negatives_hash[index].push_back(hv);
    bucket_weights[index].push_back(weights[i]);
    bucket_negatives_hash[alt_index].push_back(hv);
    bucket_weights[alt_index].push_back(weights[i]);
  }

  for (uint64_t i = 0; i < table_->NumBuckets(); i++) {
    uint64_t min_fp_cost = 0xffffffffffffffff;
    uint64_t best_hash_chooser = 0;
    // std::cout << i << ":";
    for (uint64_t hash_chooser = 0; hash_chooser < n_hashchooser;
         hash_chooser++) {
      adjust_hash(i, hash_chooser);
      uint64_t fp_cost = 0;
      for (uint64_t j = 0; j < bucket_negatives_hash[i].size(); j++) {
        uint32_t tag = bucket_negatives_hash[i][j][hash_chooser];
        if (!tag)
          tag++;
        if (table_->FindTagInBucket(i, tag)) {
          fp_cost += bucket_weights[i][j];
        }
      }
      // std::cout << " " << fp_cost;
      if (fp_cost <= min_fp_cost) {
        min_fp_cost = fp_cost;
        best_hash_chooser = hash_chooser;
      }
      if (fp_cost == 0)
        break;
    }
    if (min_fp_cost != 0) {
      cost += min_fp_cost;
      adjust_hash(i, best_hash_chooser);
    }
    // std::cout << std::endl;
  }
  std::cout << cost << ",";
}
template <typename ItemType, size_t bits_per_item,
          template <size_t> class TableType, typename HashFamily>
Status CuckooFilter<ItemType, bits_per_item, TableType, HashFamily>::AddImpl(
    const size_t i, const ItemType &item) {
  size_t curindex = i;
  uint32_t curtag;
  if (bits_per_item == 8) {
    // MurmurHash3_x64_128(&item, 8, seed1, hv);
#ifdef LC
    curtag = _hasher(item) & 0xff;
#else
    curtag = hasher1_(item) & 0xff;
#endif
    if (!curtag)
      curtag += 1;
  } else
    curtag = TagHash(item, 0);
  uint32_t oldtag;
  ItemType cur_item = item;
  for (uint32_t count = 0; count < kMaxCuckooCount; count++) {
    bool kickout = count > 0;
    oldtag = 0;
    uint32_t kicked_index = 0;
    uint32_t insert_index = 0;
    if (table_->InsertTagToBucket(curindex, curtag, kickout, oldtag,
                                  kicked_index, insert_index)) {
      remote[curindex][insert_index] = cur_item;
      num_items_++;
      return Ok;
    }
    if (kickout) {
      ItemType temp_item = cur_item;
      cur_item = remote[curindex][kicked_index];
      remote[curindex][kicked_index] = temp_item;
      curtag = oldtag;
    }
    // uint64_t hash = hasher_(cur_item);
    // hash >>= 32;
    // curindex = (hash * table_->NumBuckets()) >> 32;
    curindex = AltIndex(curindex, curtag);
  }
  return NotEnoughSpace;
}
template <typename ItemType, size_t bits_per_item,
          template <size_t> class TableType, typename HashFamily>
void CuckooFilter<ItemType, bits_per_item, TableType, HashFamily>::adjust_hash(
    size_t i, size_t hash_chooser) {
  // if (hash_chooser != 0)
  //   std::cout << "yes" << std::endl;
  // if (hash_chooser == 2)
  //   std::cout << "yes" << std::endl;
  table_->SetHashchooser(i, hash_chooser);
  if (hash_chooser != table_->GetHashChooser(i)) {
    table_->SetHashchooser(i, hash_chooser);
    size_t a = table_->GetHashChooser(i);
    std::cout << a << std::endl;
  }
  for (uint32_t j = 0; j < 4; j++) {
    ItemType item = remote[i][j];
    if (!item)
      continue;
    uint32_t tag;
    if (bits_per_item == 8) {
      uint8_t hv[16];
      // uint8_t *hv = new uint8_t[16];
      uint64_t *p = (uint64_t *)hv;
#ifdef LC
      p[0] = _hasher(item);
      p[1] = _hasher(p[0]);
#else
      p[0] = hasher1_(item);
      p[1] = hasher2_(item);
#endif
      // MurmurHash3_x64_128(&item, 8, seed1, hv);
      tag = hv[hash_chooser];
      if (!tag)
        tag += 1;
    } else
      tag = TagHash(item, hash_chooser);
    table_->WriteTag(i, j, tag);
  }
}
template <typename ItemType, size_t bits_per_item,
          template <size_t> class TableType, typename HashFamily>
Status CuckooFilter<ItemType, bits_per_item, TableType, HashFamily>::Contain(
    const ItemType &key) const {
  bool found = false;
  size_t i1, i2;
  uint32_t tag, tag1, tag2;
  uint8_t hv[16];
  // GenerateIndexTagHash(key, &i1);
  //  uint32_t index;
  if (bits_per_item == 8) {
    uint64_t *p = (uint64_t *)hv;
#ifdef LC
    p[0] = _hasher(key);
    p[1] = _hasher(p[0]);
#else
    p[0] = hasher1_(key);
    p[1] = hasher2_(key);
#endif
    // MurmurHash3_x64_128(&key, 8, seed1, hv);
    tag = hv[0];
    if (!tag)
      tag += 1;
  } else
    tag = TagHash(key, 0);
  const uint64_t hash = hasher_(key);
  i1 = IndexHash(hash >> 32);
  i2 = AltIndex(i1, tag);
  uint64_t hash_chooser1 = table_->GetHashChooser(i1);
  uint64_t hash_chooser2 = table_->GetHashChooser(i2);
  if (bits_per_item == 8) {
    tag1 = hv[hash_chooser1];
    if (!tag1)
      tag1++;
    tag2 = hv[hash_chooser2];
    if (!tag2)
      tag2++;
  } else {
    tag1 = TagHash(key, hash_chooser1);
    tag2 = TagHash(key, hash_chooser2);
  }
  assert(i1 == AltIndex(i2, tag));
  if (table_->FindTagInBuckets(i1, i2, tag1, tag2)) {
    return Ok;
  } else {
    return NotFound;
  }
}

template <typename ItemType, size_t bits_per_item,
          template <size_t> class TableType, typename HashFamily>
Status CuckooFilter<ItemType, bits_per_item, TableType, HashFamily>::Delete(
    const ItemType &key) {
  size_t i1, i2;
  uint32_t tag;

  GenerateIndexTagHash(key, &i1, &tag);
  i2 = AltIndex(i1, tag);

  if (table_->DeleteTagFromBucket(i1, tag)) {
    num_items_--;
    goto TryEliminateVictim;
  } else if (table_->DeleteTagFromBucket(i2, tag)) {
    num_items_--;
    goto TryEliminateVictim;
  } else if (victim_.used && tag == victim_.tag &&
             (i1 == victim_.index || i2 == victim_.index)) {
    // num_items_--;
    victim_.used = false;
    return Ok;
  } else {
    return NotFound;
  }
TryEliminateVictim:
  if (victim_.used) {
    victim_.used = false;
    size_t i = victim_.index;
    uint32_t tag = victim_.tag;
    AddImpl(i, tag);
  }
  return Ok;
}

template <typename ItemType, size_t bits_per_item,
          template <size_t> class TableType, typename HashFamily>
std::string
CuckooFilter<ItemType, bits_per_item, TableType, HashFamily>::Info() const {
  std::stringstream ss;
  ss << "CuckooFilter Status:\n"
     << "\t\t" << table_->Info() << "\n"
     << "\t\tKeys stored: " << Size() << "\n"
     << "\t\tLoad factor: " << LoadFactor() << "\n"
     << "\t\tHashtable size: " << (table_->SizeInBytes() >> 10) << " KB\n";
  if (Size() > 0) {
    ss << "\t\tbit/key:   " << BitsPerItem() << "\n";
  } else {
    ss << "\t\tbit/key:   N/A\n";
  }
  return ss.str();
}
} // namespace cuckoofilter
#endif // CUCKOO_FILTER_CUCKOO_FILTER_H_
