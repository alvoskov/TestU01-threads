/**
 * @brief Multithreaded extension of TestU01 library.
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
    unif01_Gen gen;
    static void WrExternGen(void *junk2) { (void) junk2; }
    std::string name;    

    UniformGenerator(const UniformGenerator &obj) = delete;
    UniformGenerator &operator=(const UniformGenerator &obj) = delete;
    static double GetU01Handle(void *param, void *state);
    static unsigned long GetBits(void *param, void *state);
    
public:
    UniformGenerator(const std::string &name);
    unif01_Gen *GetPtr() const { return const_cast<unif01_Gen *>(&gen); }
    const std::string &GetName() { return name; }
    virtual double GetU01() = 0;
    virtual uint32_t GetBits() = 0;
};

/**
 * @brief Keeps the p value obtained for the test
 */
class PValueRecord
{
public:
    int id;
    std::string name;
    double pvalue;

    PValueRecord(int id_, const std::string &name_, double pvalue_)
    : id(id_), name(name_), pvalue(pvalue_) {}
    PValueRecord() : id(-1), name("-----"), pvalue(-1.0) {}

    friend bool operator<(const PValueRecord &a, const PValueRecord &b)
    {
        return a.id < b.id;
    }
};



class BatteryIO
{
    std::shared_ptr<UniformGenerator> gen;
    std::vector<PValueRecord> results;

public:
    BatteryIO(std::shared_ptr<UniformGenerator> gobj) : gen(gobj) {}
    inline unif01_Gen *Gen() const { return gen.get()->GetPtr(); }

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

void run_tests(std::vector<TestDescr> &tests,
    std::function<std::shared_ptr<UniformGenerator>()> create_gen,
    const std::string &battery_name);

TestCbFunc sstring_AutoCor_cb(long N, long n, int r, int s, int d);
TestCbFunc smarsa_BirthdaySpacings_cb(long N, long n, int r, long d, int t, int p);
TestCbFunc smarsa_CollisionOver_cb(long N, long n, int r, long d, int t);
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
TestCbFunc sstring_Run_cb(long N, long n, int r, int s);
TestCbFunc svaria_SampleProd_cb(long N, long n, int r, int t);
TestCbFunc svaria_SampleMean_cb(long N, long n, int r);
TestCbFunc smarsa_Savir2_cb(long N, long n, int r, long m, int t);
TestCbFunc smarsa_SerialOver_cb(long N, long n, int r, long d, int t);
TestCbFunc sknuth_SimpPoker_cb(long N, long n, int r, int d, int k);
TestCbFunc svaria_WeightDistrib_cb(long N, long n, int r, long k,
    double alpha, double beta);

#endif
