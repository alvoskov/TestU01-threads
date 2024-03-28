
#include "testu01_mt.h"

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


static void run_tests(std::vector<TestDescr> &tests,
    std::function<std::shared_ptr<UniformGenerator>()> create_gen,
    const std::string &battery_name)
{
    chrono_Chrono *timer = chrono_Create();

    size_t nthreads = std::thread::hardware_concurrency();
    size_t ntests = tests.size();
    while (nthreads > ntests)
        nthreads /= 2;
    std::vector<std::vector<TestDescr>> threads_tasks(nthreads);
    std::vector<BatteryIO> threads_bats;
    for (size_t i = 0; i < nthreads; i++) {
        threads_bats.emplace_back(create_gen());
    }
    size_t th_id = 0;
    for (auto &t : tests) {
        threads_tasks[th_id].push_back(t);
        if (++th_id == nthreads)
            th_id = 0;
    }
    auto thread_func = [] (std::vector<TestDescr> &tests, BatteryIO &io) -> void {
        for (auto &t : tests) {
            t.Run(io);
        }        
    };

    std::vector<std::thread> threads;
    for (size_t i = 0; i < nthreads; i++) {
        threads.emplace_back(thread_func, std::ref(threads_tasks[i]), std::ref(threads_bats[i]));
    }

    for (auto &th : threads) {
        th.join();
    }

    auto gen = create_gen();
    BatteryIO io(gen);
    for (auto &bat : threads_bats) {
        io.Add(bat);
    }
    io.WriteReport(battery_name.c_str(), gen.get()->GetName().c_str(), timer);
    chrono_Delete(timer);
}


void custom_SmallCrush (std::function<std::shared_ptr<UniformGenerator>()> create_gen)
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

    tests.emplace_back(++j2, "SimpPoker", [] (TestDescr &td, BatteryIO &io) { // 4
        auto *res2 = sres_CreateChi2 ();
        sknuth_SimpPoker(io.Gen(), res2, 1, 2 * MILLION / 5, 24, 64, 64);
        io.Add(td.GetId(), td.GetName(), res2->pVal2[gofw_Mean]);
        sres_DeleteChi2(res2);
    });

    tests.emplace_back(++j2, "CouponCollector", [] (TestDescr &td, BatteryIO &io) { // 5
        auto *res2 = sres_CreateChi2 ();
        sknuth_CouponCollector (io.Gen(), res2, 1, MILLION / 2, 26, 16);
        io.Add(td.GetId(), td.GetName(), res2->pVal2[gofw_Mean]);
        sres_DeleteChi2(res2);
    });

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


/////////////////////////////////////////////////////////////////////////////////

/*
 * A battery of stringent statistical tests for Random Number Generators
 * used in simulation.
 * Rep[i] gives the number of times that test i will be done. The default
 * values are Rep[i] = 1 for all i.
 */
void custom_Crush (std::function<std::shared_ptr<UniformGenerator>()> create_gen)
{
    const int s = 30;
    const int r = 0;
    int j2 = 0;
    printf ("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n"
        "                 Starting Crush\n"
        "                 Version: %s\n"
        "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n\n\n",
        PACKAGE_STRING);

    std::vector<TestDescr> tests;
    // SerialOver tests
    tests.emplace_back(++j2, "SerialOver, t = 2", [] (TestDescr &td, BatteryIO &io) {
        sres_Basic *res = sres_CreateBasic();
        smarsa_SerialOver (io.Gen(), res, 1, 500 * MILLION, 0, 4096, 2);
        io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_Mean]);
        sres_DeleteBasic (res);
    });

    tests.emplace_back(++j2, "SerialOver, t = 4", [] (TestDescr &td, BatteryIO &io) {
        sres_Basic *res = sres_CreateBasic();
        smarsa_SerialOver (io.Gen(), res, 1, 300 * MILLION, 0, 64, 4);
        io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_Mean]);
        sres_DeleteBasic (res);
    });

    // CollisionOver tests
    tests.emplace_back(++j2, "CollisionOver, t = 2",
        smarsa_CollisionOver_cb(10, 10 * MILLION, 0, 1024 * 1024, 2));

    tests.emplace_back(++j2, "CollisionOver, t = 2",
        smarsa_CollisionOver_cb(10, 10 * MILLION, 10, 1024 * 1024, 2));

    tests.emplace_back(++j2, "CollisionOver, t = 4",
        smarsa_CollisionOver_cb(10, 10 * MILLION, 0, 1024, 4));

    tests.emplace_back(++j2, "CollisionOver, t = 4",
        smarsa_CollisionOver_cb(10, 10 * MILLION, 20, 1024, 4));

    tests.emplace_back(++j2, "CollisionOver, t = 8",
        smarsa_CollisionOver_cb(10, 10 * MILLION, 0, 32, 8));

    tests.emplace_back(++j2, "CollisionOver, t = 8",
        smarsa_CollisionOver_cb(10, 10 * MILLION, 25, 32, 8));

    tests.emplace_back(++j2, "CollisionOver, t = 20",
        smarsa_CollisionOver_cb(10, 10 * MILLION, 0, 4, 20));

    tests.emplace_back(++j2, "CollisionOver, t = 20",
        smarsa_CollisionOver_cb(10, 10 * MILLION, 28, 4, 20));

    // BirthdaySpacings tests
