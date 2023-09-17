//
//  scrambled_zipfian_generator.h
//  YCSB-C
//
//  Created by Jinglei Ren on 12/8/14.
//  Copyright (c) 2014 Jinglei Ren <jinglei@ren.systems>.
//

#ifndef YCSB_C_SCRAMBLED_ZIPFIAN_GENERATOR_H_
#define YCSB_C_SCRAMBLED_ZIPFIAN_GENERATOR_H_

#include "generator.h"

#include "utils.h"
#include "zipfian_generator.h"
#include <atomic>
#include <cstdint>
const uint64_t kFNVOffsetBasis64 = 0xCBF29CE484222325;
const uint64_t kFNVPrime64 = 1099511628211;

inline uint64_t FNVHash64(uint64_t val) {
  uint64_t hash = kFNVOffsetBasis64;

  for (int i = 0; i < 8; i++) {
    uint64_t octet = val & 0x00ff;
    val = val >> 8;

    hash = hash ^ octet;
    hash = hash * kFNVPrime64;
  }
  return hash;
}

namespace ycsbc {

class ScrambledZipfianGenerator : public Generator<uint64_t> {
public:
  ScrambledZipfianGenerator(
      uint64_t min, uint64_t max,
      double zipfian_const = ZipfianGenerator::kZipfianConst)
      : base_(min), num_items_(max - min + 1),
        generator_(min, max, zipfian_const) {}

  ScrambledZipfianGenerator(uint64_t num_items)
      : ScrambledZipfianGenerator(0, num_items - 1) {}

  uint64_t Next();
  uint64_t Last();

private:
  const uint64_t base_;
  const uint64_t num_items_;
  ZipfianGenerator generator_;

  uint64_t Scramble(uint64_t value) const;
};

inline uint64_t ScrambledZipfianGenerator::Scramble(uint64_t value) const {
  // return base_ + FNVHash64(value) % num_items_;
  return base_ + FNVHash64(value);
}

inline uint64_t ScrambledZipfianGenerator::Next() {
  return Scramble(generator_.Next());
}

inline uint64_t ScrambledZipfianGenerator::Last() {
  return Scramble(generator_.Last());
}

} // namespace ycsbc

#endif // YCSB_C_SCRAMBLED_ZIPFIAN_GENERATOR_H_
