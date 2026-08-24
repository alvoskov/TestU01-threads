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

#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64) || defined(__MINGW32__) || defined(__MINGW64__)
#define ENABLE_BIN_STDIN
#endif

#include "testu01th/testu01_mt.h"
#include "testu01th/testu01_callbacks.h"
#include <iostream>
#include <cstring>
#include <cmath>
#include <random>
#include <sstream>
#ifdef ENABLE_BIN_STDIN
#include <io.h>
#endif
#include <fcntl.h>
#include <stdarg.h>

using namespace testu01_threads;

static std::string printf_tos(const char* format, ...)
{
    char buffer[256];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, 256, format, args);
    va_end(args);
    return std::string(buffer);
}

/**
 * @brief C++ version of num_writeD function. It writes to string, not
 * to stdout.
 * @param x  Input value to be printed
 * @param I  Minimal width in characters
 * @param J  Number of digits after . (%f format, without exponent)
 * @param K  Number of significant digits in scientific notation
 */
std::string double_tostring(double x, int I, int J, int K)
{
    std::stringstream sout;
    int PosEntier = 0; // Number of positions occupied by x integer part (%f format)
    int EntierSign; // 0-based position of . in string (%f format)
    int Neg = 0;    // Number is negative

    if (x == 0.0) {
        EntierSign = 1;
    } else {
        EntierSign = PosEntier = static_cast<int>(std::floor(std::log10(std::fabs(x)) + 1.0));
        if (x < 0.0)
            Neg = 1;
    }
    if (EntierSign <= 0) {
        PosEntier = 1;
    }

    if ((x == 0.0) ||
        (((EntierSign + J) >= K) && (I >= (PosEntier + J + Neg + 1)))) {
        sout << printf_tos("%*.*f", I, J, x);
    } else { // Use scientific (exponential) notation
        char S[100];
        std::snprintf(S, 100, "%*.*e", I, K - 1, x);
        char* p = std::strstr(S, "e+0");
        if (p == nullptr) {
            p = std::strstr(S, "e-0");
        }
        // Remove the 0 in e-0 and in e+0
        if (p) {
            p += 2;
            while ((*p = *(p + 1)))
	            p++;
            sout << " "; // Output must have at least 'I' spaces
        }
        sout << std::string(S);
    }
    return sout.str();
}


/**
 * @brief Prints the significance level of a test, without a descriptor
 */
std::string p0_tostring(double p)
{
    if ((p >= 0.01) && (p <= 0.99)) {
        return double_tostring(p, 8, 2, 1);
    } else if (p < gofw_Epsilonp) {
        return "   eps  ";
    } else if (p < 0.01) {
        return double_tostring(p, 8, 2, 2);
   } else if (p >= 1.0 - gofw_Epsilonp1) {
        return " 1 - eps1";
   } else if (p < 1.0 - 1.0e-4) {
        return printf_tos("    %.4f", p);
   } else {
        return " 1 - " + double_tostring(1.0 - p, 7, 2, 2);
   }
}


std::string chrono_tostring(chrono_Chrono* C, chrono_TimeFormat Form)
{
    double temps;
    if (Form != chrono_hms) {
        temps = chrono_Val(C, Form);
    } else {
        temps = 0.0;
    }
    switch (Form) {
    case chrono_sec:
        return double_tostring(temps, 10, 2, 1) + " seconds";

    case chrono_min:
        return double_tostring(temps, 10, 2, 1) + " minutes";

    case chrono_hours:
        return double_tostring(temps, 10, 2, 1) + " hours";

    case chrono_days:
        return double_tostring(temps, 10, 2, 1) + " days";

    case chrono_hms:
        {
        constexpr double hours_in_sec{1.0 / 3600.0};
        constexpr double mins_in_sec{1.0 / 60.0};
        temps = chrono_Val(C, chrono_sec);
        const long hour{static_cast<long>(temps * hours_in_sec)};
        if (hour > 0)
            temps -= static_cast<double>(hour) * 3600.0;
        const long minute{static_cast<long>(temps * mins_in_sec)};
        if (minute > 0)
            temps -= static_cast<double>(minute) * 60.0;
        const long second{static_cast<long>(temps)};
        const long centisecond{static_cast<long>(100.0 * (temps - static_cast<double>(second)))};
        return printf_tos("%02ld:%02ld:%02ld.%02ld",
            hour, minute, second, centisecond);
        }

    default:
        return "";
    }
}