#if LONG_MAX <= 2147483647L
    tests.emplace_back(++j2, "BirthdaySpacings, t = 2",
        smarsa_BirthdaySpacings_cb(10, 10 * MILLION, 0, 1073741824L, 2, 1));
#else
    tests.emplace_back(++j2, "BirthdaySpacings, t = 2",
        smarsa_BirthdaySpacings_cb(5, 20 * MILLION, 0, 2*1073741824L, 2, 1));
#endif

    tests.emplace_back(++j2, "BirthdaySpacings, t = 3",
        smarsa_BirthdaySpacings_cb(5, 20 * MILLION, 0, 2097152, 3, 1));

    tests.emplace_back(++j2, "BirthdaySpacings, t = 4",
        smarsa_BirthdaySpacings_cb(5, 20 * MILLION, 0, 65536, 4, 1));

    tests.emplace_back(++j2, "BirthdaySpacings, t = 7",
        smarsa_BirthdaySpacings_cb(3, 20 * MILLION, 0, 512, 7, 1));

    tests.emplace_back(++j2, "BirthdaySpacings, t = 7",
        smarsa_BirthdaySpacings_cb(3, 20 * MILLION, 7, 512, 7, 1));

    tests.emplace_back(++j2, "BirthdaySpacings, t = 8",
        smarsa_BirthdaySpacings_cb(3, 20 * MILLION, 14, 256, 8, 1));

    tests.emplace_back(++j2, "BirthdaySpacings, t = 8",
        smarsa_BirthdaySpacings_cb(3, 20 * MILLION, 22, 256, 8, 1));

    // ClosePairs tests
