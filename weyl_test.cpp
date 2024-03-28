
#include "testu01_mt.h"
#include "bigcrush.h"
#include "crush.h"

#include <stdlib.h>
#include <stdio.h>

#include <string>
#include <cstring>
#include <functional>
#include <memory>
#include <vector>
#include <thread>
#include <mutex>
#include <algorithm>
#include <iostream>

//////////////////////////////////////////////////////////////////////


class LcgGenerator : public UniformGenerator
{
    uint32_t x;

public:
    LcgGenerator(int seed = 1) : UniformGenerator("LCG")
    {
        x = seed;
    }

    double GetU01() override
    {
        x = (((uint64_t) x) *  397204094 + 0) % 2147483647;
        return (double) x / (UINT_MAX);
    }

    uint32_t GetBits() override
    {
        x = (((uint64_t) x) *  397204094 + 0) % 2147483647;
        return x;
    }
};


// 2^59, 13^13, 0
class LcgGenerator59 : public UniformGenerator
{
    uint64_t x;
    static constexpr uint64_t mask_mod = (static_cast<uint64_t>(2) << 59) - 1;
    static constexpr uint64_t m_2_pow_59 = (static_cast<uint64_t>(2) << 59);
    static constexpr uint64_t a = 302875106592253; ///< 13^13

public:
    LcgGenerator59(int seed = 1) : UniformGenerator("LCG59")
    {
        x = seed;
    }

    double GetU01() override
    {
        x = (a * x) & mask_mod;
        return (double) x / m_2_pow_59;
    }

    uint32_t GetBits() override
    {
        x = (a * x) & mask_mod;
        return static_cast<uint32_t>(x >> 27);
    }
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
    KISS93Generator(uint32_t s1 = 12345, uint32_t s2 = 6789, uint32_t s3 = 111213)
    : UniformGenerator("KISS93"), S1(s1), S2(s2), S3(s3) {}

    double GetU01() override
    {
        return GetBits() * INV32;
    }

    uint32_t GetBits() override
    {
        S1 = 69069 * S1 + 23606797;
        uint32_t b = S2 ^ (S2 << 17);
        S2 = (b >> 15) ^ b;
        b = ((S3 << 18) ^ S3) & MASK31;
        S3 = (b >> 13) ^ b;
        return S1 + S2 + S3;
    }
};

//////////////////////////////////////////////////////////////////////

template<size_t lfib_a, size_t lfib_b>
class LFibGenerator : public UniformGenerator
{
    static constexpr double c = 5566755282872655.0 / 9007199254740992.0; ///< shift
    static constexpr double r = 9007199254740881.0 / 9007199254740992.0; ///< base (prime)

    double z;
    double w;
    double U[lfib_a + 1];
    int i;
    int j;

    inline double amb_mod_r(double a, double b)
    {
        double x = a - b;
        return (x >= 0.0) ? x : (x + r);
    }

public:
    LFibGenerator(uint32_t seed) : UniformGenerator("LFIB")
    {    
        z = (double) seed / UINT_MAX;
        w = z;
        double w2 = z, w3 = z;
        for (size_t k = 1; k <= lfib_a; k++) {
            z = amb_mod_r(z, c);
            w = amb_mod_r(w, z);
            w2 = amb_mod_r(w2, w);
            w3 = amb_mod_r(w3, w2);
            U[k] = w3;
        }
        i = lfib_a; j = lfib_b;
    }

    double GetU01()
    {
        double x = U[i] - U[j];
        if (x < 0.0) x += 1.0;
        U[i] = x;
	    if(--i == 0) i = lfib_a;
    	if(--j == 0) j = lfib_a;
        z = amb_mod_r(z, c);
        w = amb_mod_r(w, z);
        x -= w;
        return (x < 0.0) ? (x + 1.0) : x;
    }

    uint32_t GetBits()
    {
	    return (uint32_t) (GetU01() * unif01_NORM32);
    }
};

/////////////////////////////////////////////////////////////////////////////////


