#include "testu01th/testu01_callbacks.h"
#include "testu01th/testu01_mt.h"


namespace testu01_threads {

///////////////////////////////////////////////
///// Some helpers required for callbacks /////
///////////////////////////////////////////////

/**
 * @brief Get the p-values in a swalk_RandomWalk1 test
 * @details It is a rewrite of the `GetPVal_Walk` function from `bbattery.c`.
 */
static void GetPValue_Walk(BatteryIO& io, long N, swalk_Res *res, int id, const std::string& mess)
{
   if (N == 1) {
        io.Add(id, "RandomWalk1 H" + mess, res->H[0]->pVal2[gofw_Mean]);
        io.Add(id, "RandomWalk1 M" + mess, res->M[0]->pVal2[gofw_Mean]);
        io.Add(id, "RandomWalk1 J" + mess, res->J[0]->pVal2[gofw_Mean]);
        io.Add(id, "RandomWalk1 R" + mess, res->R[0]->pVal2[gofw_Mean]);
        io.Add(id, "RandomWalk1 C" + mess, res->C[0]->pVal2[gofw_Mean]);
   } else {
        io.Add(id, "RandomWalk1 H" + mess, res->H[0]->pVal2[gofw_Sum]);
        io.Add(id, "RandomWalk1 M" + mess, res->M[0]->pVal2[gofw_Sum]);
        io.Add(id, "RandomWalk1 J" + mess, res->J[0]->pVal2[gofw_Sum]);
        io.Add(id, "RandomWalk1 R" + mess, res->R[0]->pVal2[gofw_Sum]);
        io.Add(id, "RandomWalk1 C" + mess, res->C[0]->pVal2[gofw_Sum]);
   }
}

/**
 * @brief Get the p-values in a snpair_ClosePairs test
 * @param flag Former snpair_mNP2S_Flag global variable (made local for thread safety).
 */
static void GetPValue_CPairs(BatteryIO& io, long N, snpair_Res *res, int id, const std::string& mess, bool flag)
{
    if (N == 1) {
        io.Add(id, "ClosePairs NP" + mess, res->pVal[snpair_NP]);
        io.Add(id, "ClosePairs mNP" + mess, res->pVal[snpair_mNP]);
   } else {
        io.Add(id, "ClosePairs NP" + mess, res->pVal[snpair_NP]);
        io.Add(id, "ClosePairs mNP" + mess, res->pVal[snpair_mNP]);
        io.Add(id, "ClosePairs mNP1" + mess, res->pVal[snpair_mNP1]);
        io.Add(id, "ClosePairs mNP2" + mess, res->pVal[snpair_mNP2]);
        io.Add(id, "ClosePairs NJumps" + mess, res->pVal[snpair_NJumps]);
        if (flag) {
            io.Add(id, "ClosePairs mNP2S" + mess, res->pVal[snpair_mNP2S]);
        }
   }
}


///////////////////////////////////////////////////////
///// Functions that generate callbacks for tests /////
///////////////////////////////////////////////////////


TestCbFunc svaria_AppearanceSpacings_cb(long N, long Q, long K, int r, int s, int L)
{
    return [=] (const TestDescr& td, BatteryIO& io) {
        sres_Basic* res = sres_CreateBasic();
        svaria_AppearanceSpacings(io.Gen()->GetPtr(), res, N, Q, K, r, s, L);
        io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_Mean]);
        sres_DeleteBasic(res);
    };
}

TestCbFunc sstring_AutoCor_cb(long N, long n, int r, int s, int d)
{
    return [=] (const TestDescr& td, BatteryIO& io) {
        sres_Basic* res = sres_CreateBasic();
        sstring_AutoCor(io.Gen()->GetPtr(), res, N, n, r, s, d);
        io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_Sum]);
        sres_DeleteBasic(res);
    };
}

TestCbFunc smarsa_BirthdaySpacings_cb(long N, long n, int r, long d, int t, int p)
{
    return [=] (const TestDescr& td, BatteryIO& io) {
        sres_Poisson* res = sres_CreatePoisson();
        smarsa_BirthdaySpacings(io.Gen()->GetPtr(), res, N, n, r, d, t, p);
        io.Add(td.GetId(), td.GetName(), res->pVal2);
        sres_DeletePoisson(res);
    };
}