/*
   {
      lebool flag = snpair_mNP2S_Flag;
      snpair_Res *res;
      res = snpair_CreateRes ();

      snpair_mNP2S_Flag = FALSE;
      ++j2;
      for (i = 0; i < Rep[j2]; ++i) {
         snpair_ClosePairs (gen, res, 10, 2 * MILLION, 0, 2, 0, 30);
         GetPVal_CPairs (10, res, &j, ", t = 2", j2);
      }

      snpair_mNP2S_Flag = TRUE;
      ++j2;
      for (i = 0; i < Rep[j2]; ++i) {
         snpair_ClosePairs (gen, res, 10, 2 * MILLION, 0, 3, 0, 30);
         GetPVal_CPairs (10, res, &j, ", t = 3", j2);
      }

      ++j2;
      for (i = 0; i < Rep[j2]; ++i) {
         snpair_ClosePairs (gen, res, 5, 2 * MILLION, 0, 7, 0, 30);
         GetPVal_CPairs (10, res, &j, ", t = 7", j2);
      }

      ++j2;
      for (i = 0; i < Rep[j2]; ++i) {
         snpair_ClosePairsBitMatch (gen, res, 4, 4 * MILLION, 0, 2);
         bbattery_pVal[++j] = res->pVal[snpair_BM];
         TestNumber[j] = j2;
         strcpy (bbattery_TestNames[j], "ClosePairsBitMatch, t = 2");
      }

      ++j2;
      for (i = 0; i < Rep[j2]; ++i) {
         snpair_ClosePairsBitMatch (gen, res, 2, 4 * MILLION, 0, 4);
         bbattery_pVal[++j] = res->pVal[snpair_BM];
         TestNumber[j] = j2;
         strcpy (bbattery_TestNames[j], "ClosePairsBitMatch, t = 4");
      }
      snpair_DeleteRes (res);
      snpair_mNP2S_Flag = flag;
   }
*/
    // SimpPoker tests
    tests.emplace_back(++j2, "SimpPoker, d = 16", [] (TestDescr &td, BatteryIO &io) {
        sres_Chi2 *res = sres_CreateChi2();
        sknuth_SimpPoker(io.Gen(), res, 1, 40 * MILLION, 0, 16, 16);
        io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_Mean]);
        sres_DeleteChi2(res);
    });

    tests.emplace_back(++j2, "SimpPoker, d = 16", [] (TestDescr &td, BatteryIO &io) {
        sres_Chi2 *res = sres_CreateChi2();
        sknuth_SimpPoker(io.Gen(), res, 1, 40 * MILLION, 26, 16, 16);
        io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_Mean]);
        sres_DeleteChi2(res);
    });

    tests.emplace_back(++j2, "SimpPoker, d = 64", [] (TestDescr &td, BatteryIO &io) {
        sres_Chi2 *res = sres_CreateChi2();
        sknuth_SimpPoker(io.Gen(), res, 1, 10 * MILLION, 0, 64, 64);
        io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_Mean]);
        sres_DeleteChi2(res);
    });

    tests.emplace_back(++j2, "SimpPoker, d = 64", [] (TestDescr &td, BatteryIO &io) {
        sres_Chi2 *res = sres_CreateChi2();
        sknuth_SimpPoker(io.Gen(), res, 1, 10 * MILLION, 24, 64, 64);
        io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_Mean]);
        sres_DeleteChi2(res);
    });

    // CouponCollector tests
    tests.emplace_back(++j2, "CouponCollector, d = 4", [] (TestDescr &td, BatteryIO &io) {
        sres_Chi2 *res = sres_CreateChi2();
        sknuth_CouponCollector (io.Gen(), res, 1, 40 * MILLION, 0, 4);
        io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_Mean]);
        sres_DeleteChi2(res);
    });

    tests.emplace_back(++j2, "CouponCollector, d = 4", [] (TestDescr &td, BatteryIO &io) {
        sres_Chi2 *res = sres_CreateChi2();
        sknuth_CouponCollector (io.Gen(), res, 1, 40 * MILLION, 28, 4);
        io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_Mean]);
        sres_DeleteChi2(res);
    });

    tests.emplace_back(++j2, "CouponCollector, d = 16", [] (TestDescr &td, BatteryIO &io) {
        sres_Chi2 *res = sres_CreateChi2();
        sknuth_CouponCollector (io.Gen(), res, 1, 10 * MILLION, 0, 16);
        io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_Mean]);
        sres_DeleteChi2(res);
    });

    tests.emplace_back(++j2, "CouponCollector, d = 16", [] (TestDescr &td, BatteryIO &io) {
        sres_Chi2 *res = sres_CreateChi2();
        sknuth_CouponCollector (io.Gen(), res, 1, 10 * MILLION, 26, 16);
        io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_Mean]);
        sres_DeleteChi2(res);
    });


    // Gap tests
    tests.emplace_back(++j2, "Gap, r = 0",
        sknuth_Gap_cb(1, 100 * MILLION, 0, 0.0, 0.125));

    tests.emplace_back(++j2, "Gap, r = 27",
        sknuth_Gap_cb(1, 100 * MILLION, 27, 0.0, 0.125));

    tests.emplace_back(++j2, "Gap, r = 0",
        sknuth_Gap_cb(1, 5 * MILLION, 0, 0.0, 1.0/256.0));

    tests.emplace_back(++j2, "Gap, r = 22",
        sknuth_Gap_cb(1, 5 * MILLION, 22, 0.0, 1.0/256.0));

    // Run of U01
    tests.emplace_back(++j2, "Run of U01, r = 0", [] (TestDescr &td, BatteryIO &io) {
        sres_Chi2 *res = sres_CreateChi2();
        sknuth_Run(io.Gen(), res, 1, 500 * MILLION, 0, TRUE);
        io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_Mean]);
        sres_DeleteChi2(res);
    });

    tests.emplace_back(++j2, "Run of U01, r = 15", [] (TestDescr &td, BatteryIO &io) {
        sres_Chi2 *res = sres_CreateChi2();
        sknuth_Run(io.Gen(), res, 1, 500 * MILLION, 15, FALSE);
        io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_Mean]);
        sres_DeleteChi2(res);
    });

    // Run of Permutation
    tests.emplace_back(++j2, "Permutation, r = 0", [] (TestDescr &td, BatteryIO &io) {
        sres_Chi2 *res = sres_CreateChi2();
        sknuth_Permutation(io.Gen(), res, 1, 50 * MILLION, 0, 10);
        io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_Mean]);
        sres_DeleteChi2(res);
    });

    tests.emplace_back(++j2, "Permutation, r = 15", [] (TestDescr &td, BatteryIO &io) {
        sres_Chi2 *res = sres_CreateChi2();
        sknuth_Permutation(io.Gen(), res, 1, 50 * MILLION, 0, 10);
        io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_Mean]);
        sres_DeleteChi2(res);
    });

    // CollisionPermut tests
    tests.emplace_back(++j2, "CollisionPermut, r = 0", [] (TestDescr &td, BatteryIO &io) {
        sknuth_Res2 *res = sknuth_CreateRes2 ();
        sknuth_CollisionPermut(io.Gen(), res, 5, 10 * MILLION, 0, 13);
        io.Add(td.GetId(), td.GetName(), res->Pois->pVal2);
        sknuth_DeleteRes2(res);
    });

    tests.emplace_back(++j2, "CollisionPermut, r = 15", [] (TestDescr &td, BatteryIO &io) {
        sknuth_Res2 *res = sknuth_CreateRes2 ();
        sknuth_CollisionPermut (io.Gen(), res, 5, 10 * MILLION, 15, 13);
        io.Add(td.GetId(), td.GetName(), res->Pois->pVal2);
        sknuth_DeleteRes2(res);
    });

    // MaxOft tests
    tests.emplace_back(++j2, "MaxOft, t = 5",
        sknuth_MaxOft_cb(10, 10 * MILLION, 0, MILLION / 10, 5));

    tests.emplace_back(++j2, "MaxOft, t = 10",
        sknuth_MaxOft_cb(5, 10 * MILLION, 0, MILLION / 10, 10));

    tests.emplace_back(++j2, "MaxOft, t = 20",
        sknuth_MaxOft_cb(1, 10 * MILLION, 0, MILLION / 10, 20));

    tests.emplace_back(++j2, "MaxOft, t = 30",
        sknuth_MaxOft_cb(1, 10 * MILLION, 0, MILLION / 10, 30));


    // SampleProd and other "Sample..." tests
    tests.emplace_back(++j2, "SampleProd, t = 10", [] (TestDescr &td, BatteryIO &io) {
        sres_Basic *res = sres_CreateBasic();
        svaria_SampleProd (io.Gen(), res, 1, 10 * MILLION, 0, 10);
        io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_Mean]);
        sres_DeleteBasic(res);
    });

    tests.emplace_back(++j2, "SampleProd, t = 30", [] (TestDescr &td, BatteryIO &io) {
        sres_Basic *res = sres_CreateBasic();
        svaria_SampleProd(io.Gen(), res, 1, 10 * MILLION, 0, 30);
        io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_Mean]);
        sres_DeleteBasic(res);
    });



    tests.emplace_back(++j2, "SampleMean", [] (TestDescr &td, BatteryIO &io) {
        sres_Basic *res = sres_CreateBasic();
        svaria_SampleMean(io.Gen(), res, 10*MILLION, 20, 0);
        io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_AD]);
        sres_DeleteBasic(res);
    });

    tests.emplace_back(++j2, "SampleCorr", [] (TestDescr &td, BatteryIO &io) {
        sres_Basic *res = sres_CreateBasic();
        svaria_SampleCorr(io.Gen(), res, 1, 500 * MILLION, 0, 1);
        io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_Mean]);
        sres_DeleteBasic(res);
    });

    tests.emplace_back(++j2, "AppearanceSpacings, r = 0", [] (TestDescr &td, BatteryIO &io) {
        sres_Basic *res = sres_CreateBasic();
        svaria_AppearanceSpacings(io.Gen(), res, 1, 10 * MILLION, 400 * MILLION,
            r, 30, 15);
        io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_Mean]);
        sres_DeleteBasic(res);
    });

    tests.emplace_back(++j2, "AppearanceSpacings, r = 20", [] (TestDescr &td, BatteryIO &io) {
        sres_Basic *res = sres_CreateBasic();
        svaria_AppearanceSpacings(io.Gen(), res, 1, 10 * MILLION, 100 * MILLION,
            20, 10, 15);
        io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_Mean]);
        sres_DeleteBasic(res);
    });

    // WeightDistrib tests
    tests.emplace_back(++j2, "WeightDistrib, r = 0",
        svaria_WeightDistrib_cb(1, 2 * MILLION, 0, 256, 0.0, 0.125));

    tests.emplace_back(++j2, "WeightDistrib, r = 8",
        svaria_WeightDistrib_cb(1, 2 * MILLION, 8, 256, 0.0, 0.125));

    tests.emplace_back(++j2, "WeightDistrib, r = 16",
        svaria_WeightDistrib_cb(1, 2 * MILLION, 16, 256, 0.0, 0.125));

    tests.emplace_back(++j2, "WeightDistrib, r = 24",
        svaria_WeightDistrib_cb(1, 2 * MILLION, 24, 256, 0.0, 0.125));

    // SumCollector test
    tests.emplace_back(++j2, "SumCollector", [] (TestDescr &td, BatteryIO &io) {
        sres_Chi2 *res = sres_CreateChi2();
        svaria_SumCollector(io.Gen(), res, 1, 20 * MILLION, 0, 10.0);
        io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_Mean]);
        sres_DeleteChi2(res);
    });    

    // MatrixRank tests
    tests.emplace_back(++j2, "MatrixRank, 60 x 60",
        smarsa_MatrixRank_cb(1, MILLION, r, s, 2 * s, 2 * s));

    tests.emplace_back(++j2, "MatrixRank, 60 x 60",
        smarsa_MatrixRank_cb(1, MILLION, 20, 10, 2 * s, 2 * s));

    tests.emplace_back(++j2, "MatrixRank, 300 x 300",
        smarsa_MatrixRank_cb(1, 50 * THOUSAND, r, s, 10 * s, 10 * s));

    tests.emplace_back(++j2, "MatrixRank, 300 x 300",
        smarsa_MatrixRank_cb(1, 50 * THOUSAND, 20, 10, 10 * s, 10 * s));

    tests.emplace_back(++j2, "MatrixRank, 1200 x 1200",
        smarsa_MatrixRank_cb(1, 2 * THOUSAND, r, s, 40 * s, 40 * s));

    tests.emplace_back(++j2, "MatrixRank, 1200 x 1200",
        smarsa_MatrixRank_cb(1, 2 * THOUSAND, 20, 10, 40 * s, 40 * s));

