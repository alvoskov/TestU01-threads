/**
 * @file testu01_mt.h
 * @brief A multithreaded extension of TestU01 library. Partially based on its
 * source code, especially on the `bbattery.c` file and some other header files.
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
#ifndef __TESTU01_MT_H
#define __TESTU01_MT_H

#include "basictypes.h"
#include <cstddef>
#include <stddef.h>
#include <cstdint>
#include <ctime>
#include <string>
#include <functional>
#include <memory>
#include <vector>
#include <thread>
#include <mutex>
#include <algorithm>
#include <random>
#include <chrono>
#include <limits>
#include <ostream>

namespace testu01_threads {

/**
 * @brief Object-oriented envelope for TestU01 structures.
 * Allows to use RAII paradigm instead of manual calloc/free.
 */
class UniformGenerator
{
    static void WrExternGen(void* junk2) { (void) junk2; }
    std::string name;    

    UniformGenerator(const UniformGenerator& obj) = delete;
    UniformGenerator& operator=(const UniformGenerator& obj) = delete;
    static double GetU01Handle(void* param, void* state);
    static unsigned long GetBits32Handle(void* param, void* state);

protected:
    std::unique_ptr<Unif01GenWrapper> gen_wrapped; ///< Pointer to wrapped unif01_Gen
    
public:
    UniformGenerator(const std::string& name);
    virtual ~UniformGenerator();
    Unif01GenWrapper* GetPtr() const { return gen_wrapped.get(); }
    const std::string& GetName() { return name; }
    virtual double GetU01() = 0;
    virtual std::uint32_t GetBits32() = 0;
};

/**
 * @brief Function that returns the `std::shared_ptr` smart pointer
 * to the initialized pseudorandom number generator.
 */
using GenFactoryFunc = std::function<std::shared_ptr<UniformGenerator>()>;

/**
 * @brief Always returns 0, has no internal state.
 */
class DummyGenerator : public UniformGenerator
{
public:
    DummyGenerator() : UniformGenerator{"Dummy"} {}
    double GetU01() override { return 0.0; }
    std::uint32_t GetBits32() override { return 0; }
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

    PValueRecord(int id_, const std::string& name_, double pvalue_)
    : id{id_}, name{name_}, pvalue{pvalue_} {}
    PValueRecord() : id{-1}, name{"-----"}, pvalue{-1.0} {}

    friend bool operator<(const PValueRecord& a, const PValueRecord& b)
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
    inline Unif01GenWrapper* Gen() const { return gen.get()->GetPtr(); }
    std::uint64_t GetResultsChecksum() const;

    /**
     * @brief Adds the result of statistical test to the battery.
     * @param id     Test id (may be the same for several tests)
     * @param name   Test name
     * @param pvalue The obtained p-value.
     */
    inline void Add(int id, const std::string& name, double pvalue)
    {
        results.emplace_back(id, name, pvalue);
    }

    void Add(const BatteryIO& obj);
    size_t GetNTestsFailed() const;
    inline size_t GetNResults() const { return results.size(); }
    inline const PValueRecord& GetPValueRecord(size_t ind) { return results[ind]; }
    std::string WritePValue(double p);
    std::string WriteReport(const char* batName, const char* genName,
        std::chrono::milliseconds ms_cpu_total, std::chrono::milliseconds ms_total);
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
    BatteryResults(unsigned int nthreads) : pvalues{nthreads} {}
    std::string GetThreadsLoadingReport() const;
    std::string ToString() const;
};


static inline std::ostream& operator<<(std::ostream& os, const BatteryResults& obj)
{
    os << obj.ToString();
    return os;
}


class TestDescr
{
    int id;
    std::string name;
    std::function<void (const TestDescr& td, BatteryIO& io)> pvalue_func;

public:
    inline int GetId() const { return id; }
    inline const std::string& GetName() const { return name; }
    inline void Run(BatteryIO& io) const { pvalue_func(*this, io); }

    TestDescr(int testid, const std::string& testname, TestCbFunc f)
    : id{testid}, name{testname}, pvalue_func{f}
    {
    }
};


/**
 * @brief Used to pass the current thread index together with the current
 * number of threads to the thread function (useful for diagnostic messages)
 */
class ThreadIndex
{
public:
    unsigned int index;
    unsigned int nthreads;
};

/**
 * @brief Implementation of the multithreaded run of TestU01 batteries.
 */
class TestsPull
{
    std::vector<TestDescr> tests;
    std::mutex get_mutex;
    size_t pos{0};

    unsigned int GetNThreads() const;
    std::string GetPosMessage();
    static void ThreadFunc(TestsPull& pull, BatteryIO& io,
        const std::vector<size_t>& tests_inds, const ThreadIndex& thr_ind);

public:
    TestsPull() {}
    TestsPull(const std::vector<TestDescr>& obj, std::seed_seq* seq);

    BatteryResults Run(std::function<std::shared_ptr<UniformGenerator>()> create_gen,
        const std::string& battery_name, unsigned int nthreads = NTHREADS_DEFAULT);
};



/**
 * @brief Generic class for tests batteries such as SmallCrush, Crush
 * or BigCrush.
 */
class TestsBattery
{
protected:
    std::vector<TestDescr> tests;
    std::string battery_name;
    GenFactoryFunc create_gen;

public:
    TestsBattery(const std::string& bat_name, GenFactoryFunc genf);
    BatteryResults Run(std::seed_seq* seq = nullptr, unsigned int nthreads = NTHREADS_DEFAULT) const;
    BatteryResults RunTest(int id, std::seed_seq* seq = nullptr, unsigned int nthreads = NTHREADS_DEFAULT) const;
};


void set_bin_stdout();
void set_bin_stdin();
void prng_bits32_to_file(std::shared_ptr<UniformGenerator> genptr);

} // namespace testu01_threads


namespace testu01_threads::original::battery {
    void SmallCrush(UniformGenerator& gen);
    void Crush(UniformGenerator& gen);
    void BigCrush(UniformGenerator& gen);
    void pseudoDIEHARD(UniformGenerator& gen);
} // namespace testu01_threads::original::battery


#endif // __TESTU01_MT_H