TestCbFunc smarsa_CollisionOver_cb(long N, long n, int r, long d, int t)
{
    return [=] (const TestDescr& td, BatteryIO& io) {
        smarsa_Res* res = smarsa_CreateRes();
        smarsa_CollisionOver (io.Gen()->GetPtr(), res, N, n, r, d, t);
        io.Add(td.GetId(), td.GetName(), res->Pois->pVal2);
        smarsa_DeleteRes(res);
    };
}

TestCbFunc sknuth_CollisionPermut_cb(long N, long n, int r, int t)
{
    return [=] (const TestDescr& td, BatteryIO& io) {
        sknuth_Res2* res = sknuth_CreateRes2();
        sknuth_CollisionPermut(io.Gen()->GetPtr(), res, N, n, r, t);
        io.Add(td.GetId(), td.GetName(), res->Pois->pVal2);
        sknuth_DeleteRes2 (res);
    };
}

TestCbFunc sknuth_CouponCollector_cb(long N, long n, int r, int d)
{
    return [=] (const TestDescr& td, BatteryIO& io) {
        auto* res2 = sres_CreateChi2();
        sknuth_CouponCollector (io.Gen()->GetPtr(), res2, N, n, r, d);
        io.Add(td.GetId(), td.GetName(), res2->pVal2[gofw_Mean]);
        sres_DeleteChi2(res2);
    };
}


TestCbFunc snpair_ClosePairs_cb(long N, long n, int r, int k, int p, int m,
    const std::string& mess, bool flag)
{
    return [=] (const TestDescr& td, BatteryIO& io) {
        snpair_Res* res = snpair_CreateRes();
        snpair_ClosePairs(io.Gen()->GetPtr(), res, N, n, r, k, p, m);
        GetPValue_CPairs(io, 10, res, td.GetId(), mess, flag);
        snpair_DeleteRes(res);
    };
}

/**
 * @brief Needed for pseudoDIEHARD battery.
 */
TestCbFunc snpair_ClosePairsNP_cb(long N, long n, int r, int k, int p, int m)
{
    return [=] (const TestDescr& td, BatteryIO& io) {
        snpair_Res* res = snpair_CreateRes();
        snpair_ClosePairs(io.Gen()->GetPtr(), res, N, n, r, k, p, m);
        io.Add(td.GetId(), td.GetName(), res->pVal[snpair_NP]);
        snpair_DeleteRes(res);
    };
}

TestCbFunc snpair_ClosePairsBitMatch_cb(long N, long n, int r, int t)
{
    return [=] (const TestDescr &td, BatteryIO &io) {
        snpair_Res* res = snpair_CreateRes();
        snpair_ClosePairsBitMatch(io.Gen()->GetPtr(), res, N, n, r, t);
        io.Add(td.GetId(), td.GetName(), res->pVal[snpair_BM]);
        snpair_DeleteRes(res);
    };
}

/**
 * @brief An envelope for smarsa_CollisionOver for pseudoDIEHARD battery.
 */
TestCbFunc smarsa_Dna_cb(int i)
{
    return [=] (const TestDescr& td, BatteryIO& io) {
        printf("***********************************************************\n"
            "Test DNA calling smarsa_CollisionOver\n\n");
        smarsa_Res* res = smarsa_CreateRes();
        smarsa_CollisionOver(io.Gen()->GetPtr(), res, 1, 2097152, i, 4, 10);
        io.Add(td.GetId(), td.GetName(), res->Bas->pVal2[gofw_Mean]);
        smarsa_DeleteRes(res);
    };
}

TestCbFunc sspectral_Fourier3_cb(long N, int k, int r, int s)
{
    return [=] (const TestDescr& td, BatteryIO& io) {
        sspectral_Res* res = sspectral_CreateRes();
        sspectral_Fourier3(io.Gen()->GetPtr(), res, N, k, r, s);
        io.Add(td.GetId(), td.GetName(), res->Bas->pVal2[gofw_AD]);
        sspectral_DeleteRes(res);
    };
}


