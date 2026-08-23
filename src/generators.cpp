#include "testu01th/generators.h"

using namespace testu01_threads;

/////////////////////////////////////////////////
///// MT19937Generator class implementation /////
/////////////////////////////////////////////////

MT19937Generator::MT19937Generator() : UniformGenerator("MT19937") {}

double MT19937Generator::GetU01()
{
    return static_cast<double>(gen()) * INV32;
}

std::uint32_t MT19937Generator::GetBits32()
{
    return static_cast<std::uint32_t>(gen());
}


/////////////////////////////////////////////
///// LcgGenerator class implementation /////
/////////////////////////////////////////////

LcgGenerator::LcgGenerator(int seed) : UniformGenerator("LCG")
{
    x = seed;
}

double LcgGenerator::GetU01()
{                                                
    constexpr double xdbl_norm = 1.0 / static_cast<double>(1ULL << 32);
    x = static_cast<std::uint32_t>(((static_cast<std::uint64_t>(x)) * a + 0) % m);
    return static_cast<double>(x) / xdbl_norm;
}

uint32_t LcgGenerator::GetBits32()
{
    x = static_cast<std::uint32_t>((static_cast<std::uint64_t>(x) * a + 0) % m);
    return x;
}

///////////////////////////////////////////////
///// LcgGenerator59 class implementation /////
///////////////////////////////////////////////

LcgGenerator59::LcgGenerator59(int seed) : UniformGenerator("LCG59")
{
    x = seed;
}

double LcgGenerator59::GetU01()
{
    x = (a * x) & mask_mod;
    return static_cast<double>(x) / m_2_pow_59;
}

uint32_t LcgGenerator59::GetBits32()
{
    x = (a * x) & mask_mod;
    return static_cast<std::uint32_t>(x >> 27);
}


////////////////////////////////////////////////
///// KISS93Generator class implementation /////
////////////////////////////////////////////////

KISS93Generator::KISS93Generator(std::uint32_t s1, std::uint32_t s2, std::uint32_t s3)
: UniformGenerator{"KISS93"},
  lcg{s1},
  xs1{(s2 != 0) ? s2 : 12345},
  xs2{(s3 != 0) ? s3 : 67890}
{
}

double KISS93Generator::GetU01()
{
    return GetBits32() * INV32;
}

uint32_t KISS93Generator::GetBits32()
{
    // LCG part
    lcg = 69069U * lcg + 23606797U;
    // LFSR 1 part
    xs1 ^= xs1 << 17;
    xs1 ^= xs1 >> 15;
    // LFSR 2 part
    xs2 = ((xs2 << 18) ^ xs2) & MASK31;
    xs2 ^= xs2 >> 13;
    return lcg + xs1 + xs2;
}