/*

      ++j2;
      for (i = 0; i < Rep[j2]; ++i) {
         smarsa_Savir2 (gen, res, 1, 20 * MILLION, 0, 1024*1024, 30);
         bbattery_pVal[++j] = res->pVal2[gofw_Mean];
         TestNumber[j] = j2;
         strcpy (bbattery_TestNames[j], "Savir2");
      }
      sres_DeleteChi2 (res);

      res2 = smarsa_CreateRes2 ();
      ++j2;
      for (i = 0; i < Rep[j2]; ++i) {
         smarsa_GCD (gen, res2, 1, 100 * MILLION, 0, 30);
         bbattery_pVal[++j] = res2->GCD->pVal2[gofw_Mean];
         TestNumber[j] = j2;
         strcpy (bbattery_TestNames[j], "GCD, r = 0");
      }

      ++j2;
      for (i = 0; i < Rep[j2]; ++i) {
         smarsa_GCD (gen, res2, 1, 40 * MILLION, 10, 20);
         bbattery_pVal[++j] = res2->GCD->pVal2[gofw_Mean];
         TestNumber[j] = j2;
         strcpy (bbattery_TestNames[j], "GCD, r = 10");
      }
      smarsa_DeleteRes2 (res2);
   }
*/


    tests.emplace_back(++j2, "RandomWalk",
        smarsa_RandomWalk1_cb(1, 50 * MILLION, r, s, 90, 90, " (L = 90)"));

    tests.emplace_back(++j2, "RandomWalk",
        smarsa_RandomWalk1_cb(1, 10 * MILLION, 20, 10, 90, 90, " (L = 90)"));

    tests.emplace_back(++j2, "RandomWalk",
        smarsa_RandomWalk1_cb(1, 5 * MILLION, r, s, 1000, 1000, " (L = 1000)"));

    tests.emplace_back(++j2, "RandomWalk",
        smarsa_RandomWalk1_cb(1, MILLION, 20, 10, 1000, 1000, " (L = 1000)"));

    tests.emplace_back(++j2, "RandomWalk",
        smarsa_RandomWalk1_cb(1, MILLION / 2, r, s, 10000, 10000, " (L = 10000)"));

    tests.emplace_back(++j2, "RandomWalk",
        smarsa_RandomWalk1_cb(1, MILLION / 10, 20, 10, 10000, 10000, " (L = 10000)"));