std::chrono::milliseconds chrono_to_ms(chrono_Chrono *C)
{
    const double ms_dbl{chrono_Val(C, chrono_sec) * 1000.0};
    std::chrono::milliseconds ms(static_cast<unsigned long long>(ms_dbl));
    return ms;    
}


/////////////////////////////////////////////////
///// UniformGenerator class implementation /////
/////////////////////////////////////////////////

double UniformGenerator::GetU01Handle(void* param, void* state)
{
    (void) param;
    UniformGenerator* obj{static_cast<UniformGenerator*>(state)};
    return obj->GetU01();
}

unsigned long UniformGenerator::GetBits32Handle(void* param, void* state)
{
    (void) param;
    UniformGenerator* obj{static_cast<UniformGenerator*>(state)};
    return obj->GetBits32();
}


UniformGenerator::UniformGenerator(const std::string& name_)
    : name{name_}, gen_wrapped{std::make_unique<Unif01GenWrapper>()}
{
    unif01_Gen* gen = gen_wrapped.get()->GetPtr();
    gen->state = static_cast<void*>(this);
    gen->param = nullptr;
    gen->Write = WrExternGen;
    gen->GetU01 = GetU01Handle;
    gen->GetBits = GetBits32Handle;
    gen->name = const_cast<char*>(name.c_str());
}


UniformGenerator::~UniformGenerator()
{
}

//////////////////////////////////////////
///// BatteryIO class implementation /////
//////////////////////////////////////////


size_t BatteryIO::GetNTestsFailed() const
{
    size_t co{0};
    for (const auto& r : results) {
        if ((r.pvalue < gofw_Suspectp) || (r.pvalue > 1.0 - gofw_Suspectp)) {
            co++;
        }
    }
    return co;
}

/**
 * @brief Returns a non-cryptographic checksum of p-values. May be useful
 * for reproducibility testing, especially in a multithreaded environment.
 * @details The algorithm is partially based on the djb2 algorithm.
 */
std::uint64_t BatteryIO::GetResultsChecksum() const
{
    std::uint64_t sum{0x123456789ABCDEF};
    for (const auto& r : results) {
        int pvalue_exp;
        std::uint64_t pvalue_x64{static_cast<uint64_t>(std::frexp(r.pvalue, &pvalue_exp) * 16777216U)};
        pvalue_x64 |= (static_cast<uint64_t>(pvalue_exp) + 32768U) << 32;
        const std::uint64_t inc{(static_cast<uint64_t>(r.id) << 48) | pvalue_x64};
        sum = 6906969069U * sum + inc;
    }    
    return sum;
}


/**
 * @brief Write a p-value with a nice format.
 */
std::string BatteryIO::WritePValue(double p)
{
    std::stringstream sout;
    if (p < gofw_Suspectp) {
        sout << p0_tostring(p);
    } else if (p > 1.0 - gofw_Suspectp) {
        if (p >= 1.0 - gofw_Epsilonp1) {
            sout << " 1 - eps1";
        } else if (p >= 1.0 - 1.0e-4) {
            sout << " 1 - "
                 << double_tostring(1.0 - p, 7, 2, 2);
        } else if (p >= 1.0 - 1.0e-2) {
            sout << printf_tos("  %.4f ", p);
        } else {
            sout << printf_tos("   %.2f", p);
        }
    }
    return sout.str();
}

void BatteryIO::Add(const BatteryIO& obj)
{
    for (const auto& o : obj.results) {
        results.push_back(o);
    }
    std::sort(results.begin(), results.end());
}

/**
 * @brief Convert milliseconds to the hh:mm:ss.msec text format.
 */
static std::string ms_to_hms(std::chrono::milliseconds ms_total)
{
    const long long ms_num{static_cast<long long>(ms_total.count())};
    const auto ms{ms_num % 1000};
    const auto s{(ms_num / 1000) % 60};
    const auto m{(ms_num / 60000) % 60};
    const auto h{ms_num / 3600000};
    return printf_tos("%.2lld:%.2lld:%.2lld.%.3lld", h, m, s, ms);
}


/**
 * @brief Generate battery run report and return it as a string
 * @param bat_name      Battery name.
 * @param gen_name      Generator name.
 * @param ms_cpu_total  TestU01 timer that calculated CPU time for all cores.
 * @param ms_total      Elapsed time, milliseconds.
 * @return Battery run report (ASCII string).
 */
