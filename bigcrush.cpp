#include "bigcrush.h"

BigCrushBattery::BigCrushBattery(GenFactoryFunc genf)
    : TestsBattery(genf)
{
    constexpr int s = 30, r = 0;
    int j2 = 0;
    battery_name = "BigCrush(mt)";

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


    // ClosePairs tests
    // Beware of snpair_mNP2S_Flag global variable!
    tests.emplace_back(++j2, "ClosePairs",
        snpair_ClosePairs_cb(30, 6 * MILLION, 0, 3, 0, 30, ", t = 3", true));

    tests.emplace_back(++j2, "ClosePairs",
        snpair_ClosePairs_cb(20, 4 * MILLION, 0, 5, 0, 30, ", t = 5", true));

    tests.emplace_back(++j2, "ClosePairs",
        snpair_ClosePairs_cb(10, 3 * MILLION, 0, 9, 0, 30, ", t = 9", true));

    tests.emplace_back(++j2, "ClosePairs",
        snpair_ClosePairs_cb(5,  2 * MILLION, 0, 16, 0, 30, ", t = 16", true));

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

    // sknuth_Run tests
    tests.emplace_back(++j2, "Run, r = 0",
        sknuth_Run_cb(5, BILLION, 0, false));

    tests.emplace_back(++j2, "Run, r = 15",
        sknuth_Run_cb(10, BILLION, 15, true));

    // Permutation tests
    tests.emplace_back(++j2, "Permutation, t = 3",
         sknuth_Permutation_cb(1, BILLION, 5, 3));

    tests.emplace_back(++j2, "Permutation, t = 5",
         sknuth_Permutation_cb(1, BILLION, 5, 5));

    tests.emplace_back(++j2, "Permutation, t = 7",
         sknuth_Permutation_cb(1, BILLION/2, 5, 7));

    tests.emplace_back(++j2, "Permutation, t = 10",
         sknuth_Permutation_cb(1, BILLION/2, 10, 10));

    // CollisionPermut tests
    tests.emplace_back(++j2, "CollisionPermut, r = 0",
         sknuth_CollisionPermut_cb(20, 20 * MILLION, 0, 14));

    tests.emplace_back(++j2, "CollisionPermut, r = 10",
         sknuth_CollisionPermut_cb(20, 20 * MILLION, 10, 14));
        
    // MaxOft tests
    tests.emplace_back(++j2, "MaxOft, t = 8",
        sknuth_MaxOft_cb(40, 10 * MILLION, 0, MILLION / 10, 8));

    tests.emplace_back(++j2, "MaxOft, t = 16",
        sknuth_MaxOft_cb(30, 10 * MILLION, 0, MILLION / 10, 16));

    tests.emplace_back(++j2, "MaxOft, t = 24",
        sknuth_MaxOft_cb(20, 10 * MILLION, 0, MILLION / 10, 24));

    tests.emplace_back(++j2, "MaxOft, t = 32",
        sknuth_MaxOft_cb(20, 10 * MILLION, 0, MILLION / 10, 32));

    // "Sample..." tests
    tests.emplace_back(++j2, "SampleProd, t = 8",
        svaria_SampleProd_cb(40, 10 * MILLION, 0, 8));

    tests.emplace_back(++j2, "SampleProd, t = 16",
        svaria_SampleProd_cb(20, 10*MILLION, 0, 16));

    tests.emplace_back(++j2, "SampleProd, t = 24",
        svaria_SampleProd_cb(20, 10*MILLION, 0, 24));

    tests.emplace_back(++j2, "SampleMean, r = 0",
        svaria_SampleMean_cb(20*MILLION, 30, 0));

    tests.emplace_back(++j2, "SampleMean, r = 10",
        svaria_SampleMean_cb(20*MILLION, 30, 10));

    tests.emplace_back(++j2, "SampleCorr, k = 1",
        svaria_SampleCorr_cb(1, 2*BILLION, 0, 1));

    tests.emplace_back(++j2, "SampleCorr, k = 2",
        svaria_SampleCorr_cb(1, 2*BILLION, 0, 2));

    // AppearanceSpacings tests
    tests.emplace_back(++j2, "AppearanceSpacings, r = 0",
        svaria_AppearanceSpacings_cb(1, 10 * MILLION, BILLION, r, 3, 15));

    tests.emplace_back(++j2, "AppearanceSpacings, r = 27",
        svaria_AppearanceSpacings_cb(1, 10 * MILLION, BILLION, 27, 3, 15));

    // WeightDistrib tests
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

    // SumCollector test
    tests.emplace_back(++j2, "SumCollector",
        svaria_SumCollector_cb(1, 500 * MILLION, 0, 10.0));

    // MatrixRank tests
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

    // Savir2 and GCD tests
    tests.emplace_back(++j2, "Savir2",
        smarsa_Savir2_cb(10, 10 * MILLION, 10, 1024*1024, 30));

    tests.emplace_back(++j2, "GCD",
        smarsa_GCD_cb(10, 50 * MILLION, 0, 30));

    // RandomWalk tests
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

    // LinearComp tests
    tests.emplace_back(++j2, "LinearComp, r = 0",
         scomp_LinearComp_cb(1, 400 * THOUSAND + 20, r, 1));

    tests.emplace_back(++j2, "LinearComp, r = 29", // Error with "r = 0" in original crush was fixed
         scomp_LinearComp_cb(1, 400 * THOUSAND + 20, 29, 1));

    // LempelZiv tests
    tests.emplace_back(++j2, "LempelZiv, r = 0",
        scomp_LempelZiv_cb(10, 27, r, s));

    tests.emplace_back(++j2, "LempelZiv, r = 15",
        scomp_LempelZiv_cb(10, 27, 15, 15));

    // Fourier3 tests
    tests.emplace_back(++j2, "Fourier3, r = 0",
        sspectral_Fourier3_cb(100 * THOUSAND, 14, r, 3));

    tests.emplace_back(++j2, "Fourier3, r = 27",
        sspectral_Fourier3_cb(100 * THOUSAND, 14, 27, 3));

    // LongestHeadRun tests
    tests.emplace_back(++j2, "LongestHeadRun, r = 0",
        sstring_LongestHeadRun_cb(1, 1000, r, 3, 20 + 10 * MILLION));

    tests.emplace_back(++j2, "LongestHeadRun, r = 27",
        sstring_LongestHeadRun_cb(1, 1000, 27, 3, 20 + 10 * MILLION));

    // PeriodsInStrings
    tests.emplace_back(++j2, "PeriodsInStrings, r = 0",
        sstring_PeriodsInStrings_cb(10, BILLION/2, r, 10));

    tests.emplace_back(++j2, "PeriodsInStrings, r = 20",
        sstring_PeriodsInStrings_cb(10, BILLION/2, 20, 10));

    // HammingWeight2 tests
    tests.emplace_back(++j2, "HammingWeight2, r = 0",
        sstring_HammingWeight2_cb(10, BILLION, r, 3, MILLION));

    tests.emplace_back(++j2, "HammingWeight2, r = 27",
        sstring_HammingWeight2_cb(10, BILLION, 27, 3, MILLION));

    // HammingCorr tests
    tests.emplace_back(++j2, "HammingCorr, L = 30",
        sstring_HammingCorr_cb(1, BILLION, 10, 10, s));

    tests.emplace_back(++j2, "HammingCorr, L = 300",
        sstring_HammingCorr_cb(1, 100 * MILLION, 10, 10, 10 * s));

    tests.emplace_back(++j2, "HammingCorr, L = 1200",
        sstring_HammingCorr_cb(1, 100 * MILLION, 10, 10, 40 * s));

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

    // sstring_Run tests
    tests.emplace_back(++j2, "Run of bits, r = 0",
         sstring_Run_cb(1, 2*BILLION, r, 3));

    tests.emplace_back(++j2, "Run of bits, r = 27",
         sstring_Run_cb(1, 2*BILLION, 27, 3));

    // AutoCor tests
    tests.emplace_back(++j2, "AutoCor, d=1, r=0",
        sstring_AutoCor_cb(10, 30 + BILLION, r, 3, 1));

    tests.emplace_back(++j2, "AutoCor, d=3, r=0",
        sstring_AutoCor_cb(10, 30 + BILLION, r, 3, 3));

    tests.emplace_back(++j2, "AutoCor, d=1, r=27",
        sstring_AutoCor_cb(10, 30 + BILLION, 27, 3, 1));

    tests.emplace_back(++j2, "AutoCor, d=3, r=27",
        sstring_AutoCor_cb(10, 30 + BILLION, 27, 3, 3));
    //util_Assert (j2 <= BIGCRUSH_NUM, "BigCrush:   j2 > BIGCRUSH_NUM");
}