/*
   {
      scomp_Res *res;
      res = scomp_CreateRes ();
      ++j2;
      for (i = 0; i < Rep[j2]; ++i) {
         scomp_LinearComp (gen, res, 1, 120 * THOUSAND, r, 1);
         bbattery_pVal[++j] = res->JumpNum->pVal2[gofw_Mean];
         TestNumber[j] = j2;
         strcpy (bbattery_TestNames[j], "LinearComp, r = 0");
         bbattery_pVal[++j] = res->JumpSize->pVal2[gofw_Mean];
         TestNumber[j] = j2;
         strcpy (bbattery_TestNames[j], "LinearComp, r = 0");
      }

      ++j2;
      for (i = 0; i < Rep[j2]; ++i) {
         scomp_LinearComp (gen, res, 1, 120 * THOUSAND, 29, 1);
         bbattery_pVal[++j] = res->JumpNum->pVal2[gofw_Mean];
         TestNumber[j] = j2;
         strcpy (bbattery_TestNames[j], "LinearComp, r = 29");
         bbattery_pVal[++j] = res->JumpSize->pVal2[gofw_Mean];
         TestNumber[j] = j2;
         strcpy (bbattery_TestNames[j], "LinearComp, r = 29");
      }
      scomp_DeleteRes (res);
   }
   {
      sres_Basic *res;
      res = sres_CreateBasic ();
      ++j2;
      for (i = 0; i < Rep[j2]; ++i) {
         scomp_LempelZiv (gen, res, 10, 25, r, s);
         bbattery_pVal[++j] = res->pVal2[gofw_Sum];
         TestNumber[j] = j2;
         strcpy (bbattery_TestNames[j], "LempelZiv");
      }
      sres_DeleteBasic (res);
   }
   {
      sspectral_Res *res;
      res = sspectral_CreateRes ();

      ++j2;
      for (i = 0; i < Rep[j2]; ++i) {
         sspectral_Fourier3 (gen, res, 50 * THOUSAND, 14, r, s);
         bbattery_pVal[++j] = res->Bas->pVal2[gofw_AD];
         TestNumber[j] = j2;
         strcpy (bbattery_TestNames[j], "Fourier3, r = 0");
      }

      ++j2;
      for (i = 0; i < Rep[j2]; ++i) {
         sspectral_Fourier3 (gen, res, 50 * THOUSAND, 14, 20, 10);
         bbattery_pVal[++j] = res->Bas->pVal2[gofw_AD];
         TestNumber[j] = j2;
         strcpy (bbattery_TestNames[j], "Fourier3, r = 20");
      }
      sspectral_DeleteRes (res);
   }
   {
      sstring_Res2 *res;
      res = sstring_CreateRes2 ();
      ++j2;
      for (i = 0; i < Rep[j2]; ++i) {
         sstring_LongestHeadRun (gen, res, 1, 1000, r, s, 20 + 10 * MILLION);
         bbattery_pVal[++j] = res->Chi->pVal2[gofw_Mean];
         TestNumber[j] = j2;
         strcpy (bbattery_TestNames[j], "LongestHeadRun, r = 0");
         bbattery_pVal[++j] = res->Disc->pVal2;
         TestNumber[j] = j2;
         strcpy (bbattery_TestNames[j], "LongestHeadRun, r = 0");
      }

      ++j2;
      for (i = 0; i < Rep[j2]; ++i) {
         sstring_LongestHeadRun (gen, res, 1, 300, 20, 10, 20 + 10 * MILLION);
         bbattery_pVal[++j] = res->Chi->pVal2[gofw_Mean];
         TestNumber[j] = j2;
         strcpy (bbattery_TestNames[j], "LongestHeadRun, r = 20");
         bbattery_pVal[++j] = res->Disc->pVal2;
         TestNumber[j] = j2;
         strcpy (bbattery_TestNames[j], "LongestHeadRun, r = 20");
      }
      sstring_DeleteRes2 (res);
   }
   {
      sres_Chi2 *res;
      res = sres_CreateChi2 ();
      ++j2;
      for (i = 0; i < Rep[j2]; ++i) {
         sstring_PeriodsInStrings (gen, res, 1, 300 * MILLION, r, s);
         bbattery_pVal[++j] = res->pVal2[gofw_Mean];
         TestNumber[j] = j2;
         strcpy (bbattery_TestNames[j], "PeriodsInStrings, r = 0");
      }

      ++j2;
      for (i = 0; i < Rep[j2]; ++i) {
         sstring_PeriodsInStrings (gen, res, 1, 300 * MILLION, 15, 15);
         bbattery_pVal[++j] = res->pVal2[gofw_Mean];
         TestNumber[j] = j2;
         strcpy (bbattery_TestNames[j], "PeriodsInStrings, r = 15");
      }
      sres_DeleteChi2 (res);
   }
   {
      sres_Basic *res;
      res = sres_CreateBasic ();
      ++j2;
      for (i = 0; i < Rep[j2]; ++i) {
         sstring_HammingWeight2 (gen, res, 100, 100 * MILLION, r, s, MILLION);
         bbattery_pVal[++j] = res->pVal2[gofw_Sum];
         TestNumber[j] = j2;
         strcpy (bbattery_TestNames[j], "HammingWeight2, r = 0");
      }

      ++j2;
      for (i = 0; i < Rep[j2]; ++i) {
         sstring_HammingWeight2 (gen, res, 30, 100 * MILLION, 20, 10, MILLION);
         bbattery_pVal[++j] = res->pVal2[gofw_Sum];
         TestNumber[j] = j2;
         strcpy (bbattery_TestNames[j], "HammingWeight2, r = 20");
      }
      sres_DeleteBasic (res);
   }
*/
//   {
//      sstring_Res *res;
//      res = sstring_CreateRes ();
      /* sstring_HammingCorr will probably be removed: less sensitive than
         svaria_HammingIndep */
