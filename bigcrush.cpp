#include "bigcrush.h"

/**
 * @brief A multi-threaded version of BigCrush test battery.
 */
void mt_bat_BigCrush (std::function<std::shared_ptr<UniformGenerator>()> create_gen)
{
    constexpr int s = 30, r = 0;
    int j2 = 0;
    std::vector<TestDescr> tests;

    printf (
        "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n"
        "                 Starting BigCrush\n"
        "                 Version: %s\n"
        "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n\n\n",
        PACKAGE_STRING);

    // SerialOver tests
    tests.emplace_back(++j2, "SerialOver, r = 0",
        smarsa_SerialOver_cb(1, BILLION, 0, 256, 3));

    tests.emplace_back(++j2, "SerialOver, r = 22",
        smarsa_SerialOver_cb(1, BILLION, 22, 256, 3));

    // CollisionOver tests
    tests.emplace_back(++j2, "CollisionOver, t = 2",
        smarsa_CollisionOver_cb(30, 20 * MILLION, 0, 1024*1024*2, 2));

    tests.emplace_back(++j2, "CollisionOver, t = 2",
        smarsa_CollisionOver_cb(30, 20 * MILLION, 9, 1024*1024*2, 2));

    tests.emplace_back(++j2, "CollisionOver, t = 3",
        smarsa_CollisionOver_cb(30, 20 * MILLION, 0, 1024*16, 3));

    tests.emplace_back(++j2, "CollisionOver, t = 3",
        smarsa_CollisionOver_cb(30, 20 * MILLION, 16, 1024*16, 3));

    tests.emplace_back(++j2, "CollisionOver, t = 7",
        smarsa_CollisionOver_cb(30, 20 * MILLION, 0, 64, 7));

    tests.emplace_back(++j2, "CollisionOver, t = 7",
        smarsa_CollisionOver_cb(30, 20 * MILLION, 24, 64, 7));

    tests.emplace_back(++j2, "CollisionOver, t = 14",
        smarsa_CollisionOver_cb(30, 20 * MILLION, 0, 8, 14));

    tests.emplace_back(++j2, "CollisionOver, t = 14",
        smarsa_CollisionOver_cb(30, 20 * MILLION, 27, 8, 14));

    tests.emplace_back(++j2, "CollisionOver, t = 21",
        smarsa_CollisionOver_cb(30, 20 * MILLION, 0, 4, 21));

    tests.emplace_back(++j2, "CollisionOver, t = 21",
        smarsa_CollisionOver_cb(30, 20 * MILLION, 28, 4, 21));


    // BirthdaySpacings tests
#if LONG_MAX <= 2147483647L
    tests.emplace_back(++j2, "BirthdaySpacings, t = 2",
        smarsa_BirthdaySpacings_cb(250, 4 * MILLION, 0, 1073741824L, 2, 1));
#else
    tests.emplace_back(++j2, "BirthdaySpacings, t = 2",
        smarsa_BirthdaySpacings_cb(100, 10 * MILLION, 0, 2147483648L, 2, 1));
#endif

    tests.emplace_back(++j2, "BirthdaySpacings, t = 3",
        smarsa_BirthdaySpacings_cb(20, 20 * MILLION, 0, 2097152, 3, 1));

    tests.emplace_back(++j2, "BirthdaySpacings, t = 4",
        smarsa_BirthdaySpacings_cb(20, 30 * MILLION, 14, 65536, 4, 1));

    tests.emplace_back(++j2, "BirthdaySpacings, t = 7",
        smarsa_BirthdaySpacings_cb(20, 20 * MILLION, 0, 512, 7, 1));

    tests.emplace_back(++j2, "BirthdaySpacings, t = 7",
        smarsa_BirthdaySpacings_cb(20, 20 * MILLION, 7, 512, 7, 1));

    tests.emplace_back(++j2, "BirthdaySpacings, t = 8",
        smarsa_BirthdaySpacings_cb(20, 30 * MILLION, 14, 256, 8, 1));

    tests.emplace_back(++j2, "BirthdaySpacings, t = 8",
        smarsa_BirthdaySpacings_cb(20, 30 * MILLION, 22, 256, 8, 1));

    tests.emplace_back(++j2, "BirthdaySpacings, t = 16",
        smarsa_BirthdaySpacings_cb(20, 30 * MILLION, 0, 16, 16, 1));

    tests.emplace_back(++j2, "BirthdaySpacings, t = 16",
        smarsa_BirthdaySpacings_cb(20, 30 * MILLION, 26, 16, 16, 1));

/*
   {
      lebool flag = snpair_mNP2S_Flag;
      snpair_Res *res;
      res = snpair_CreateRes ();

      snpair_mNP2S_Flag = TRUE;
      ++j2;
      for (i = 0; i < Rep[j2]; ++i) {
         snpair_ClosePairs (gen, res, 30, 6 * MILLION, 0, 3, 0, 30);
         GetPVal_CPairs (40, res, &j, ", t = 3", j2);
      }

      ++j2;
      for (i = 0; i < Rep[j2]; ++i) {
         snpair_ClosePairs (gen, res, 20, 4 * MILLION, 0, 5, 0, 30);
         GetPVal_CPairs (40, res, &j, ", t = 5", j2);
      }

      ++j2;
      for (i = 0; i < Rep[j2]; ++i) {
         snpair_ClosePairs (gen, res, 10, 3 * MILLION, 0, 9, 0, 30);
         GetPVal_CPairs (20, res, &j, ", t = 9", j2);
      }

      ++j2;
      for (i = 0; i < Rep[j2]; ++i) {
         snpair_ClosePairs (gen, res, 5, 2*MILLION, 0, 16, 0, 30);
         GetPVal_CPairs (10, res, &j, ", t = 16", j2);
      }
      snpair_DeleteRes (res);
      snpair_mNP2S_Flag =flag;
   }
*/

//   {
//      sres_Chi2 *res;
//      res = sres_CreateChi2 ();

    // SimpPoker tests
    tests.emplace_back(++j2, "SimpPoker, r = 0",
        sknuth_SimpPoker_cb(1, 400 * MILLION, 0, 8, 8));

    tests.emplace_back(++j2, "SimpPoker, r = 27",
        sknuth_SimpPoker_cb(1, 400 * MILLION, 27, 8, 8));

    tests.emplace_back(++j2, "SimpPoker, r = 0",
        sknuth_SimpPoker_cb(1, 100 * MILLION, 0, 32, 32));

    tests.emplace_back(++j2, "SimpPoker, r = 25",
        sknuth_SimpPoker_cb(1, 100 * MILLION, 25, 32, 32));


    // CouponCollector tests    
    tests.emplace_back(++j2, "CouponCollector, r = 0",
        sknuth_CouponCollector_cb(1, 200 * MILLION, 0, 8));

    tests.emplace_back(++j2, "CouponCollector, r = 10",
        sknuth_CouponCollector_cb(1, 200 * MILLION, 10, 8));

    tests.emplace_back(++j2, "CouponCollector, r = 20",
        sknuth_CouponCollector_cb(1, 200 * MILLION, 20, 8));

    tests.emplace_back(++j2, "CouponCollector, r = 27",
        sknuth_CouponCollector_cb(1, 200 * MILLION, 27, 8));

    // Gap tests
    tests.emplace_back(++j2, "Gap, r = 0",
        sknuth_Gap_cb(1, BILLION/2, 0, 0.0, 1.0/16.0));

    tests.emplace_back(++j2, "Gap, r = 25",
        sknuth_Gap_cb(1, 300*MILLION, 25, 0.0, 1.0/32.0));

    tests.emplace_back(++j2, "Gap, r = 0",
        sknuth_Gap_cb(1, BILLION/10, 0, 0.0, 1.0/128.0));

    tests.emplace_back(++j2, "Gap, r = 20",
        sknuth_Gap_cb(1, 10*MILLION, 20, 0.0, 1.0/1024.0));

/*
      ++j2;
      for (i = 0; i < Rep[j2]; ++i) {
         sknuth_Run (gen, res, 5, BILLION, 0, FALSE);
         bbattery_pVal[++j] = res->pVal2[gofw_Sum];
         TestNumber[j] = j2;
         strcpy (bbattery_TestNames[j], "Run, r = 0");
      }

      ++j2;
      for (i = 0; i < Rep[j2]; ++i) {
         sknuth_Run (gen, res, 10, BILLION, 15, TRUE);
         bbattery_pVal[++j] = res->pVal2[gofw_Sum];
         TestNumber[j] = j2;
         strcpy (bbattery_TestNames[j], "Run, r = 15");
      }
*/


    tests.emplace_back(++j2, "Permutation, t = 3",
         sknuth_Permutation_cb(1, BILLION, 5, 3));

    tests.emplace_back(++j2, "Permutation, t = 5",
         sknuth_Permutation_cb(1, BILLION, 5, 5));

    tests.emplace_back(++j2, "Permutation, t = 7",
         sknuth_Permutation_cb(1, BILLION/2, 5, 7));

    tests.emplace_back(++j2, "Permutation, t = 10",
         sknuth_Permutation_cb(1, BILLION/2, 10, 10));

/*
   {
      sknuth_Res2 *res;
      res = sknuth_CreateRes2 ();
      ++j2;
      for (i = 0; i < Rep[j2]; ++i) {
         sknuth_CollisionPermut (gen, res, 20, 20 * MILLION, 0, 14);
         bbattery_pVal[++j] = res->Pois->pVal2;
         TestNumber[j] = j2;
         strcpy (bbattery_TestNames[j], "CollisionPermut, r = 0");
      }

      ++j2;
      for (i = 0; i < Rep[j2]; ++i) {
         sknuth_CollisionPermut (gen, res, 20, 20 * MILLION, 10, 14);
         bbattery_pVal[++j] = res->Pois->pVal2;
         TestNumber[j] = j2;
         strcpy (bbattery_TestNames[j], "CollisionPermut, r = 10");
      }
      sknuth_DeleteRes2 (res);
   }
*/

    tests.emplace_back(++j2, "MaxOft, t = 8",
        sknuth_MaxOft_cb(40, 10 * MILLION, 0, MILLION / 10, 8));

    tests.emplace_back(++j2, "MaxOft, t = 16",
        sknuth_MaxOft_cb(30, 10 * MILLION, 0, MILLION / 10, 16));

    tests.emplace_back(++j2, "MaxOft, t = 24",
        sknuth_MaxOft_cb(20, 10 * MILLION, 0, MILLION / 10, 24));

    tests.emplace_back(++j2, "MaxOft, t = 32",
        sknuth_MaxOft_cb(20, 10 * MILLION, 0, MILLION / 10, 32));

/*
   {
      sres_Basic *res;
      res = sres_CreateBasic ();
      ++j2;
      for (i = 0; i < Rep[j2]; ++i) {
         svaria_SampleProd (gen, res, 40, 10 * MILLION, 0, 8);
         bbattery_pVal[++j] = res->pVal2[gofw_AD];
         TestNumber[j] = j2;
         strcpy (bbattery_TestNames[j], "SampleProd, t = 8");
      }

      ++j2;
      for (i = 0; i < Rep[j2]; ++i) {
         svaria_SampleProd (gen, res, 20, 10*MILLION, 0, 16);
         bbattery_pVal[++j] = res->pVal2[gofw_AD];
         TestNumber[j] = j2;
         strcpy (bbattery_TestNames[j], "SampleProd, t = 16");
      }

      ++j2;
      for (i = 0; i < Rep[j2]; ++i) {
         svaria_SampleProd (gen, res, 20, 10*MILLION, 0, 24);
         bbattery_pVal[++j] = res->pVal2[gofw_AD];
         TestNumber[j] = j2;
         strcpy (bbattery_TestNames[j], "SampleProd, t = 24");
      }

      ++j2;
      for (i = 0; i < Rep[j2]; ++i) {
         svaria_SampleMean (gen, res, 20*MILLION, 30, 0);
         bbattery_pVal[++j] = res->pVal2[gofw_AD];
         TestNumber[j] = j2;
         strcpy (bbattery_TestNames[j], "SampleMean, r = 0");
      }

      ++j2;
      for (i = 0; i < Rep[j2]; ++i) {
         svaria_SampleMean (gen, res, 20*MILLION, 30, 10);
         bbattery_pVal[++j] = res->pVal2[gofw_AD];
         TestNumber[j] = j2;
         strcpy (bbattery_TestNames[j], "SampleMean, r = 10");
      }

      ++j2;
      for (i = 0; i < Rep[j2]; ++i) {
         svaria_SampleCorr (gen, res, 1, 2*BILLION, 0, 1);
         bbattery_pVal[++j] = res->pVal2[gofw_Mean];
         TestNumber[j] = j2;
         strcpy (bbattery_TestNames[j], "SampleCorr, k = 1");
      }

      ++j2;
      for (i = 0; i < Rep[j2]; ++i) {
         svaria_SampleCorr (gen, res, 1, 2*BILLION, 0, 2);
         bbattery_pVal[++j] = res->pVal2[gofw_Mean];
         TestNumber[j] = j2;
         strcpy (bbattery_TestNames[j], "SampleCorr, k = 2");
      }

      ++j2;
      for (i = 0; i < Rep[j2]; ++i) {
         svaria_AppearanceSpacings (gen, res, 1, 10 * MILLION, BILLION,
            r, 3, 15);
         bbattery_pVal[++j] = res->pVal2[gofw_Mean];
         TestNumber[j] = j2;
         strcpy (bbattery_TestNames[j], "AppearanceSpacings, r = 0");
      }

      ++j2;
      for (i = 0; i < Rep[j2]; ++i) {
         svaria_AppearanceSpacings (gen, res, 1, 10 * MILLION, BILLION,
            27, 3, 15);
         bbattery_pVal[++j] = res->pVal2[gofw_Mean];
         TestNumber[j] = j2;
         strcpy (bbattery_TestNames[j], "AppearanceSpacings, r = 27");
      }
      sres_DeleteBasic (res);
   }
   {
      smarsa_Res2 *res2;
      sres_Chi2 *res;
      res = sres_CreateChi2 ();
*/


    tests.emplace_back(++j2, "WeightDistrib, r = 0",
        svaria_WeightDistrib_cb(1, 20 * MILLION, 0, 256, 0.0, 0.25));

    tests.emplace_back(++j2, "WeightDistrib, r = 20",
        svaria_WeightDistrib_cb(1, 20 * MILLION, 20, 256, 0.0, 0.25));

    tests.emplace_back(++j2, "WeightDistrib, r = 28",
        svaria_WeightDistrib_cb(1, 20 * MILLION, 28, 256, 0.0, 0.25));

    tests.emplace_back(++j2, "WeightDistrib, r = 0",
        svaria_WeightDistrib_cb(1, 20 * MILLION, 0, 256, 0.0, 0.0625));

    tests.emplace_back(++j2, "WeightDistrib, r = 10",
        svaria_WeightDistrib_cb(1, 20 * MILLION, 10, 256, 0.0, 0.0625));

    tests.emplace_back(++j2, "WeightDistrib, r = 26",
        svaria_WeightDistrib_cb(1, 20 * MILLION, 26, 256, 0.0, 0.0625));


/*
      ++j2;
      for (i = 0; i < Rep[j2]; ++i) {
         svaria_SumCollector (gen, res, 1, 500 * MILLION, 0, 10.0);
         bbattery_pVal[++j] = res->pVal2[gofw_Mean];
         TestNumber[j] = j2;
         strcpy (bbattery_TestNames[j], "SumCollector");
      }
*/

    tests.emplace_back(++j2, "MatrixRank, L=30, r=0",
        smarsa_MatrixRank_cb(10, MILLION, r, 5, 30, 30));

    tests.emplace_back(++j2, "MatrixRank, L=30, r=26",
        smarsa_MatrixRank_cb(10, MILLION, 25, 5, 30, 30));

    tests.emplace_back(++j2, "MatrixRank, L=1000, r=0",
        smarsa_MatrixRank_cb(1, 5 * THOUSAND, r, 4, 1000, 1000));

    tests.emplace_back(++j2, "MatrixRank, L=1000, r=26",
        smarsa_MatrixRank_cb(1, 5 * THOUSAND, 26, 4, 1000, 1000));

    tests.emplace_back(++j2, "MatrixRank, L=5000",
        smarsa_MatrixRank_cb(1, 80, 15, 15, 5000, 5000));

    tests.emplace_back(++j2, "MatrixRank, L=5000",
        smarsa_MatrixRank_cb(1, 80, 0, 30, 5000, 5000));

/*

      ++j2;
      for (i = 0; i < Rep[j2]; ++i) {
         smarsa_Savir2 (gen, res, 10, 10 * MILLION, 10, 1024*1024, 30);
         bbattery_pVal[++j] = res->pVal2[gofw_Sum];
         TestNumber[j] = j2;
         strcpy (bbattery_TestNames[j], "Savir2");
      }
      sres_DeleteChi2 (res);

      res2 = smarsa_CreateRes2 ();
      ++j2;
      for (i = 0; i < Rep[j2]; ++i) {
         smarsa_GCD (gen, res2, 10, 50 * MILLION, 0, 30);
         bbattery_pVal[++j] = res2->GCD->pVal2[gofw_Sum];
         TestNumber[j] = j2;
         strcpy (bbattery_TestNames[j], "GCD");
      }
      smarsa_DeleteRes2 (res2);
   }
*/

    tests.emplace_back(++j2, "RandomWalk",
        smarsa_RandomWalk1_cb(1, 100 * MILLION, r, 5, 50, 50, " (L=50, r=0)"));

    tests.emplace_back(++j2, "RandomWalk",
        smarsa_RandomWalk1_cb(1, 100 * MILLION, 25, 5, 50, 50, " (L=50, r=25)"));

    tests.emplace_back(++j2, "RandomWalk",
        smarsa_RandomWalk1_cb(1, 10 * MILLION, r, 10, 1000, 1000, " (L=1000, r=0)"));

    tests.emplace_back(++j2, "RandomWalk",
        smarsa_RandomWalk1_cb(1, 10 * MILLION, 20, 10, 1000, 1000, " (L=1000, r=20)"));

    tests.emplace_back(++j2, "RandomWalk",
        smarsa_RandomWalk1_cb(1, 1 * MILLION, r, 15, 10000, 10000, " (L=10000, r=0)"));

    tests.emplace_back(++j2, "RandomWalk",
        smarsa_RandomWalk1_cb(1, 1 * MILLION, 15, 15, 10000, 10000, " (L=10000, r=15)"));

/*
   {
      scomp_Res *res;
      res = scomp_CreateRes ();
      ++j2;
      for (i = 0; i < Rep[j2]; ++i) {
         scomp_LinearComp (gen, res, 1, 400 * THOUSAND + 20, r, 1);
         bbattery_pVal[++j] = res->JumpNum->pVal2[gofw_Mean];
         TestNumber[j] = j2;
         strcpy (bbattery_TestNames[j], "LinearComp, r = 0");
         bbattery_pVal[++j] = res->JumpSize->pVal2[gofw_Mean];
         TestNumber[j] = j2;
         strcpy (bbattery_TestNames[j], "LinearComp, r = 0");
      }

      ++j2;
      for (i = 0; i < Rep[j2]; ++i) {
         scomp_LinearComp (gen, res, 1, 400 * THOUSAND + 20, 29, 1);
         bbattery_pVal[++j] = res->JumpNum->pVal2[gofw_Mean];
         TestNumber[j] = j2;
         strcpy (bbattery_TestNames[j], "LinearComp, r = 29");
         bbattery_pVal[++j] = res->JumpSize->pVal2[gofw_Mean];
         TestNumber[j] = j2;
         strcpy (bbattery_TestNames[j], "LinearComp, r = 0");
      }
      scomp_DeleteRes (res);
   }
*/

    tests.emplace_back(++j2, "LempelZiv, r = 0",
        scomp_LempelZiv_cb(10, 27, r, s));

    tests.emplace_back(++j2, "LempelZiv, r = 15",
        scomp_LempelZiv_cb(10, 27, 15, 15));

/*
   {
      sspectral_Res *res;
      res = sspectral_CreateRes ();
      ++j2;
      for (i = 0; i < Rep[j2]; ++i) {
         sspectral_Fourier3 (gen, res, 100 * THOUSAND, 14, r, 3);
         bbattery_pVal[++j] = res->Bas->pVal2[gofw_AD];
         TestNumber[j] = j2;
         strcpy (bbattery_TestNames[j], "Fourier3, r = 0");
      }

      ++j2;
      for (i = 0; i < Rep[j2]; ++i) {
         sspectral_Fourier3 (gen, res, 100 * THOUSAND, 14, 27, 3);
         bbattery_pVal[++j] = res->Bas->pVal2[gofw_AD];
         TestNumber[j] = j2;
         strcpy (bbattery_TestNames[j], "Fourier3, r = 27");
      }
      sspectral_DeleteRes (res);
   }
   {
      sstring_Res2 *res;
      res = sstring_CreateRes2 ();
      ++j2;
      for (i = 0; i < Rep[j2]; ++i) {
         sstring_LongestHeadRun (gen, res, 1, 1000, r, 3, 20 + 10 * MILLION);
         bbattery_pVal[++j] = res->Chi->pVal2[gofw_Mean];
         TestNumber[j] = j2;
         strcpy (bbattery_TestNames[j], "LongestHeadRun, r = 0");
         bbattery_pVal[++j] = res->Disc->pVal2;
         TestNumber[j] = j2;
         strcpy (bbattery_TestNames[j], "LongestHeadRun, r = 0");
      }

      ++j2;
      for (i = 0; i < Rep[j2]; ++i) {
         sstring_LongestHeadRun (gen, res, 1, 1000, 27, 3, 20 + 10 * MILLION);
         bbattery_pVal[++j] = res->Chi->pVal2[gofw_Mean];
         TestNumber[j] = j2;
         strcpy (bbattery_TestNames[j], "LongestHeadRun, r = 27");
         bbattery_pVal[++j] = res->Disc->pVal2;
         TestNumber[j] = j2;
         strcpy (bbattery_TestNames[j], "LongestHeadRun, r = 27");
      }
      sstring_DeleteRes2 (res);
   }
   {
      sres_Chi2 *res;
      res = sres_CreateChi2 ();
      ++j2;
      for (i = 0; i < Rep[j2]; ++i) {
         sstring_PeriodsInStrings (gen, res, 10, BILLION/2, r, 10);
         bbattery_pVal[++j] = res->pVal2[gofw_Sum];
         TestNumber[j] = j2;
         strcpy (bbattery_TestNames[j], "PeriodsInStrings, r = 0");
      }

      ++j2;
      for (i = 0; i < Rep[j2]; ++i) {
         sstring_PeriodsInStrings (gen, res, 10, BILLION/2, 20, 10);
         bbattery_pVal[++j] = res->pVal2[gofw_Sum];
         TestNumber[j] = j2;
         strcpy (bbattery_TestNames[j], "PeriodsInStrings, r = 20");
      }
      sres_DeleteChi2 (res);
   }
   {
      sres_Basic *res;
      res = sres_CreateBasic ();
      ++j2;
      for (i = 0; i < Rep[j2]; ++i) {
         sstring_HammingWeight2 (gen, res, 10, BILLION, r, 3, MILLION);
         bbattery_pVal[++j] = res->pVal2[gofw_Sum];
         TestNumber[j] = j2;
         strcpy (bbattery_TestNames[j], "HammingWeight2, r = 0");
      }

      ++j2;
      for (i = 0; i < Rep[j2]; ++i) {
         sstring_HammingWeight2 (gen, res, 10, BILLION, 27, 3, MILLION);
         bbattery_pVal[++j] = res->pVal2[gofw_Sum];
         TestNumber[j] = j2;
         strcpy (bbattery_TestNames[j], "HammingWeight2, r = 27");
      }
      sres_DeleteBasic (res);
   }
   {
      sstring_Res *res;
      res = sstring_CreateRes ();
      ++j2;
      for (i = 0; i < Rep[j2]; ++i) {
         sstring_HammingCorr (gen, res, 1, BILLION, 10, 10, s);
         bbattery_pVal[++j] = res->Bas->pVal2[gofw_Mean];
         TestNumber[j] = j2;
         strcpy (bbattery_TestNames[j], "HammingCorr, L = 30");
      }

      ++j2;
      for (i = 0; i < Rep[j2]; ++i) {
         sstring_HammingCorr (gen, res, 1, 100 * MILLION, 10, 10, 10 * s);
         bbattery_pVal[++j] = res->Bas->pVal2[gofw_Mean];
         TestNumber[j] = j2;
         strcpy (bbattery_TestNames[j], "HammingCorr, L = 300");
      }

      ++j2;
      for (i = 0; i < Rep[j2]; ++i) {
         sstring_HammingCorr (gen, res, 1, 100 * MILLION, 10, 10, 40 * s);
         bbattery_pVal[++j] = res->Bas->pVal2[gofw_Mean];
         TestNumber[j] = j2;
         strcpy (bbattery_TestNames[j], "HammingCorr, L = 1200");
      }
*/

    // HammingIndep tests
    tests.emplace_back(++j2, "HammingIndep, L=30, r=0",
        sstring_HammingIndep_cb(10, 30 * MILLION, r, 3, s, 0));

    tests.emplace_back(++j2, "HammingIndep, L=30, r=27",
        sstring_HammingIndep_cb(10, 30 * MILLION, 27, 3, s, 0));

    tests.emplace_back(++j2, "HammingIndep, L=300, r=0",
        sstring_HammingIndep_cb(1, 30 * MILLION, r, 4, 10 * s, 0));

    tests.emplace_back(++j2, "HammingIndep, L=300, r=0",
        sstring_HammingIndep_cb(1, 30 * MILLION, 26, 4, 10 * s, 0));

    tests.emplace_back(++j2, "HammingIndep, L=1200, r=0",
        sstring_HammingIndep_cb(1, 10 * MILLION, r, 5, 40 * s, 0));

    tests.emplace_back(++j2, "HammingIndep, L=1200, r=25",
        sstring_HammingIndep_cb(1, 10 * MILLION, 25, 5, 40 * s, 0));

/*    
   {
      sstring_Res3 *res;
      res = sstring_CreateRes3 ();
      ++j2;
      for (i = 0; i < Rep[j2]; ++i) {
         sstring_Run (gen, res, 1, 2*BILLION, r, 3);
         bbattery_pVal[++j] = res->NRuns->pVal2[gofw_Mean];
         TestNumber[j] = j2;
         strcpy (bbattery_TestNames[j], "Run of bits, r = 0");
         bbattery_pVal[++j] = res->NBits->pVal2[gofw_Mean];
         TestNumber[j] = j2;
         strcpy (bbattery_TestNames[j], "Run of bits, r = 0");
      }

      ++j2;
      for (i = 0; i < Rep[j2]; ++i) {
         sstring_Run (gen, res, 1, 2*BILLION, 27, 3);
         bbattery_pVal[++j] = res->NRuns->pVal2[gofw_Mean];
         TestNumber[j] = j2;
         strcpy (bbattery_TestNames[j], "Run of bits, r = 27");
         bbattery_pVal[++j] = res->NBits->pVal2[gofw_Mean];
         TestNumber[j] = j2;
         strcpy (bbattery_TestNames[j], "Run of bits, r = 27");
      }
      sstring_DeleteRes3 (res);
   }
*/


    tests.emplace_back(++j2, "AutoCor, d=1, r=0",
        sstring_AutoCor_cb(10, 30 + BILLION, r, 3, 1));

    tests.emplace_back(++j2, "AutoCor, d=3, r=0",
        sstring_AutoCor_cb(10, 30 + BILLION, r, 3, 3));

    tests.emplace_back(++j2, "AutoCor, d=1, r=27",
        sstring_AutoCor_cb(10, 30 + BILLION, 27, 3, 1));

    tests.emplace_back(++j2, "AutoCor, d=3, r=27",
        sstring_AutoCor_cb(10, 30 + BILLION, 27, 3, 3));
    //util_Assert (j2 <= BIGCRUSH_NUM, "BigCrush:   j2 > BIGCRUSH_NUM");

    run_tests(tests, create_gen, "BigCrush(mt)");
}