TestCbFunc sknuth_Gap_cb(long N, long n, int r, double Alpha, double Beta)
{
    return [=] (const TestDescr& td, BatteryIO& io) {
        sres_Chi2* res = sres_CreateChi2();
        sknuth_Gap(io.Gen()->GetPtr(), res, N, n, r, Alpha, Beta);
        io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_Mean]);
        sres_DeleteChi2(res);
    };
}


TestCbFunc smarsa_GCD_cb(long N, long n, int r, int s)
{
    return [=] (const TestDescr& td, BatteryIO& io) {
        smarsa_Res2* res = smarsa_CreateRes2();
        smarsa_GCD(io.Gen()->GetPtr(), res, N, n, r, s);
        if (N == 1)
            io.Add(td.GetId(), td.GetName(), res->GCD->pVal2[gofw_Mean]);
        else
            io.Add(td.GetId(), td.GetName(), res->GCD->pVal2[gofw_Sum]);
        smarsa_DeleteRes2(res);
    };
}


TestCbFunc sstring_HammingCorr_cb(long N, long n, int r, int s, int L)
{
    return [=] (const TestDescr& td, BatteryIO& io) {
        sstring_Res* res = sstring_CreateRes();
        sstring_HammingCorr(io.Gen()->GetPtr(), res, N, n, r, s, L);
        io.Add(td.GetId(), td.GetName(), res->Bas->pVal2[gofw_Mean]);
        sstring_DeleteRes(res);

    };
}


TestCbFunc sstring_HammingIndep_cb(long N, long n, int r, int s, int L, int d)
{
    return [=] (const TestDescr& td, BatteryIO& io) {
        sstring_Res* res = sstring_CreateRes();
        sstring_HammingIndep(io.Gen()->GetPtr(), res, N, n, r, s, L, d);
        if (N == 1)
            io.Add(td.GetId(), td.GetName(), res->Bas->pVal2[gofw_Mean]);
        else
            io.Add(td.GetId(), td.GetName(), res->Bas->pVal2[gofw_Sum]);
        sstring_DeleteRes(res);
    };
}


TestCbFunc sstring_HammingWeight2_cb(long N, long n, int r, int s, long L)
{
    return [=] (const TestDescr& td, BatteryIO& io) {
        sres_Basic* res = sres_CreateBasic();
        sstring_HammingWeight2(io.Gen()->GetPtr(), res, N, n, r, s, L);
        io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_Sum]);
        sres_DeleteBasic (res);
    };
}


TestCbFunc scomp_LempelZiv_cb(long N, int t, int r, int s)
{
    return [=] (const TestDescr& td, BatteryIO& io) {
        sres_Basic* res = sres_CreateBasic();
        scomp_LempelZiv(io.Gen()->GetPtr(), res, N, t, r, s);
        io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_Sum]);
        sres_DeleteBasic(res);
    };
}


TestCbFunc scomp_LinearComp_cb(long N, long n, int r, int s)
{
    return [=] (const TestDescr& td, BatteryIO& io) {
        scomp_Res* res = scomp_CreateRes();
        scomp_LinearComp(io.Gen()->GetPtr(), res, N, n, r, s);
        io.Add(td.GetId(), td.GetName(), res->JumpNum->pVal2[gofw_Mean]);
        io.Add(td.GetId(), td.GetName(), res->JumpSize->pVal2[gofw_Mean]);
        scomp_DeleteRes(res);
    };
}

TestCbFunc sstring_LongestHeadRun_cb(long N, long n, int r, int s, long L)
{
    return [=] (const TestDescr& td, BatteryIO& io) {
        sstring_Res2* res = sstring_CreateRes2();
        sstring_LongestHeadRun(io.Gen()->GetPtr(), res, N, n, r, s, L);
        io.Add(td.GetId(), td.GetName(), res->Chi->pVal2[gofw_Mean]);
        io.Add(td.GetId(), td.GetName(), res->Disc->pVal2);
        sstring_DeleteRes2(res);
    };
}


