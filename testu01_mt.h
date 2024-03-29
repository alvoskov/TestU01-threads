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

extern "C" {
#include "unif01.h"
#include "config.h"
#include "gofw.h"
#include "bbattery.h"
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
#include "gdef.h"
}

#define THOUSAND 1000
#define MILLION (THOUSAND * THOUSAND)
#define BILLION (THOUSAND * MILLION)

#include "testu01_mt_cintf.h"
#include <string>
#include <functional>
#include <memory>
#include <vector>
#include <thread>
#include <mutex>
#include <algorithm>


/**
 * @brief Object-oriented envelope for TestU01 structures.
 * Allows to use RAII paradigm instead of manual calloc/free.
 */
class UniformGenerator
{
    static void WrExternGen(void *junk2) { (void) junk2; }
    std::string name;    

    UniformGenerator(const UniformGenerator &obj) = delete;
    UniformGenerator &operator=(const UniformGenerator &obj) = delete;
    static double GetU01Handle(void *param, void *state);
    static unsigned long GetBits(void *param, void *state);

protected:
    unif01_Gen gen;
    
public:
    UniformGenerator(const std::string &name);
    unif01_Gen *GetPtr() const { return const_cast<unif01_Gen *>(&gen); }
    const std::string &GetName() { return name; }
    virtual double GetU01() = 0;
    virtual uint32_t GetBits() = 0;
};



/**
 * @brief A variant of UniformGenerator that is designed as an interface
 * for C program.
 * @details The next functions should be supplied by the C module:
 * - `double get_u01(void *param, void *state)` - that returns uniformly
 *    distributed pseudorandom numbers from the [0;1) interval.
 * - `uint32_t get_bits32(void *param, void *state)` - that returns
 *    uniformly distributed 32-bit unsigned pseudorandom numbers.
 * - `void gen_delete(void *param, void *state)` - destroys the generator.
 */
class UniformGeneratorC : public UniformGenerator
{
    static void WrExternGen(void *junk2) { (void) junk2; }
    std::string name;
    DeleteStateCallbackC destroy;

    UniformGeneratorC(const UniformGeneratorC &obj) = delete;
    UniformGeneratorC &operator=(const UniformGeneratorC &obj) = delete;
    
public:
    UniformGeneratorC(const GenInfoC *gi);
    unif01_Gen *GetPtr() const { return const_cast<unif01_Gen *>(&gen); }
    const std::string &GetName() { return name; }
    double GetU01() override { return 0.0; }
    uint32_t GetBits() override { return 0; }
    virtual ~UniformGeneratorC() { destroy(gen.param, gen.state); }
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
    void WritePValue(double p);
    void WriteReport(const char *batName, const char *genName, chrono_Chrono *timer);
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
    inline int GetId() { return id; }
    inline const std::string &GetName() { return name; }
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

    void Run(std::function<std::shared_ptr<UniformGenerator>()> create_gen,
        const std::string &battery_name);
};


void run_tests(std::vector<TestDescr> &tests,
    std::function<std::shared_ptr<UniformGenerator>()> create_gen,
    const std::string &battery_name);

TestCbFunc svaria_AppearanceSpacings_cb(long N, long Q, long K, int r, int s, int L);
TestCbFunc sstring_AutoCor_cb(long N, long n, int r, int s, int d);
TestCbFunc smarsa_BirthdaySpacings_cb(long N, long n, int r, long d, int t, int p);
TestCbFunc smarsa_CollisionOver_cb(long N, long n, int r, long d, int t);
TestCbFunc sknuth_CollisionPermut_cb(long N, long n, int r, int t);
TestCbFunc sknuth_CouponCollector_cb(long N, long n, int r, int d);
TestCbFunc snpair_ClosePairs_cb(long N, long n, int r, int k, int p, int m, const std::string &mess, bool flag);
TestCbFunc snpair_ClosePairsBitMatch_cb(long N, long n, int r, int t);
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

#endif
