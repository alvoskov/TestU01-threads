#ifndef __GENERATORS_H
#define __GENERATORS_H
#include "testu01_mt.h"
#include <random>

class MT19937Generator : public UniformGenerator
{
    std::mt19937 gen;
    static constexpr double INV32 = 1.0 / (static_cast<uint64_t>(1) << 32);

public:
    MT19937Generator();
    double GetU01() override;
    uint32_t GetBits() override;
};


class LcgGenerator : public UniformGenerator
{
    uint32_t x;

public:
    LcgGenerator(int seed = 1);
    double GetU01() override;
    uint32_t GetBits() override;
};


//////////////////////////////////////////////////////////////////////

// 2^59, 13^13, 0
class LcgGenerator59 : public UniformGenerator
{
    uint64_t x;
    static constexpr uint64_t mask_mod = (static_cast<uint64_t>(2) << 59) - 1;
    static constexpr uint64_t m_2_pow_59 = (static_cast<uint64_t>(2) << 59);
    static constexpr uint64_t a = 302875106592253; ///< 13^13

public:
    LcgGenerator59(int seed = 1);
    double GetU01() override;
    uint32_t GetBits() override;
};


//////////////////////////////////////////////////////////////////////

class KISS93Generator : public UniformGenerator
{
    uint32_t S1;
    uint32_t S2;
    uint32_t S3;
    static constexpr uint32_t MASK31 = 0x7fffffffU; ///< Mask of 31 bits
    static constexpr double INV32 = 2.3283064365386963E-10; ///< 1 / 2^32

public:
    KISS93Generator(uint32_t s1 = 12345, uint32_t s2 = 6789, uint32_t s3 = 111213);
    double GetU01() override;
    uint32_t GetBits() override;
};


#endif
