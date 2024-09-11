#include "testu01th/generators.h"

using namespace testu01_threads;

/////////////////////////////////////////////////
///// MT19937Generator class implementation /////
/////////////////////////////////////////////////

MT19937Generator::MT19937Generator() : UniformGenerator("MT19937") {}

double MT19937Generator::GetU01()
{
    return gen() * INV32;
}

uint32_t MT19937Generator::GetBits32()
{
    return gen();
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
    x = static_cast<uint32_t>(((static_cast<uint64_t>(x)) * a + 0) % m);
    return (double) x / (UINT_MAX);
}

uint32_t LcgGenerator::GetBits32()
{
    x = static_cast<uint32_t>((static_cast<uint64_t>(x) *  a + 0) % m);
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
    return (double) x / m_2_pow_59;
}

uint32_t LcgGenerator59::GetBits32()
{
    x = (a * x) & mask_mod;
    return static_cast<uint32_t>(x >> 27);
}


////////////////////////////////////////////////
///// KISS93Generator class implementation /////
////////////////////////////////////////////////

KISS93Generator::KISS93Generator(uint32_t s1, uint32_t s2, uint32_t s3)
: UniformGenerator("KISS93"), S1(s1), S2(s2), S3(s3) {}

double KISS93Generator::GetU01()
{
    return GetBits32() * INV32;
}

uint32_t KISS93Generator::GetBits32()
{
    S1 = 69069 * S1 + 23606797;
    uint32_t b = S2 ^ (S2 << 17);
    S2 = (b >> 15) ^ b;
    b = ((S3 << 18) ^ S3) & MASK31;
    S3 = (b >> 13) ^ b;
    return S1 + S2 + S3;
}
