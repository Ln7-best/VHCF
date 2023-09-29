# VHCF

## Introduction

This is the implementation for VHCF and other baselines.
The directory "variable_hash_cuckoo_filter" contains three different versions of VHCF:

- cuckoofilter_fast.h

  The fast implementation for VHCF with 8 bits fingerprint

- cuckoofilter_flex.h

  This implementation supports arbitrary number setting of buckets in VHCF

  This implementation may sacrifice max load factor a little

- cuckoofilter.h

  Implementation for VHCF which supports arbitrary length of fingerprint

  Note that the number of buckets must be power of two

## How to run

First, include the header file "cuckoofilter_fast.h", "cuckoofilter_flex.h", "cuckoofilter.h" in the main file, then compile the project .

Create a VHCF:

```c++
// Create a VHCF with
// key type: unsigned long long
// fingerprint: length fp_bit
// max number of elemnets: pos_size
cuckoofilter::CuckooFilter<uint64_t, fp_bits> cf(pos_size);
```

Insert an element:

```c++
//Insert element e
cf.Add(e);
```

Adapt:

```C++
std::vector<uint64_t> negatives;
std::vector<uint64_t> weights;
//For "cuckoofilter_flex.h" or "cuckoofilter.h"
cf.FindBestHashChooser(negatives, weights);
//For "cuckoofilter_fast.h"
cf.FindBestHashChooser_FAST(negatives, weights);
```

Query:

```c++
//Ok means find
cuckoofilter::Status ans=cf.Contain(negatives[i])
```