std::string BatteryIO::WriteReport(const char* bat_name, const char* gen_name,
    std::chrono::milliseconds ms_cpu_total, std::chrono::milliseconds ms_total)
{
    std::stringstream sout;
    sout << printf_tos("\n========= Summary results of %s", bat_name)
         << printf_tos(" =========\n\n")
         << printf_tos(" Version:                      %s\n", PACKAGE_STRING)
         << printf_tos(" Generator:                    ") << gen_name
         << printf_tos("\n Number of statistics:         %1u\n",
                static_cast<unsigned int>(results.size()))
         << printf_tos(" Total CPU time (all cores):   ")
         << ms_to_hms(ms_cpu_total)
         << printf_tos("\n Elapsed time:                 ")
         << ms_to_hms(ms_total)
         << printf_tos("\n p-values checksum:            0x%llX",
            (unsigned long long) GetResultsChecksum());

    if (GetNTestsFailed() == 0) {
        sout << printf_tos("\n\n All tests were passed\n\n\n\n");
        return sout.str();
    }

    if (gofw_Suspectp >= 0.01) {
        sout << printf_tos("\n The following tests gave p-values outside [%.4g, %.2f]",
            gofw_Suspectp, 1.0 - gofw_Suspectp);
    } else if (gofw_Suspectp >= 0.0001) {
        sout << printf_tos("\n The following tests gave p-values outside [%.4g, %.4f]",
            gofw_Suspectp, 1.0 - gofw_Suspectp);
    } else if (gofw_Suspectp >= 0.000001) {
        sout << printf_tos("\n The following tests gave p-values outside [%.4g, %.6f]",
            gofw_Suspectp, 1.0 - gofw_Suspectp);
    } else {
        sout << printf_tos("\n The following tests gave p-values outside [%.4g, %.14f]",
            gofw_Suspectp, 1.0 - gofw_Suspectp);
    }
    sout << printf_tos(":\n (eps  means a value < %6.1e)", gofw_Epsilonp)
         << printf_tos(":\n (eps1 means a value < %6.1e)", gofw_Epsilonp1)
         << printf_tos(":\n\n       Test                          p-value\n")
         << printf_tos(" ----------------------------------------------\n");

    for (auto& r : results) {
        if ((r.pvalue >= gofw_Suspectp) && (r.pvalue <= 1.0 - gofw_Suspectp))
            continue; // That test was passed
        sout << printf_tos(" %2d ", r.id)
             << printf_tos(" %-30s", r.name.c_str())
             << WritePValue(r.pvalue) << "\n";
    }

    sout << " ----------------------------------------------\n"
         << " All other tests were passed\n"
         << "\n\n\n";
    return sout.str();
}


///////////////////////////////////////////////
///// BatteryResults class implementation /////
///////////////////////////////////////////////


std::string BatteryResults::GetThreadsLoadingReport() const
{
    std::stringstream sout;
    for (size_t i = 0; i < pvalues.size(); i++) {
        sout << "===== Tests for thread " << i << " =====\n";
        for (const auto& rec : pvalues[i]) {
            sout << printf_tos("  %5d %32s %.6g\n",
                static_cast<int>(rec.id), rec.name.c_str(), rec.pvalue);
        }
    }
    return sout.str();
}

std::string BatteryResults::ToString() const
{
    return GetThreadsLoadingReport() + report;
}


//////////////////////////////////////////
///// TestsPull class implementation /////
//////////////////////////////////////////

TestsPull::TestsPull(const std::vector<TestDescr>& obj, std::seed_seq* seq)
{
    const size_t len{obj.size()};
    std::vector<size_t> tests_inds(len);
    for (size_t i = 0; i < len; i++) {
        tests_inds[i] = i;
    }
    if (seq == nullptr) {
        std::random_device rd;
        std::mt19937 prng(rd()); 
        std::shuffle(tests_inds.begin(), tests_inds.end(), prng);
    } else {
        std::mt19937 prng(*seq); 
        std::shuffle(tests_inds.begin(), tests_inds.end(), prng);
    }
    for (auto ind : tests_inds) {
        tests.push_back(obj[ind]);
    }
}

/**
 * @brief Returns the `test _ of _` message. Uses a counter controlled
 * by a mutex.
 */
std::string TestsPull::GetPosMessage()
{
    std::lock_guard<std::mutex> lock(get_mutex);
    if (pos < tests.size()) {
        pos++;
        return "test " + std::to_string(pos) +
            " of " + std::to_string(tests.size());
    } else {
        return "NONE";
    }
}

/**
 * @brief Get the number of threads optimal for the hardware.
 */
