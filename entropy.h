/**
 * @file entropy.h
 * @brief Generator of random seeds for other PRNGs.
 */
#ifndef __ENTROPY_H
#define __ENTROPY_H
#include <stdint.h>
#include <vector>
#include <thread>
#include <mutex>
#include <iostream>


enum EntropyMethod
{
    ENTROPY_TIME, ///< Seed is from system time.
    ENTROPY_RDTSC, ///< Seed is from from RDTSC instruction and system time.
    ENTROPY_RDSEED ///< Hardware RNG in CPU is used.
};

/**
 * @brief 
 */
class SeedRecord
{
public:
    uint64_t state;
    uintptr_t thread_id;
};

/**
 * @brief Generates seeds for PRNGs using some entropy sources.
 * DON'T USE FOR CRYPTOGRAPHY! IT IS DESIGNED ONLY FOR STATISTCAL TESTS!
 */
class Entropy
{
    mutable std::mutex mut;
    uint32_t key[4];
    uint64_t state;
    
    uint64_t SplitMixHash(uint64_t z) const;
    uint64_t Xxtea(const uint64_t inp) const;
    uint64_t NextState();
    uint64_t MixRdSeed(const uint64_t x) const;

public:
    std::vector<SeedRecord> seeds_log;
    Entropy();
    bool XxteaTest();
    uint64_t Seed64();
    
};

#endif
