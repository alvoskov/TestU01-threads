#include "crush.h"



/*
 * A battery of stringent statistical tests for Random Number Generators
 * used in simulation.
 * Rep[i] gives the number of times that test i will be done. The default
 * values are Rep[i] = 1 for all i.
 */
void mt_bat_Crush (std::function<std::shared_ptr<UniformGenerator>()> create_gen)
{
    constexpr int s = 30, r = 0;
    int j2 = 0;
    std::vector<TestDescr> tests;

    printf ("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n"
        "                 Starting Crush\n"
        "                 Version: %s\n"
        "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n\n\n",
        PACKAGE_STRING);

    // SerialOver tests
    tests.emplace_back(++j2, "SerialOver, t = 2",
        smarsa_SerialOver_cb(1, 500 * MILLION, 0, 4096, 2));

    tests.emplace_back(++j2, "SerialOver, t = 4",
        smarsa_SerialOver_cb(1, 300 * MILLION, 0, 64, 4));

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
    tests.emplace_back(++j2, "SimpPoker, d = 16", 
        sknuth_SimpPoker_cb(1, 40 * MILLION, 0, 16, 16));

    tests.emplace_back(++j2, "SimpPoker, d = 16", 
        sknuth_SimpPoker_cb(1, 40 * MILLION, 26, 16, 16));

    tests.emplace_back(++j2, "SimpPoker, d = 64", 
        sknuth_SimpPoker_cb(1, 10 * MILLION, 0, 64, 64));

    tests.emplace_back(++j2, "SimpPoker, d = 64", 
        sknuth_SimpPoker_cb(1, 10 * MILLION, 24, 64, 64));

    // CouponCollector tests
    tests.emplace_back(++j2, "CouponCollector, d = 4",
        sknuth_CouponCollector_cb(1, 40 * MILLION, 0, 4));

    tests.emplace_back(++j2, "CouponCollector, d = 4",
        sknuth_CouponCollector_cb(1, 40 * MILLION, 28, 4));

    tests.emplace_back(++j2, "CouponCollector, d = 16",
        sknuth_CouponCollector_cb(1, 10 * MILLION, 0, 16));

    tests.emplace_back(++j2, "CouponCollector, d = 16",
        sknuth_CouponCollector_cb(1, 10 * MILLION, 26, 16));

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
    tests.emplace_back(++j2, "Permutation, r = 0",
        sknuth_Permutation_cb(1, 50 * MILLION, 0, 10));

    tests.emplace_back(++j2, "Permutation, r = 15",
        sknuth_Permutation_cb(1, 50 * MILLION, 0, 10));

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
*/
    // GCD tests
    tests.emplace_back(++j2, "GCD, r = 0",
        smarsa_GCD_cb(1, 100 * MILLION, 0, 30));

    tests.emplace_back(++j2, "GCD, r = 10",
        smarsa_GCD_cb(1, 40 * MILLION, 10, 20));

    // RandomWalk tests
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


    // LinearComp tests
    tests.emplace_back(++j2, "LinearComp, r = 0",
        scomp_LinearComp_cb(1, 120 * THOUSAND, r, 1));

    tests.emplace_back(++j2, "LinearComp, r = 29",
        scomp_LinearComp_cb(1, 120 * THOUSAND, 29, 1));

    // LZ test
    tests.emplace_back(++j2, "LempelZiv",
        scomp_LempelZiv_cb(10, 25, r, s));

    // Fourier3 test
    tests.emplace_back(++j2, "Fourier3, r = 0",
        sspectral_Fourier3_cb(50 * THOUSAND, 14, r, s));

    tests.emplace_back(++j2, "Fourier3, r = 20",
        sspectral_Fourier3_cb(50 * THOUSAND, 14, 20, 10));


    // LongestHeadRun tests
    tests.emplace_back(++j2, "LongestHeadRun, r = 0",
        sstring_LongestHeadRun_cb(1, 1000, r, s, 20 + 10 * MILLION));

    tests.emplace_back(++j2, "LongestHeadRun, r = 20",
        sstring_LongestHeadRun_cb(1, 300, 20, 10, 20 + 10 * MILLION));

    // PeriodsInStrings tests
    tests.emplace_back(++j2, "PeriodsInStrings, r = 0",
        sstring_PeriodsInStrings_cb(1, 300 * MILLION, r, s));

    tests.emplace_back(++j2, "PeriodsInStrings, r = 15",
        sstring_PeriodsInStrings_cb(1, 300 * MILLION, 15, 15));

/*
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

    // HammingCorr tests
    tests.emplace_back(++j2, "HammingCorr, L = 30",
        sstring_HammingCorr_cb(1, 500 * MILLION, r, s, s));

    tests.emplace_back(++j2, "HammingCorr, L = 300",
        sstring_HammingCorr_cb(1, 50 * MILLION, r, s, 10 * s));

    tests.emplace_back(++j2, "HammingCorr, L = 1200",
        sstring_HammingCorr_cb(1, 10 * MILLION, r, s, 40 * s));


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


    // sstring_Run tests
    tests.emplace_back(++j2, "Run of bits, r = 0",
        sstring_Run_cb(1, 1 * BILLION, r, s));

    tests.emplace_back(++j2, "Run of bits, r = 20",
        sstring_Run_cb(1, 1 * BILLION, 20, 10));

    // AutoCor tests
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