unsigned int TestsPull::GetNThreads() const
{
    const size_t ntests{tests.size()};
    unsigned int nthreads{std::thread::hardware_concurrency()};
    while (nthreads > ntests)
        nthreads /= 2;
    return nthreads;
}


void TestsPull::ThreadFunc(TestsPull& pull, BatteryIO& io,
    const std::vector<size_t>& tests_inds, const ThreadIndex& thr_ind)
{
    fprintf(stderr, "vvvvvvvvvv  Thread #%u started  vvvvvvvvvv\n", thr_ind.index);
    for (auto test_ind : tests_inds) {
        const TestDescr& t = pull.tests[test_ind];
        const std::string pos_msg = pull.GetPosMessage();
        fprintf(stderr, "vvvvv  Thread #%u: test %s started (%s)\n",
            thr_ind.index, t.GetName().c_str(), pos_msg.c_str());
        const size_t ind1{io.GetNResults()};
        t.Run(io);
        const size_t ind2{io.GetNResults()};
        fprintf(stderr, "^^^^^  Thread #%u: test %s finished (%s)",
            thr_ind.index, t.GetName().c_str(), pos_msg.c_str());
        if (ind2 > ind1) {
            fprintf(stderr, "; p = [");
            for (size_t i = ind1; i < ind2; i++) {
                fprintf(stderr, "%g ", io.GetPValueRecord(i).pvalue);
            }
            fprintf(stderr, "]\n");
        } else {
            fprintf(stderr, "\n");
        }
    }
    fprintf(stderr, "^^^^^^^^^^  Thread #%u finished  ^^^^^^^^^^\n", thr_ind.index);
}


BatteryResults TestsPull::Run(std::function<std::shared_ptr<UniformGenerator>()> create_gen,
    const std::string& battery_name, unsigned int nthreads)
{
    // Timers and threads number
    chrono_Chrono* timer = chrono_Create();
    if (nthreads == NTHREADS_DEFAULT) {
        nthreads = GetNThreads();
    }
    fprintf(stderr, "=====> Number of threads: %u\n", nthreads);
    BatteryResults results(nthreads);
    std::vector<BatteryIO> threads_bats;
    for (unsigned int i = 0; i < nthreads; i++) {
        threads_bats.emplace_back(create_gen());
    }
    // Disable thread unsafe features of TestU01
    init_TestU01_internals();
    // Create tests indexes arrays for each thread
    std::vector<std::vector<size_t>> thrd_testinds(nthreads);
    for (size_t i = 0; i < tests.size(); i++) {
        thrd_testinds[i % nthreads].push_back(i);
    }
    for (unsigned int i = 0; i < nthreads; i++) {
        const auto& testinds = thrd_testinds[i];
        fprintf(stderr, "=====> Thread %u tests: ", i);
        for (const auto tind : testinds) {
            fprintf(stderr, "%d(%s) ",
                tests[tind].GetId(),
                tests[tind].GetName().c_str()
            );
        }
        fprintf(stderr, "\n");
    }
    // Multi-threaded run
    const auto tic = std::chrono::high_resolution_clock::now();
    std::vector<std::thread> threads;
    for (size_t i = 0; i < nthreads; i++) {
        const ThreadIndex thr_ind{static_cast<unsigned int>(i), nthreads};
        threads.emplace_back(ThreadFunc,
            std::ref(*this),
            std::ref(threads_bats[i]),
            std::ref(thrd_testinds[i]),
            thr_ind);
    }
    for (auto& th : threads) {
        th.join();
    }
    // Save p-values from different threads to output array
    // (it preserves an exact order of calls).
    for (size_t i = 0; i < threads_bats.size(); i++) {
        for (size_t j = 0; j < threads_bats[i].GetNResults(); j++) {
            results.pvalues[i].push_back(threads_bats[i].GetPValueRecord(j));
        }
    }
    // Merge results from different threads.
    const char* gen_name;
    if (nthreads > 0) {
        gen_name = threads_bats[0].Gen()->GetPtr()->name;
    } else {
        gen_name = "Dummy";
    }
    BatteryIO io(std::make_shared<DummyGenerator>());
    for (const auto& bat : threads_bats) {
        io.Add(bat);
    }
    // Estimate the elapsed time
    const auto toc = std::chrono::high_resolution_clock::now();    
    const auto ms_total = std::chrono::duration_cast<std::chrono::milliseconds>(toc - tic);
    // Print report
    results.report = io.WriteReport(battery_name.c_str(), gen_name, chrono_to_ms(timer), ms_total);
    chrono_Delete(timer);
    return results;
}

