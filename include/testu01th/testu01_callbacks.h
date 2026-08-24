/**
 * @file testu01_callbacks.h
 * @brief Some lower-level code that wraps some internal structures/functions
 * of TestU01 library. Hides TestU01 internals (header files) from
 * TestU01-threads users.
 *
 * @copyright
 * (c) 2024-2026 Alexey L. Voskov, Lomonosov Moscow State University.
 * alvoskov@gmail.com
 *
 * (c) 2002 Pierre L'Ecuyer, DIRO, Université de Montréal.
 * e-mail: lecuyer@iro.umontreal.ca
 *
 * All rights reserved.
 *
 * This software is provided under the Apache 2 License.
 *
 * In scientific publications which used this software, a reference to it
 * would be appreciated.
 */
#ifndef __TESTU01_CALLBACKS_H
#define __TESTU01_CALLBACKS_H
#include "basictypes.h"

extern "C" {
#include "unif01.h"
#include "gdef.h"
#ifndef PACKAGE_STRING
#include "config.h"
#endif
#include "bbattery.h"
#include "fbar.h"
#include "gofw.h"
#include "gofs.h"
#include "smultin.h"
#include "sknuth.h"
#include "smarsa.h"
#include "snpair.h"
#include "svaria.h"
#include "sstring.h"
#include "swalk.h"
#include "scomp.h"
#include "sspectral.h"
#include "swrite.h"
#include "sres.h"
#include "num.h"
#include "unif01.h"
#include "ufile.h"
}


#include <string>
#include <functional>
#include <memory>

namespace testu01_threads {

static constexpr long THOUSAND{1000};
static constexpr long MILLION{THOUSAND * THOUSAND};
static constexpr long BILLION{THOUSAND * MILLION};

/**
 * @brief Needed only to make forward declaration (for pointers) possible.
 */
class Unif01GenWrapper {
public:
    unif01_Gen gen;
    inline unif01_Gen* GetPtr() { return &gen; }
};

void init_TestU01_internals(bool verbose = false);

TestCbFunc svaria_AppearanceSpacings_cb(long N, long Q, long K, int r, int s, int L);
TestCbFunc sstring_AutoCor_cb(long N, long n, int r, int s, int d);
TestCbFunc smarsa_BirthdaySpacings_cb(long N, long n, int r, long d, int t, int p);
TestCbFunc smarsa_CollisionOver_cb(long N, long n, int r, long d, int t);
TestCbFunc sknuth_CollisionPermut_cb(long N, long n, int r, int t);
TestCbFunc sknuth_CouponCollector_cb(long N, long n, int r, int d);
TestCbFunc snpair_ClosePairs_cb(long N, long n, int r, int k, int p, int m,
    const std::string& mess, bool flag);
TestCbFunc snpair_ClosePairsNP_cb(long N, long n, int r, int k, int p, int m);
TestCbFunc snpair_ClosePairsBitMatch_cb(long N, long n, int r, int t);
TestCbFunc smarsa_Dna_cb(int i);
TestCbFunc sspectral_Fourier3_cb(long N, int k, int r, int s);
TestCbFunc sknuth_Gap_cb(long N, long n, int r, double Alpha, double Beta);
TestCbFunc smarsa_GCD_cb(long N, long n, int r, int s);
TestCbFunc sstring_HammingCorr_cb(long N, long n, int r, int s, int L);
TestCbFunc sstring_HammingIndep_cb(long N, long n, int r, int s, int L, int d);
TestCbFunc sstring_HammingWeight2_cb(long N, long n, int r, int s, long L);
TestCbFunc scomp_LempelZiv_cb(long N, int t, int r, int s);
TestCbFunc scomp_LinearComp_cb(long N, long n, int r, int s);
TestCbFunc sstring_LongestHeadRun_cb(long N, long n, int r, int s, long L);
TestCbFunc smarsa_MatrixRank_cb(long N, long n, int r, int s, int L, int k);
TestCbFunc sknuth_MaxOft_cb(long N, long n, int r, int d, int t);
TestCbFunc smarsa_Opso_cb(long N, int r, int p);
TestCbFunc smarsa_Oqso_cb(int i);
TestCbFunc sstring_PeriodsInStrings_cb(long N, long n, int r, int s);
TestCbFunc sknuth_Permutation_cb(long N, long n, int r, int t);
TestCbFunc smarsa_RandomWalk1_cb(long N, long n, int r, int s,
    long L0, long L1, const std::string &mess);
TestCbFunc sknuth_Run_cb(long N, long n, int r, bool Up);
TestCbFunc sstring_Run_cb(long N, long n, int r, int s);
TestCbFunc svaria_SampleCorr_cb(long N, long n, int r, int k);
TestCbFunc svaria_SampleProd_cb(long N, long n, int r, int t);
TestCbFunc svaria_SampleMean_cb(long N, long n, int r);
TestCbFunc smarsa_Savir2_cb(long N, long n, int r, long m, int t);
TestCbFunc smarsa_SerialOver_cb(long N, long n, int r, long d, int t);
TestCbFunc sknuth_SimpPoker_cb(long N, long n, int r, int d, int k);
TestCbFunc svaria_SumCollector_cb(long N, long n, int r, double g);
TestCbFunc svaria_WeightDistrib_cb(long N, long n, int r, long k,
    double alpha, double beta);

} // namespace testu01_threads

#endif // __TESTU01_CALLBACKS_H