void mt_bat_SmallCrush (std::function<std::shared_ptr<UniformGenerator>()> create_gen)
{
   const int r = 0;
   int j2 = 0;
   printf ("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n"
      "                 Starting SmallCrush\n"
      "                 Version: %s\n"
      "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n\n\n",
      PACKAGE_STRING);

    std::vector<TestDescr> tests;

    tests.emplace_back(++j2, "BirthdaySpacings", // 1
        smarsa_BirthdaySpacings_cb(1, 5 * MILLION, r, 1073741824, 2, 1));

    tests.emplace_back(++j2, "Collision", [] (TestDescr &td, BatteryIO &io) { // 2
        auto *res3 = sknuth_CreateRes2 ();
        sknuth_Collision(io.Gen(), res3, 1, 5 * MILLION, 0, 65536, 2);
        io.Add(td.GetId(), td.GetName(), res3->Pois->pVal2);
        sknuth_DeleteRes2(res3);
    });

    tests.emplace_back(++j2, "Gap", // 3
        sknuth_Gap_cb(1, MILLION / 5, 22, 0.0, .00390625));

    tests.emplace_back(++j2, "SimpPoker", // 4
        sknuth_SimpPoker_cb(1, 2 * MILLION / 5, 24, 64, 64));

    tests.emplace_back(++j2, "CouponCollector", // 5
        sknuth_CouponCollector_cb(1, MILLION / 2, 26, 16));

    tests.emplace_back(++j2, "MaxOft", // 6-7
        sknuth_MaxOft_cb(1, 2 * MILLION, 0, MILLION / 10, 6));

    tests.emplace_back(++j2, "WeightDistrib", [] (TestDescr &td, BatteryIO &io) { // 8
        auto *res2 = sres_CreateChi2 ();
        svaria_WeightDistrib (io.Gen(), res2, 1, MILLION / 5, 27, 256, 0.0, 0.125);
        io.Add(td.GetId(), td.GetName(), res2->pVal2[gofw_Mean]);
        sres_DeleteChi2(res2);
    });

    tests.emplace_back(++j2, "MatrixRank", // 9
        smarsa_MatrixRank_cb(1, 20 * THOUSAND, 20, 10, 60, 60));

    tests.emplace_back(++j2, "HammingIndep",
        sstring_HammingIndep_cb(1, MILLION/2, 20, 10, 300, 0)); // 10

    tests.emplace_back(++j2, "RandomWalk", // 11-15
        smarsa_RandomWalk1_cb(1, MILLION, r, 30, 150, 150, ""));

    //------------------------------------------------------------------------------------
    run_tests(tests, create_gen, "SmallCrush(mt)");
}


////////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[]) 
{
    if (argc != 3) {
        printf("Usage: mt_test battery generator\n");
        printf("battery: battery name (SmallCrush, Crush, BigCrush)\n");
        printf("generator: PRNG name (LCG, LCG59, KISS93)\n");
        return 0;
    }

    std::string battery(argv[1]), generator(argv[2]);

    // LCG59 generator: fails BirthdaySpacings test in SmallCrush
    // KISS93Generator : fails LinearComp (r = 29) tests in Crush battery
    std::function<std::shared_ptr<UniformGenerator>()> create_gen;
    if (generator == "LCG") {
        create_gen = [] () -> std::shared_ptr<UniformGenerator> {
            return std::shared_ptr<UniformGenerator>(new LcgGenerator());
        };
    } else if (generator == "LCG59") {
        create_gen = [] () -> std::shared_ptr<UniformGenerator> {
            return std::shared_ptr<UniformGenerator>(new LcgGenerator59());
        };
    } else if (generator == "KISS93") {
        create_gen = [] () -> std::shared_ptr<UniformGenerator> {
            return std::shared_ptr<UniformGenerator>(new KISS93Generator());
        };
    } else {
        std::cerr << "Unknown generator " << generator << std::endl;
        return 1;
    }

    if (battery == "SmallCrush") {
        mt_bat_SmallCrush(create_gen);
    } else if (battery == "Crush") {
        mt_bat_Crush(create_gen);
    } else if (battery == "BigCrush") {
        mt_bat_BigCrush(create_gen);
    } else {
        std::cerr << "Unknown battery " << battery << std::endl;
    }

    // (10, 7), (17, 5), (55, 24)
    //LFibGenerator<17, 5> lcgobj(123);
//    LcgGenerator lcgobj;
    //bbattery_SmallCrush(lcgobj.GetPtr());
/*
    custom_Crush([] () -> std::shared_ptr<UniformGenerator> {
        return std::shared_ptr<UniformGenerator>(new LFibGenerator<17, 5>(123));
    });
*/

    return 0;
}