/////////////////////////////////////////////
///// TestsBattery class implementation /////
/////////////////////////////////////////////

/**
 * @brief Initializes the battery name and PRNG factory function. Should be
 * called by derived classes.
 * @param bat_name  Battery name.
 * @param genf      Generator factory function.
 */
TestsBattery::TestsBattery(const std::string& bat_name, GenFactoryFunc genf)
    : battery_name{bat_name}, create_gen{genf}
{
}

/**
 * @brief Run all tests from the battery.
 */
BatteryResults TestsBattery::Run(std::seed_seq* seq, unsigned int nthreads) const
{
    printf("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n"
        "                 Starting %s\n"
        "                 Version: %s\n"
        "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n\n\n",
        battery_name.c_str(), PACKAGE_STRING);

    TestsPull pull(tests, seq);
    return pull.Run(create_gen, battery_name, nthreads);
}

/**
 * @brief Run selected test(s) from the battery.
 * @param id  Test ID (if it is <= 0 -- all tests will be run)
 * @param seq  Seed for PRNG that shuffles the tests between threads.
 * @param nthreads Number of threads (or `NTHREADS_DEFAULT` for autodetection)
 */
BatteryResults TestsBattery::RunTest(int id, std::seed_seq* seq, unsigned int nthreads) const
{
    // Run all tests
    if (id <= 0) {
        return Run(seq, nthreads);
    }
    // Run selected tests
    std::vector<TestDescr> t;
    BatteryResults results(1);
    for (size_t i = 0; i < tests.size(); i++) {
        if (tests[i].GetId() == id)
            t.push_back(tests[i]);
    }
    if (t.size() == 0) {
        return results;
    }
    printf("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n"
        "                 Starting %s test %d\n"
        "                 Version: %s\n"
        "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n\n\n",
        battery_name.c_str(), id, PACKAGE_STRING);

    TestsPull pull(t, seq);
    return pull.Run(create_gen, battery_name + " test " + std::to_string(id), nthreads);
}



//////////////////////////
///// Some functions /////
//////////////////////////

namespace testu01_threads {

/**
 * @brief Needed to enable binary stdout on Windows
 */
void set_bin_stdout()
{
#ifdef ENABLE_BIN_STDIN
    _setmode( _fileno(stdout), _O_BINARY);
#endif
}


/**
 * @brief Needed to enable binary stdin on Windows
 */
void set_bin_stdin()
{
#ifdef ENABLE_BIN_STDIN
    _setmode( _fileno(stdin), _O_BINARY); // needed to allow binary stdin on windows
#endif
}


/**
 * @brief Dump an output of a 32-bit PRNG to the stdout in the format suitable
 * for PractRand.
 */
void prng_bits32_to_file(std::shared_ptr<UniformGenerator> genptr)
{
    std::uint32_t buf[256];
    set_bin_stdout();
    while (1) {
        for (size_t i = 0; i < 256; i++) {
            buf[i] = genptr->GetBits32();
        }
        fwrite(buf, sizeof(std::uint32_t), 256, stdout);
    }
}


} // namespace testu01_threads

///////////////////////////////////////////////////
///// Wrappers for TestU01 original batteries /////
///////////////////////////////////////////////////

namespace testu01_threads::original::battery {
    /**
     * @brief Directly calls the unmodified (single-threaded) TestU01
     * SmallCrush battery.
     */
    void SmallCrush(UniformGenerator& gen)
    {
        bbattery_SmallCrush(gen.GetPtr()->GetPtr());
    }

    /**
     * @brief Directly calls the unmodified (single-threaded) TestU01
     * Crush battery.
     */
    void Crush(UniformGenerator& gen)
    {
        bbattery_Crush(gen.GetPtr()->GetPtr());
    }

    /**
     * @brief Directly calls the unmodified (single-threaded) TestU01
     * BigCrush battery.
     */
    void BigCrush(UniformGenerator& gen)
    {
        bbattery_BigCrush(gen.GetPtr()->GetPtr());
    }

    /**
     * @brief Directly calls the unmodified (single-threaded) TestU01
     * pseudoDIEHARD battery.
     */
    void pseudoDIEHARD(UniformGenerator& gen)
    {
        bbattery_pseudoDIEHARD(gen.GetPtr()->GetPtr());
    }
} // namespace testu01_threads::original::battery

