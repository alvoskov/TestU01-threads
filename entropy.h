/**
 * @file entropy.h
 * @brief Generator of random seeds for other PRNGs.
 * @details It uses some sources of entropy (timestamp, rdtsc) and hardware PRNG
 * that built in CPU (RDSEED). They are used for initialization of modified
 * SplitMix PRNG and XXTEA block cipher. Output of SplitMix is XORed with
 * RDSEED and encrypted with XXTEA.
 *
 * DON'T USE THIS MODULE FOR CRYPTOGRAPHY, E.G. GENERATION OF KEYS FOR ENCRYPTION!
 * IT IS DESIGNED ONLY FOR STATISTICAL TESTS AND PRNG SEEDING!
 *
 * The next references are used:
 *
 * 1. https://mostlymangling.blogspot.com/2018/07/on-mixing-functions-in-fast-splittable.html
 * 2. https://www.movable-type.co.uk/scripts/xxtea.pdf
 * 3. https://github.com/an0maly/Crypt-XXTEA/blob/master/reference/test-vector.t
 *
 * @copyright (c) 2024 Alexey L. Voskov, Lomonosov Moscow State University.
 * alvoskov@gmail.com
 *
 * All rights reserved.
 *
 * This software is provided under the Apache 2 License.
 *
 * In scientific publications which used this software, a reference to it
 * would be appreciated.
 */
#ifndef __ENTROPY_H
#define __ENTROPY_H
#include <stdint.h>
#include <vector>
#include <thread>
#include <mutex>
#include <iostream>


namespace testu01_threads {

enum EntropyMethod
{
    ENTROPY_TIME, ///< Seed is from system time.
    ENTROPY_RDTSC, ///< Seed is from from RDTSC instruction and system time.
    ENTROPY_RDSEED ///< Hardware RNG in CPU is used.
};

/**
 * @brief Generates seeds for PRNGs using some entropy sources: current time
 * in seconds, RDTSC instruction and RDSEED built-in hardware RNG. Everything
 * is mixed with PRNG counter and encrypted by XXTEA block cipher.
 *
 * DON'T USE FOR THIS CLASS FOR CRYPTOGRAPHY, E.G. GENERATION OF KEYS FOR
 * ENCRYPTION! IT IS DESIGNED ONLY FOR STATISTCAL TESTS AND PRNG SEEDING!
 *
 * @details. It uses the next algorithm of generation of seeds:
 *
 * - RND(x) function is defined as RND(SplitMixHash(x) ^ RDSEED) where
 *   RDSEED is RDSEED instruction, hardware RNG in CPU.
 * - Internal counter CTR is "Weyl sequence" from SplitMix.
 * - Output function is XXTEA(RND(CTR)) where XXTEA is block cipher with
 *   64-bit block and 128-bit key.
 * - XXTEA keys are made as RND(time(NULL)) and RND(~time(NULL)) ^ RND(RDTSC)
 *
 * XORing with RDSEED can be excluded if CPU doesn't support that instruction.
 * RDTSC can be excluded if CPU doesn't support this instruction. Even if all
 * hardware sources of entropy except time are excluded --- it will return
 * high-quality pseudorandom seeds.
 *
 * Usage of XXTEA over RDSEED is also intended to exclude any biases.
 */
class Entropy
{
    std::mutex mut;
    uint32_t key[4];
    uint64_t state;
    
    uint64_t MixHash(uint64_t z) const;
    uint64_t Xxtea(const uint64_t inp) const;
    uint64_t NextState();
    uint64_t MixRdSeed(const uint64_t x) const;

public:
    std::vector<uint64_t> seeds_log;

    Entropy();
    bool XxteaTest();
    uint64_t Seed64();
    inline size_t GetNSeeds() const { return seeds_log.size(); }
    static uint64_t CpuClock();
};

} // namespace testu01_threads

#endif