TestCbFunc smarsa_MatrixRank_cb(long N, long n, int r, int s, int L, int k)
{
    return [=] (const TestDescr& td, BatteryIO& io) {
        sres_Chi2* res = sres_CreateChi2();
        smarsa_MatrixRank(io.Gen()->GetPtr(), res, N, n, r, s, L, k);
        if (N == 1)
            io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_Mean]);
        else
            io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_Sum]);
        sres_DeleteChi2(res);
    };
}

TestCbFunc sknuth_MaxOft_cb(long N, long n, int r, int d, int t)
{
    return [=] (const TestDescr& td, BatteryIO& io) {
        gofw_TestType type_chi = gofw_Sum, type_bas = gofw_AD;
        if (N == 1) {
            type_chi = gofw_Mean;
            type_bas = gofw_Mean;
        }
        auto* res5 = sknuth_CreateRes1();
        sknuth_MaxOft(io.Gen()->GetPtr(), res5, N, n, r, d, t);
        io.Add(td.GetId(), td.GetName(), res5->Chi->pVal2[type_chi]);
        std::string ad_name = td.GetName();
        ad_name.replace(ad_name.find("MaxOft"), sizeof("MaxOft") - 1, "MaxOft AD");
        io.Add(td.GetId(), ad_name, res5->Bas->pVal2[type_bas]);
        sknuth_DeleteRes1(res5);        
    };
}


TestCbFunc smarsa_Opso_cb(long N, int r, int p)
{
    return [=] (const TestDescr& td, BatteryIO& io) {
        smarsa_Res* res = smarsa_CreateRes();
        smarsa_Opso(io.Gen()->GetPtr(), res, N, r, p);
        io.Add(td.GetId(), td.GetName(), res->Pois->pVal2);
        smarsa_DeleteRes(res);
    };
}

/**
 * @brief An envelope for smarsa_CollisionOver for pseudoDIEHARD battery.
 */
TestCbFunc smarsa_Oqso_cb(int i)
{
    return [=] (const TestDescr& td, BatteryIO& io) {
        printf("***********************************************************\n"
            "Test OQSO calling smarsa_CollisionOver\n\n");
        smarsa_Res* res = smarsa_CreateRes();
        smarsa_CollisionOver(io.Gen()->GetPtr(), res, 1, 2097152, i, 32, 4);
        io.Add(td.GetId(), td.GetName(), res->Bas->pVal2[gofw_Mean]);
        smarsa_DeleteRes(res);
    };
}



TestCbFunc sstring_PeriodsInStrings_cb(long N, long n, int r, int s)
{
    return [=] (const TestDescr& td, BatteryIO& io) {
        sres_Chi2* res = sres_CreateChi2();
        sstring_PeriodsInStrings(io.Gen()->GetPtr(), res, N, n, r, s);
        if (N == 1)        
            io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_Mean]);
        else
            io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_Sum]);
        sres_DeleteChi2 (res);
    };
}

TestCbFunc sknuth_Permutation_cb(long N, long n, int r, int t)
{
    return [=] (const TestDescr& td, BatteryIO& io) {
        sres_Chi2* res = sres_CreateChi2();
        sknuth_Permutation(io.Gen()->GetPtr(), res, N, n, r, t);
        io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_Mean]);
        sres_DeleteChi2(res);
    };
}


TestCbFunc smarsa_RandomWalk1_cb(long N, long n, int r, int s,
    long L0, long L1, const std::string &mess)
{
    return [=] (const TestDescr& td, BatteryIO& io) {
        auto* res = swalk_CreateRes();
        swalk_RandomWalk1(io.Gen()->GetPtr(), res, N, n, r, s, L0, L1);
        GetPValue_Walk(io, 1, res, td.GetId(), mess.c_str());
        swalk_DeleteRes(res);
    };
}