/*
      ++j2;
      for (i = 0; i < Rep[j2]; ++i) {
         sstring_HammingCorr (gen, res, 1, 500 * MILLION, r, s, s);
         bbattery_pVal[++j] = res->Bas->pVal2[gofw_Mean];
         TestNumber[j] = j2;
         strcpy (bbattery_TestNames[j], "HammingCorr, L = 30");
      }

      ++j2;
      for (i = 0; i < Rep[j2]; ++i) {
         sstring_HammingCorr (gen, res, 1, 50 * MILLION, r, s, 10 * s);
         bbattery_pVal[++j] = res->Bas->pVal2[gofw_Mean];
         TestNumber[j] = j2;
         strcpy (bbattery_TestNames[j], "HammingCorr, L = 300");
      }

      ++j2;
      for (i = 0; i < Rep[j2]; ++i) {
         sstring_HammingCorr (gen, res, 1, 10 * MILLION, r, s, 40 * s);
         bbattery_pVal[++j] = res->Bas->pVal2[gofw_Mean];
         TestNumber[j] = j2;
         strcpy (bbattery_TestNames[j], "HammingCorr, L = 1200");
      }
*/


    // HammingIndep tests
    tests.emplace_back(++j2, "HammingIndep, L = 30",
        sstring_HammingIndep_cb(1, 300 * MILLION, r, s, s, 0));

    tests.emplace_back(++j2, "HammingIndep, L = 30",
        sstring_HammingIndep_cb(1, 100 * MILLION, 20, 10, s, 0));

    tests.emplace_back(++j2, "HammingIndep, L = 300",
        sstring_HammingIndep_cb(1, 30 * MILLION, r, s, 10 * s, 0));

    tests.emplace_back(++j2, "HammingIndep, L = 300",
        sstring_HammingIndep_cb(1, 10 * MILLION, 20, 10, 10 * s, 0));

    tests.emplace_back(++j2, "HammingIndep, L = 1200",
        sstring_HammingIndep_cb(1, 10 * MILLION, r, s, 40 * s, 0));

    tests.emplace_back(++j2, "HammingIndep, L = 1200",
        sstring_HammingIndep_cb(1, MILLION, 20, 10, 40 * s, 0));

