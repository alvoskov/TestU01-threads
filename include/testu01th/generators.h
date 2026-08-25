/**
 * @file generators.h
 * @brief Some pseudorandom number generators examples designed
 * for TestU01-threads.
 * @copyright
 * (c) 2024-2026 Alexey L. Voskov, Lomonosov Moscow State University.
 * alvoskov@gmail.com
 *
 * All rights reserved.
 *
 * This software is provided under the Apache 2 License.
 *
 * In scientific publications which used this software, a reference to it
 * would be appreciated.
 */
#ifndef __TESTU01_GENERATORS_H
#define __TESTU01_GENERATORS_H
#include "testu01_mt.h"
#include <random>
#include <sstream>

namespace testu01_threads {

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
    std::uint32_t x;
    static constexpr std::uint64_t a{397204094};
    static constexpr std::uint64_t m{2147483647}; // 2^31 - 1

public:
    LcgGenerator(int seed = 1);
    double GetU01() override;
    std::uint32_t GetBits32() override;
};


/**
 * @brief LCG(2^59, 13^13, 0) pseudorandom number generator that
 * passes almost all SmallCrush tests except BirthdaySpacings.
 */
class LcgGenerator59 : public UniformGenerator
{
    std::uint64_t x;
    static constexpr std::uint64_t mask_mod{(static_cast<std::uint64_t>(2) << 59) - 1};
    static constexpr std::uint64_t m_2_pow_59{(static_cast<std::uint64_t>(2) << 59)};
    static constexpr std::uint64_t a{302875106592253}; ///< 13^13

public:
    LcgGenerator59(int seed = 1);
    double GetU01() override;
    std::uint32_t GetBits32() override;
};


/**
 * @brief SplitMix PRNG that passes all three batteries, i.e. SmallCrush,
 * Crush and BigCrush. May be useful for initialization of other PRNG
 * with larger periods, e.g. lagged Fibbonaci PRNG.
 */
class SplitMixGenerator: public UniformGenerator
{
    std::uint64_t x;

public:
    SplitMixGenerator(std::uint64_t seed = 0)
    : UniformGenerator{"SplitMix"}, x{seed} {}

    inline uint64_t GetBits64()
    {
        constexpr std::uint64_t gamma{0x9E3779B97F4A7C15};
        std::uint64_t z = (x += gamma);
        z = (z ^ (z >> 30)) * UINT64_C(0xBF58476D1CE4E5B9);
        z = (z ^ (z >> 27)) * UINT64_C(0x94D049BB133111EB);
        return z ^ (z >> 31);
    }

    double GetU01() override { return uint64_to_udouble(GetBits64()); }
    std::uint32_t GetBits32() override { return static_cast<std::uint32_t>(GetBits64() >> 32); }
};



/**
 * @brief KISS93 pseudorandom number generator. It passes SmallCrush
 * but fails the next tests:
 * - LinearComp (r = 29) test in the Crush battery.
 * - LinearComp (r = 29) test in the BigCrush battery.
 */
class KISS93Generator : public UniformGenerator
{
    std::uint32_t lcg; ///< LCG internal staet.
    std::uint32_t xs1; ///< LFSR 1 internal state.
    std::uint32_t xs2; ///< LFSR 2 internal state.
    static constexpr uint32_t MASK31{0x7fffffffU}; ///< Mask of 31 bits

public:
    KISS93Generator(uint32_t s1 = 12345, uint32_t s2 = 6789, uint32_t s3 = 111213);
    double GetU01() override;
    std::uint32_t GetBits32() override;
};

/**
 * @brief Multiplicative lagged Fibbonaci PRNG.
 */
template<size_t lfib_a, size_t lfib_b>
class LFibMulGenerator : public UniformGenerator
{
    std::uint64_t U[lfib_a + 1];
    int i;
    int j;

    static std::string MakeGeneratorName()
    {
        return "LFIB( " + std::to_string(lfib_a) + ","
            + std::to_string(lfib_b) + ")";
    }

public:
    LFibMulGenerator(std::uint32_t seed = 0)
    : UniformGenerator{MakeGeneratorName()}, i{lfib_a}, j{lfib_b}
    {
        SplitMixGenerator splitmix{seed};
        for (size_t k = 1; k <= lfib_a; k++) {
            U[k] = splitmix.GetBits64() | 0x1; // The lowest bit must be 1
        }
    }

    double GetU01() override { return uint64_to_udouble(GetBits64()); }
    std::uint32_t GetBits32() override { return static_cast<std::uint32_t>(GetBits64() >> 32); }
    inline std::uint64_t GetBits64()
    {
        const std::uint64_t x{U[i] * U[j]};
        U[i] = x;
	    if (--i == 0) i = lfib_a;
    	if (--j == 0) j = lfib_a;
        return x;
    }
};


/**
 * @brief xorwow generators family.
 * @details Recommended parameters:
 *
 * - `[a, b, c, weyl_inc] = [2, 1, 4, 362437]`: classical by G. Marsaglia
 * - `[a, b, c, weyi_inc] = [19, 3, 11, 0x9E3779B9]`: found by A.L. Voskov, give
 *   a better quality of lower bits.
 */
template<int a, int b, int c, std::uint32_t weyl_inc>
class XorwowGeneratorFamily : public UniformGenerator
{
    std::uint32_t x; ///< Xorshift register
    std::uint32_t y; ///< Xorshift register
    std::uint32_t z; ///< Xorshift register
    std::uint32_t w; ///< Xorshift register
    std::uint32_t v; ///< Xorshift register
    std::uint32_t d; ///< "Weyl sequence" counter

    static std::string MakeGeneratorName()
    {
        std::stringstream sout;
        sout << "xorwow[" << a << ", " << b << ", " << c << "; " << weyl_inc << "]";
        return sout.str();
    }

public:
    XorwowGeneratorFamily(const std::array<std::uint32_t, 6>& seeds = {123456, 654321, 11111, 44444, 55555, 66666})
    : UniformGenerator{MakeGeneratorName()},
      x{seeds[0]}, y{seeds[1]}, z{seeds[2]}, w{seeds[3]}, v{seeds[4]}, d{seeds[5]}
    {
        if (x == 0 && y == 0) {
            x = 0x12345678;
            y = 0x87654321;
        }
    }

    double GetU01() override
    {
        return uint32_to_udouble(GetBits32());
    }

    std::uint32_t GetBits32() override
    {
        const std::uint32_t t = (x ^ (x >> a));
        x = y;
        y = z;
        z = w;
        w = v;
        v = (v ^ (v << c)) ^ (t ^ (t << b));
        return (d += weyl_inc) + v;
    }
};


} // namespace testu01_threads

#endif // __TESTU01_GENERATORS_H
