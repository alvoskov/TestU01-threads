#include "entropy.h"
#include <time.h>
#include <x86intrin.h>

using namespace testu01_threads;

uint64_t Entropy::SplitMixHash(uint64_t z) const
{
    z = (z ^ (z >> 30)) * UINT64_C(0xBF58476D1CE4E5B9);
    z = (z ^ (z >> 27)) * UINT64_C(0x94D049BB133111EB);
    return z ^ (z >> 31);
}

/**
 * @brief XXTEA mixing function.
 */
#define MX(p) (((z>>5^y<<2) + (y>>3^z<<4)) ^ ((sum^y) + (key[((p)&3)^e] ^ z)))


/**
 * @brief XXTEA encryption subroutine for 64-bit block. Used for
 * generation of seeds from 64-bit state.
 * @details https://www.movable-type.co.uk/scripts/xxtea.pdf
 */
uint64_t Entropy::Xxtea(const uint64_t inp) const
{
    constexpr uint32_t DELTA = 0x9e3779b9;
    constexpr unsigned int nrounds = 32;
    uint32_t y, z, sum = 0;
    union {
        uint32_t v[2];
        uint64_t x;
    } out;
    out.x = inp;
    z = out.v[1];
    for (unsigned int i = 0; i < nrounds; i++) {
        sum += DELTA;
        unsigned int e = (sum >> 2) & 3;
        y = out.v[1]; 
        z = out.v[0] += MX(0);
        y = out.v[0];
        z = out.v[1] += MX(1);
    }
    return out.x;
}

/**
 * @brief XORs input with output of hardware RNG in CPU (rdseed).
 */
uint64_t Entropy::MixRdSeed(const uint64_t x) const
{
    long long unsigned int rd;
    while (!_rdseed64_step(&rd)) {}
    return x ^ rd;
}


/**
 * @brief Generates the next state and returns it in the hashed form.
 * It is essentially SplitMix PRNG that will be used as an input of XXTEA.
 */
uint64_t Entropy::NextState()
{
    state += 0x9E3779B97F4A7C15;
    return MixRdSeed(SplitMixHash(state));
}



/*
# 64-bit block
00000000000000000000000000000000, 0000000000000000, ab043705808c5d57
0102040810204080fffefcf8f0e0c080, fffefcf8f0e0c080, 8c3707c01c7fccc4
https://github.com/an0maly/Crypt-XXTEA/blob/master/reference/test-vector.t
*/
bool Entropy::XxteaTest()
{
    constexpr uint64_t OUT0 = 0x575d8c80053704ab;
    constexpr uint64_t OUT1 = 0xc4cc7f1cc007378c;
    uint32_t key_old[4] = {key[0], key[1], key[2], key[3]};
    // Test 1
    key[0] = 0; key[1] = 0; key[2] = 0; key[3] = 0;
    if (Xxtea(0) != OUT0)
        return false;
    // Test 2
    key[0] = 0x08040201; key[1] = 0x80402010;
    key[2] = 0xf8fcfeff; key[3] = 0x80c0e0f0; 
    if (Xxtea(0x80c0e0f0f8fcfeff) != OUT1)
        return false;    
    // Restore key
    for (int i = 0; i < 4; i++) { key[i] = key_old[i]; }
    return true;
}


Entropy::Entropy()
{
    uint64_t seed0 = MixRdSeed(SplitMixHash(time(NULL)));
    uint64_t seed1 = MixRdSeed(SplitMixHash(~seed0));
    seed1 ^= MixRdSeed(SplitMixHash(__rdtsc()));
    key[0] = seed0; key[1] = seed0 >> 32;
    key[2] = seed1; key[3] = seed1 >> 32;
    state = time(NULL);
//    printf("%X %X %X %X\n", key[0], key[1], key[2], key[3]);
}

/**
 * @brief Thread-safe function for returning seed.
 * @details It is a very slow function.
 */
uint64_t Entropy::Seed64()
{
    std::lock_guard<std::mutex> guard(mut);
    uint64_t seed = Xxtea(NextState());
    seeds_log.push_back(seed);
    return seed;
}