/*
   {
      sstring_Res3 *res;
      res = sstring_CreateRes3 ();
      ++j2;
      for (i = 0; i < Rep[j2]; ++i) {
         sstring_Run (gen, res, 1, 1 * BILLION, r, s);
         bbattery_pVal[++j] = res->NRuns->pVal2[gofw_Mean];
         TestNumber[j] = j2;
         strcpy (bbattery_TestNames[j], "Run of bits, r = 0");
         bbattery_pVal[++j] = res->NBits->pVal2[gofw_Mean];
         TestNumber[j] = j2;
         strcpy (bbattery_TestNames[j], "Run of bits, r = 0");
      }

      ++j2;
      for (i = 0; i < Rep[j2]; ++i) {
         sstring_Run (gen, res, 1, 1 * BILLION, 20, 10);
         bbattery_pVal[++j] = res->NRuns->pVal2[gofw_Mean];
         TestNumber[j] = j2;
         strcpy (bbattery_TestNames[j], "Run of bits, r = 20");
         bbattery_pVal[++j] = res->NBits->pVal2[gofw_Mean];
         TestNumber[j] = j2;
         strcpy (bbattery_TestNames[j], "Run of bits, r = 20");
      }
      sstring_DeleteRes3 (res);
   }
*/

    tests.emplace_back(++j2, "AutoCor, d = 1",
        sstring_AutoCor_cb(10, 30 + BILLION, r, s, 1));

    tests.emplace_back(++j2, "AutoCor, d = 1",
        sstring_AutoCor_cb(10, 30 + BILLION, r, s, 1));

    tests.emplace_back(++j2, "AutoCor, d = 1",
        sstring_AutoCor_cb(5, 1 + BILLION, 20, 10, 1));

    tests.emplace_back(++j2, "AutoCor, d = 30",
        sstring_AutoCor_cb(10, 31 + BILLION, r, s, s));

    tests.emplace_back(++j2, "AutoCor, d = 10",
        sstring_AutoCor_cb(5, 11 + BILLION, 20, 10, 10));

    run_tests(tests, create_gen, "Crush(mt)");
}




////////////////////////////////////////////////////////////////////////////////
int main (void) 
{
    // (10, 7), (17, 5), (55, 24)
    //LFibGenerator<17, 5> lcgobj(123);
    LcgGenerator lcgobj;
    //bbattery_SmallCrush(lcgobj.GetPtr());
/*
    custom_Crush([] () -> std::shared_ptr<UniformGenerator> {
        return std::shared_ptr<UniformGenerator>(new LFibGenerator<17, 5>(123));
    });
*/

    custom_SmallCrush([] () -> std::shared_ptr<UniformGenerator> {
        return std::shared_ptr<UniformGenerator>(new LcgGenerator());
        //return std::shared_ptr<UniformGenerator>(new LFibGenerator<17, 5>(123));
    });


    return 0;
}
