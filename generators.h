#ifndef __GENERATORS_H
#define __GENERATORS_H
#include "testu01_mt.h"
#include <random>

/**
 * @brief A wrapper for Mersenne twister from the C++ standard library.
 * @details Fails the next tests from the Crush batter:
 * - 71 LinearComp, r = 0
 * - 72 LinearComp, r = 29
 * And from the BigCrush battery:
 * - 80 LinearComp, r = 0
 * - 81 LinearComp, r = 29
 */
class MT19937Generator : public UniformGenerator
{
    std::mt19937 gen;
    static constexpr double INV32 = 1.0 / (static_cast<uint64_t>(1) << 32);

public:
    MT19937Generator();
    double GetU01() override;
    uint32_t GetBits32() override;
};


/**
 * @brief A simple LCG that fails several tests in SmallCrush
 */
class LcgGenerator : public UniformGenerator
{
    uint32_t x;
    static constexpr uint64_t a = 397204094;
    static constexpr uint64_t m = 2147483647; // 2^31 - 1

public:
    LcgGenerator(int seed = 1);
    double GetU01() override;
    uint32_t GetBits32() override;
};


/**
 * @brief LCG(2^59, 13^13, 0) pseudorandom number generator that
 * passes almost all SmallCrush tests except BirthdaySpacings.
 */
class LcgGenerator59 : public UniformGenerator
{
    uint64_t x;
    static constexpr uint64_t mask_mod = (static_cast<uint64_t>(2) << 59) - 1;
    static constexpr uint64_t m_2_pow_59 = (static_cast<uint64_t>(2) << 59);
    static constexpr uint64_t a = 302875106592253; ///< 13^13

public:
    LcgGenerator59(int seed = 1);
    double GetU01() override;
    uint32_t GetBits32() override;
};


/**
 * @brief SplitMix PRNG that passes all three batteries, i.e. SmallCrush,
 * Crush and BigCrush. May be useful for initialization of other PRNG
 * with larger periods, e.g. lagged Fibbonaci PRNG.
 */
class SplitMixGenerator: public UniformGenerator
{
    uint64_t x;

public:
    SplitMixGenerator(uint64_t seed = 0) : UniformGenerator("SplitMix"), x(seed) {}

    inline uint64_t GetBits64()
    {
        constexpr uint64_t gamma = UINT64_C(0x9E3779B97F4A7C15);
        uint64_t z = (x += gamma);
        z = (z ^ (z >> 30)) * UINT64_C(0xBF58476D1CE4E5B9);
        z = (z ^ (z >> 27)) * UINT64_C(0x94D049BB133111EB);
        return z ^ (z >> 31);
    }

    double GetU01() override { return uint64_to_udouble(GetBits64()); }
    uint32_t GetBits32() override { return static_cast<uint32_t>(GetBits64() >> 32); }
};



/**
 * @brief KISS93 pseudorandom number generator. It passes SmallCrush
 * but fails the next tests:
 * - LinearComp (r = 29) test in the Crush battery.
 * - LinearComp (r = 29) test in the BigCrush battery.
 */
class KISS93Generator : public UniformGenerator
{
    uint32_t S1; ///< PRNG internal state.
    uint32_t S2; ///< PRNG internal state.
    uint32_t S3; ///< PRNG internal state.
    static constexpr uint32_t MASK31 = 0x7fffffffU; ///< Mask of 31 bits
    static constexpr double INV32 = 2.3283064365386963E-10; ///< 1 / 2^32

public:
    KISS93Generator(uint32_t s1 = 12345, uint32_t s2 = 6789, uint32_t s3 = 111213);
    double GetU01() override;
    uint32_t GetBits32() override;
};

/**
 * @brief Multiplicative lagged Fibbonaci PRNG.
 */
template<size_t lfib_a, size_t lfib_b>
class LFibMulGenerator : public UniformGenerator
{
    uint64_t U[lfib_a + 1];
    int i;
    int j;

    static std::string MakeGeneratorName()
    {
        return "LFIB( " + std::to_string(lfib_a) + ","
            + std::to_string(lfib_b) + ")";
    }

public:
    LFibMulGenerator(uint32_t seed = 0) : UniformGenerator(MakeGeneratorName())
    {
        SplitMixGenerator splitmix(seed);
        for (size_t k = 1; k <= lfib_a; k++) {
            U[k] = splitmix.GetBits64() | 0x1; // The lowest bit must be 1
        }
        i = lfib_a; j = lfib_b;
    }

    double GetU01() override { return uint64_to_udouble(GetBits64()); }
    uint32_t GetBits32() override { return static_cast<uint32_t>(GetBits64() >> 32); }
    inline uint64_t GetBits64()
    {
        uint64_t x = U[i] * U[j];
        U[i] = x;
	    if(--i == 0) i = lfib_a;
    	if(--j == 0) j = lfib_a;
        return x;
    }
};

#endif
