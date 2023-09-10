#include <iostream>
#include <random>
// magic_number:
// mult : 0xc6a4a7935bd1e995
// incr : 0x5bd1e9955bd1e995
template <typename T> class linear_congruential_hash {
public:
  linear_congruential_hash() {
    std::random_device random;
    /*
        mult = random();
        mult |= 0xc6a4a793;
        mult <<= 32;
        mult |= random();
        mult |= 0x5bd1e995;
        mult |= 1;
        incr = random();
        incr |= 0x5bd1e995;
        incr <<= 32;
        incr |= random();
        incr |= 0x5bd1e995;
    */
    // std::cout << mult << std::endl;
    // std::cout << incr << std::endl;
    mult = 0xc6a4a7935bd1e995ULL;
    incr = 0x5bd1e9955bd1e995ULL;
    // mult = 0xc6a4a793;
    // incr = 0x5bd1e995;
    //      mult |= random();
    //      mult |= random() << 32;
    //      mult |= 1;
    //      incr |= random();
    //      incr |= random() << 32;
  }
  T operator()(T key) const { return mult * key + incr; }

private:
  T mult;
  T incr;
};