TestCbFunc sknuth_Run_cb(long N, long n, int r, bool Up)
{
    return [=] (const TestDescr& td, BatteryIO& io) {
        sres_Chi2* res = sres_CreateChi2();
        sknuth_Run(io.Gen()->GetPtr(), res, N, n, r, Up);
        io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_Sum]);
        sres_DeleteChi2(res);
    };
}


TestCbFunc sstring_Run_cb(long N, long n, int r, int s)
{
    return [=] (const TestDescr& td, BatteryIO& io) {
        sstring_Res3* res = sstring_CreateRes3();
        sstring_Run(io.Gen()->GetPtr(), res, N, n, r, s);
        io.Add(td.GetId(), td.GetName(), res->NRuns->pVal2[gofw_Mean]);
        io.Add(td.GetId(), td.GetName(), res->NBits->pVal2[gofw_Mean]);
        sstring_DeleteRes3 (res);
    };
}

TestCbFunc svaria_SampleCorr_cb(long N, long n, int r, int k)
{
    return [=] (const TestDescr& td, BatteryIO& io) {
        sres_Basic* res = sres_CreateBasic();
        svaria_SampleCorr(io.Gen()->GetPtr(), res, N, n, r, k);
        io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_Mean]);
        sres_DeleteBasic(res);
    };
}

TestCbFunc svaria_SampleProd_cb(long N, long n, int r, int t)
{
    return [=] (const TestDescr& td, BatteryIO& io) {
        sres_Basic *res = sres_CreateBasic();
        svaria_SampleProd(io.Gen()->GetPtr(), res, N, n, r, t);
        if (N > 1) // Derived from comparison of Crush and BigCrush
            io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_AD]);
        else
            io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_Mean]);
        sres_DeleteBasic(res);
    };
}

TestCbFunc svaria_SampleMean_cb(long N, long n, int r)
{
    return [=] (const TestDescr& td, BatteryIO& io) {
        sres_Basic* res = sres_CreateBasic();
        svaria_SampleMean(io.Gen()->GetPtr(), res, N, n, r);
        io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_AD]);
        sres_DeleteBasic(res);
    };
}

TestCbFunc smarsa_Savir2_cb(long N, long n, int r, long m, int t)
{
    return [=] (const TestDescr& td, BatteryIO& io) {
        auto* res = sres_CreateChi2();
        smarsa_Savir2(io.Gen()->GetPtr(), res, N, n, r, m, t);
        if (N == 1)
            io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_Mean]);
        else
            io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_Sum]);
        sres_DeleteChi2(res);
    };
}

TestCbFunc smarsa_SerialOver_cb(long N, long n, int r, long d, int t)
{
    return [=] (const TestDescr& td, BatteryIO& io) {
        sres_Basic* res = sres_CreateBasic();
        smarsa_SerialOver(io.Gen()->GetPtr(), res, N, n, r, d, t);
        io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_Mean]);
        sres_DeleteBasic (res);
    };
}


TestCbFunc sknuth_SimpPoker_cb(long N, long n, int r, int d, int k)
{
    return [=] (const TestDescr& td, BatteryIO& io) {
        sres_Chi2* res = sres_CreateChi2();
        sknuth_SimpPoker(io.Gen()->GetPtr(), res, N, n, r, d, k);
        io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_Mean]);
        sres_DeleteChi2(res);
    };
}

TestCbFunc svaria_SumCollector_cb(long N, long n, int r, double g)
{
    return [=] (const TestDescr& td, BatteryIO& io) {
        sres_Chi2* res = sres_CreateChi2();
        svaria_SumCollector(io.Gen()->GetPtr(), res, N, n, r, g);
        io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_Mean]);
        sres_DeleteChi2(res);
    };
}

TestCbFunc svaria_WeightDistrib_cb(long N, long n, int r, long k,
    double alpha, double beta)
{
    return [=] (const TestDescr& td, BatteryIO& io) {
        sres_Chi2* res = sres_CreateChi2();
        svaria_WeightDistrib(io.Gen()->GetPtr(), res, N, n, r, k, alpha, beta);
        io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_Mean]);
        sres_DeleteChi2(res);
    };
}


} // namespace testu01_threads
