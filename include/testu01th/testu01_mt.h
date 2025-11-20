/**
 * @file testu01_mt.h
 * @brief A multithreaded extension of TestU01 library. Partially based on its
 * source code, especiall on the `bbattery.c` file and some other header files.
 * @copyright (c) 2024 Alexey L. Voskov, Lomonosov Moscow State University.
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
#ifndef __TESTU01_MT_H
#define __TESTU01_MT_H

#include <cstddef>
#include <cstdint>


/**
 * @brief Block size (in elements) for vectorized PRNG calls.
 */
constexpr size_t ELEMENTS_PER_BLOCK = 1024;

#if defined(__GNUC__) && defined(__x86_64__)
#include <x86intrin.h>
#undef STDC_HEADERS
#endif

#include <stdint.h>
#include <time.h>
#include "entropy.h"
#include <string>
#include <functional>
#include <memory>
#include <vector>
#include <thread>
#include <mutex>
#include <algorithm>

#include "testu01_mt_intf.h"

namespace testu01_threads {

/**
 * @brief Always returns 0, has no internal state.
 */
class DummyGenerator : public UniformGenerator
{
public:
    DummyGenerator() : UniformGenerator("Dummy") {}
    double GetU01() override { return 0.0; }
    uint32_t GetBits32() override { return 0; }
};



/**
 * @brief Keeps the p value obtained for the test. Supports comparison
 * operator `<` that is important for `std::sort`.
 */
class PValueRecord
{
public:
    int id; ///< Test ID (several tests may have the same ID)
    std::string name; ///< Test name.
    double pvalue; ///< The obtained p-value.

    PValueRecord(int id_, const std::string &name_, double pvalue_)
    : id(id_), name(name_), pvalue(pvalue_) {}
    PValueRecord() : id(-1), name("-----"), pvalue(-1.0) {}

    friend bool operator<(const PValueRecord &a, const PValueRecord &b)
    {
        return a.id < b.id;
    }
};


/**
 * @brief The class keeps the shared pointer to the used PRNG
 * and allows to store the results of statistical tests. It is
 * not thread safe and each thread should use its own example
 * of BatteryIO.
 */
class BatteryIO
{
    std::shared_ptr<UniformGenerator> gen; ///< The used PRNG.
    std::vector<PValueRecord> results; ///< The stored results.

public:
    BatteryIO(std::shared_ptr<UniformGenerator> gobj) : gen(gobj) {}
    inline unif01_Gen *Gen() const { return gen.get()->GetPtr(); }

    /**
     * @brief Adds the result of statistical test to the battery.
     * @param id     Test id (may be the same for several tests)
     * @param name   Test name
     * @param pvalue The obtained p-value.
     */
    inline void Add(int id, const std::string &name, double pvalue)
    {
        results.emplace_back(id, name, pvalue);
    }

    void Add(const BatteryIO &obj);
    size_t GetNTestsFailed() const;
    inline size_t GetNResults() const { return results.size(); }
    inline const PValueRecord &GetPValueRecord(size_t ind) { return results[ind]; }
    std::string WritePValue(double p);
    std::string WriteReport(const char *batName, const char *genName,
        chrono_Chrono *timer, size_t ms_total);
};


/**
 * @brief Array of p-values obtained from different tests from all threads
 * + TestU01 report.
 */
class BatteryResults
{
public:
    std::vector<std::vector<PValueRecord>> pvalues; ///< results[thread][test_ind]
    std::string report;

    BatteryResults() {}
    BatteryResults(size_t nthreads) : pvalues(nthreads) {}
    std::string ToString() const;
};



class TestDescr;

/**
 * @brief Callback function that runs the test and saves its
 * result in BatteryIO class using the test description from
 * TestDescr.
 */
typedef std::function<void(TestDescr &, BatteryIO &)> TestCbFunc;

/**
 * @brief Function that returns the `std::shared_ptr` smart pointer
 * to the initialized pseudorandom number generator.
 */
typedef std::function<std::shared_ptr<UniformGenerator>()> GenFactoryFunc;


class TestDescr
{
    int id;
    std::string name;
    std::function<void (TestDescr &td, BatteryIO &io)> pvalue_func;

public:
    inline int GetId() const { return id; }
    inline const std::string &GetName() const { return name; }
    inline void Run(BatteryIO &io) { pvalue_func(*this, io); }

    TestDescr(int testid, const std::string &testname, TestCbFunc f)
    : id(testid),
        name(testname), pvalue_func(f)
    {
    }
};




class TestsPull
{
    std::vector<TestDescr> tests;
    std::mutex get_mutex;
    size_t pos;

    size_t GetNThreads() const;
    static void ThreadFunc(TestsPull &pull, BatteryIO &io, int thread_id);


public:
    TestsPull() {}
    TestsPull(const std::vector<TestDescr> &obj);
    const TestDescr *Get(std::string &pos_msg);

    BatteryResults Run(std::function<std::shared_ptr<UniformGenerator>()> create_gen,
        const std::string &battery_name);
};



/**
 * @brief Generic class for tests batteries such as SmallCrush, Crush
 * or BigCrush.
 */
class TestsBattery
{
protected:
    std::vector<TestDescr> tests;
    GenFactoryFunc create_gen;
    std::string battery_name;
    std::string generator_name;

public:
    TestsBattery(GenFactoryFunc genf);
    BatteryResults Run() const;
    BatteryResults RunTest(int id) const;
};


void set_bin_stdout();
void set_bin_stdin();
void prng_bits32_to_file(std::shared_ptr<UniformGenerator> genptr);
void prng_array32_to_file(std::shared_ptr<UniformGenerator> genptr);
void prng_bits64_to_file(std::shared_ptr<UniformGenerator> genptr);
void prng_array64_to_file(std::shared_ptr<UniformGenerator> genptr);

TestCbFunc svaria_AppearanceSpacings_cb(long N, long Q, long K, int r, int s, int L);
TestCbFunc sstring_AutoCor_cb(long N, long n, int r, int s, int d);
TestCbFunc smarsa_BirthdaySpacings_cb(long N, long n, int r, long d, int t, int p);
TestCbFunc smarsa_CollisionOver_cb(long N, long n, int r, long d, int t);
TestCbFunc sknuth_CollisionPermut_cb(long N, long n, int r, int t);
TestCbFunc sknuth_CouponCollector_cb(long N, long n, int r, int d);
TestCbFunc snpair_ClosePairs_cb(long N, long n, int r, int k, int p, int m, const std::string &mess, bool flag);
TestCbFunc snpair_ClosePairsNP_cb(long N, long n, int r, int k, int p, int m);
TestCbFunc snpair_ClosePairsBitMatch_cb(long N, long n, int r, int t);
TestCbFunc smarsa_Dna_cb(int i);
TestCbFunc sspectral_Fourier3_cb(long N, int k, int r, int s);
TestCbFunc sknuth_Gap_cb(long N, long n, int r, double Alpha, double Beta);
TestCbFunc smarsa_GCD_cb(long N, long n, int r, int s);
TestCbFunc sstring_HammingCorr_cb(long N, long n, int r, int s, int L);
TestCbFunc sstring_HammingIndep_cb(long N, long n, int r, int s, int L, int d);
TestCbFunc sstring_HammingWeight2_cb(long N, int r, int s, long L, long K);
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

#endif
