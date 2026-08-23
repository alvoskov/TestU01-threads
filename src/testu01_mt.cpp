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
#include "testu01th/testu01_mt.h"
#include <iostream>
#include <cstring>
#include <cmath>
#include <random>
#include <sstream>
#ifdef USE_LOADLIBRARY
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
    std::string txt;
    int PosEntier = 0; // Number of positions occupied by x integer part (%f format)
    int EntierSign; // 0-based position of . in string (%f format)
    int Neg = 0;    // Number is negative

    if (x == 0.0) {
        EntierSign = 1;
    } else {
        EntierSign = PosEntier = (int) floor(log10(fabs(x)) + 1);
        if (x < 0.0)
            Neg = 1;
    }
    if (EntierSign <= 0) {
        PosEntier = 1;
    }

    if ((x == 0.0) ||
        (((EntierSign + J) >= K) && (I >= (PosEntier + J + Neg + 1)))) {
        txt += printf_tos("%*.*f", I, J, x);
    } else { // Use scientific (exponential) notation
        char S[100];
        snprintf(S, 100, "%*.*e", I, K - 1, x);
        char* p = strstr(S, "e+0");
        if (p == nullptr) {
            p = strstr(S, "e-0");
        }
        // Remove the 0 in e-0 and in e+0
        if (p) {
            p += 2;
            while ((*p = *(p + 1)))
	            p++;
            txt += " "; // Output must have at least 'I' spaces
        }
        txt += std::string(S);
    }
    return txt;
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
    : name{name_}
{
    gen.state = static_cast<void*>(this);
    gen.param = nullptr;
    gen.Write = WrExternGen;
    gen.GetU01 = GetU01Handle;
    gen.GetBits = GetBits32Handle;
    gen.name = const_cast<char*>(name.c_str());
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
uint64_t BatteryIO::GetResultsChecksum() const
{
    uint64_t sum{0x123456789ABCDEF};
    for (const auto& r : results) {
        int pvalue_exp;
        uint64_t pvalue_x64{static_cast<uint64_t>(std::frexp(r.pvalue, &pvalue_exp) * 16777216U)};
        pvalue_x64 |= (static_cast<uint64_t>(pvalue_exp) + 32768U) << 32;
        const uint64_t inc{(static_cast<uint64_t>(r.id) << 48) | pvalue_x64};
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
 * @param bat_name  Battery name.
 * @param gen_name  Generator name.
 * @param timer     TestU01 timer that calculated CPU time for all cores.
 * @param ms_total  Elapsed time, milliseconds.
 * @return Battery run report (ASCII string).
 */
std::string BatteryIO::WriteReport(const char* bat_name, const char* gen_name,
    chrono_Chrono* timer, std::chrono::milliseconds ms_total)
{
    std::stringstream sout;
    sout << printf_tos("\n========= Summary results of %s", bat_name)
         << printf_tos(" =========\n\n")
         << printf_tos(" Version:                      %s\n", PACKAGE_STRING)
         << printf_tos(" Generator:                    ") << gen_name
         << printf_tos("\n Number of statistics:         %1u\n",
                static_cast<unsigned int>(results.size()))
         << printf_tos(" Total CPU time (all cores):   ")
         << chrono_tostring(timer, chrono_hms)
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


std::string BatteryResults::ToString() const
{
    std::stringstream sout;
    sout << report;
    for (size_t i = 0; i < pvalues.size(); i++) {
        sout << "===== Tests for thread " << i << " =====\n";
        for (const auto& rec : pvalues[i]) {
            sout << printf_tos("  %5d %32s %.6g\n",
                static_cast<int>(rec.id), rec.name.c_str(), rec.pvalue);
        }
    }
    return sout.str();
}


//////////////////////////////////////////
///// TestsPull class implementation /////
//////////////////////////////////////////

TestsPull::TestsPull(const std::vector<TestDescr>& obj, std::seed_seq* seq)
{
    pos = 0;
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
/*
    std::cout << "INDS" << std::endl;
    for (size_t i = 0; i < len; i++) {
        std::cout << tests_inds[i] << "; ";
    }
    std::cout << std::endl;
*/
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
    swrite_Host = FALSE;
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
        gen_name = threads_bats[0].Gen()->name;
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
    results.report = io.WriteReport(battery_name.c_str(), gen_name, timer, ms_total);
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


void set_bin_stdout()
{
#ifdef USE_LOADLIBRARY
    _setmode( _fileno(stdout), _O_BINARY); // needed to allow binary stdout on windows
#endif
}


void set_bin_stdin()
{
#ifdef USE_LOADLIBRARY
    _setmode( _fileno(stdin), _O_BINARY); // needed to allow binary stdin on windows
#endif
}


/**
 * @brief Dump an output of a 32-bit PRNG to the stdout in the format suitable
 * for PractRand.
 */
void prng_bits32_to_file(std::shared_ptr<UniformGenerator> genptr)
{
    uint32_t buf[256];
    set_bin_stdout();
    while (1) {
        for (size_t i = 0; i < 256; i++) {
            buf[i] = genptr->GetBits32();
        }
        fwrite(buf, sizeof(uint32_t), 256, stdout);
    }
}

///////////////////////////////////////////////////////
///// Functions that generate callbacks for tests /////
///////////////////////////////////////////////////////


TestCbFunc svaria_AppearanceSpacings_cb(long N, long Q, long K, int r, int s, int L)
{
    return [=] (const TestDescr& td, BatteryIO& io) {
        sres_Basic* res = sres_CreateBasic();
        svaria_AppearanceSpacings(io.Gen(), res, N, Q, K, r, s, L);
        io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_Mean]);
        sres_DeleteBasic(res);
    };
}

TestCbFunc sstring_AutoCor_cb(long N, long n, int r, int s, int d)
{
    return [=] (const TestDescr& td, BatteryIO& io) {
        sres_Basic* res = sres_CreateBasic();
        sstring_AutoCor(io.Gen(), res, N, n, r, s, d);
        io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_Sum]);
        sres_DeleteBasic(res);
    };
}

TestCbFunc smarsa_BirthdaySpacings_cb(long N, long n, int r, long d, int t, int p)
{
    return [=] (const TestDescr& td, BatteryIO& io) {
        sres_Poisson* res = sres_CreatePoisson();
        smarsa_BirthdaySpacings(io.Gen(), res, N, n, r, d, t, p);
        io.Add(td.GetId(), td.GetName(), res->pVal2);
        sres_DeletePoisson(res);
    };
}

TestCbFunc smarsa_CollisionOver_cb(long N, long n, int r, long d, int t)
{
    return [=] (const TestDescr& td, BatteryIO& io) {
        smarsa_Res* res = smarsa_CreateRes();
        smarsa_CollisionOver (io.Gen(), res, N, n, r, d, t);
        io.Add(td.GetId(), td.GetName(), res->Pois->pVal2);
        smarsa_DeleteRes(res);
    };
}

TestCbFunc sknuth_CollisionPermut_cb(long N, long n, int r, int t)
{
    return [=] (const TestDescr& td, BatteryIO& io) {
        sknuth_Res2* res = sknuth_CreateRes2();
        sknuth_CollisionPermut(io.Gen(), res, N, n, r, t);
        io.Add(td.GetId(), td.GetName(), res->Pois->pVal2);
        sknuth_DeleteRes2 (res);
    };
}

TestCbFunc sknuth_CouponCollector_cb(long N, long n, int r, int d)
{
    return [=] (const TestDescr& td, BatteryIO& io) {
        auto* res2 = sres_CreateChi2();
        sknuth_CouponCollector (io.Gen(), res2, N, n, r, d);
        io.Add(td.GetId(), td.GetName(), res2->pVal2[gofw_Mean]);
        sres_DeleteChi2(res2);
    };
}


TestCbFunc snpair_ClosePairs_cb(long N, long n, int r, int k, int p, int m,
    const std::string& mess, bool flag)
{
    return [=] (const TestDescr& td, BatteryIO& io) {
        snpair_Res* res = snpair_CreateRes();
        snpair_ClosePairs(io.Gen(), res, N, n, r, k, p, m);
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
        snpair_ClosePairs(io.Gen(), res, N, n, r, k, p, m);
        io.Add(td.GetId(), td.GetName(), res->pVal[snpair_NP]);
        snpair_DeleteRes(res);
    };
}

TestCbFunc snpair_ClosePairsBitMatch_cb(long N, long n, int r, int t)
{
    return [=] (const TestDescr &td, BatteryIO &io) {
        snpair_Res* res = snpair_CreateRes();
        snpair_ClosePairsBitMatch(io.Gen(), res, N, n, r, t);
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
        smarsa_CollisionOver(io.Gen(), res, 1, 2097152, i, 4, 10);
        io.Add(td.GetId(), td.GetName(), res->Bas->pVal2[gofw_Mean]);
        smarsa_DeleteRes(res);
    };
}

TestCbFunc sspectral_Fourier3_cb(long N, int k, int r, int s)
{
    return [=] (const TestDescr& td, BatteryIO& io) {
        sspectral_Res* res = sspectral_CreateRes();
        sspectral_Fourier3(io.Gen(), res, N, k, r, s);
        io.Add(td.GetId(), td.GetName(), res->Bas->pVal2[gofw_AD]);
        sspectral_DeleteRes(res);
    };
}


TestCbFunc sknuth_Gap_cb(long N, long n, int r, double Alpha, double Beta)
{
    return [=] (const TestDescr& td, BatteryIO& io) {
        sres_Chi2* res = sres_CreateChi2();
        sknuth_Gap(io.Gen(), res, N, n, r, Alpha, Beta);
        io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_Mean]);
        sres_DeleteChi2(res);
    };
}


TestCbFunc smarsa_GCD_cb(long N, long n, int r, int s)
{
    return [=] (const TestDescr& td, BatteryIO& io) {
        smarsa_Res2* res = smarsa_CreateRes2();
        smarsa_GCD(io.Gen(), res, N, n, r, s);
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
        sstring_HammingCorr(io.Gen(), res, N, n, r, s, L);
        io.Add(td.GetId(), td.GetName(), res->Bas->pVal2[gofw_Mean]);
        sstring_DeleteRes(res);

    };
}


TestCbFunc sstring_HammingIndep_cb(long N, long n, int r, int s, int L, int d)
{
    return [=] (const TestDescr& td, BatteryIO& io) {
        sstring_Res* res = sstring_CreateRes();
        sstring_HammingIndep(io.Gen(), res, N, n, r, s, L, d);
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
        sstring_HammingWeight2(io.Gen(), res, N, n, r, s, L);
        io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_Sum]);
        sres_DeleteBasic (res);
    };
}


TestCbFunc scomp_LempelZiv_cb(long N, int t, int r, int s)
{
    return [=] (const TestDescr& td, BatteryIO& io) {
        sres_Basic* res = sres_CreateBasic();
        scomp_LempelZiv(io.Gen(), res, N, t, r, s);
        io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_Sum]);
        sres_DeleteBasic(res);
    };
}


TestCbFunc scomp_LinearComp_cb(long N, long n, int r, int s)
{
    return [=] (const TestDescr& td, BatteryIO& io) {
        scomp_Res* res = scomp_CreateRes();
        scomp_LinearComp(io.Gen(), res, N, n, r, s);
        io.Add(td.GetId(), td.GetName(), res->JumpNum->pVal2[gofw_Mean]);
        io.Add(td.GetId(), td.GetName(), res->JumpSize->pVal2[gofw_Mean]);
        scomp_DeleteRes(res);
    };
}

TestCbFunc sstring_LongestHeadRun_cb(long N, long n, int r, int s, long L)
{
    return [=] (const TestDescr& td, BatteryIO& io) {
        sstring_Res2* res = sstring_CreateRes2();
        sstring_LongestHeadRun(io.Gen(), res, N, n, r, s, L);
        io.Add(td.GetId(), td.GetName(), res->Chi->pVal2[gofw_Mean]);
        io.Add(td.GetId(), td.GetName(), res->Disc->pVal2);
        sstring_DeleteRes2(res);
    };
}


TestCbFunc smarsa_MatrixRank_cb(long N, long n, int r, int s, int L, int k)
{
    return [=] (const TestDescr& td, BatteryIO& io) {
        sres_Chi2* res = sres_CreateChi2();
        smarsa_MatrixRank(io.Gen(), res, N, n, r, s, L, k);
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
        sknuth_MaxOft(io.Gen(), res5, N, n, r, d, t);
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
        smarsa_Opso(io.Gen(), res, N, r, p);
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
        smarsa_CollisionOver(io.Gen(), res, 1, 2097152, i, 32, 4);
        io.Add(td.GetId(), td.GetName(), res->Bas->pVal2[gofw_Mean]);
        smarsa_DeleteRes(res);
    };
}



TestCbFunc sstring_PeriodsInStrings_cb(long N, long n, int r, int s)
{
    return [=] (const TestDescr& td, BatteryIO& io) {
        sres_Chi2* res = sres_CreateChi2();
        sstring_PeriodsInStrings(io.Gen(), res, N, n, r, s);
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
        sknuth_Permutation(io.Gen(), res, N, n, r, t);
        io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_Mean]);
        sres_DeleteChi2(res);
    };
}


TestCbFunc smarsa_RandomWalk1_cb(long N, long n, int r, int s,
    long L0, long L1, const std::string &mess)
{
    return [=] (const TestDescr& td, BatteryIO& io) {
        auto* res = swalk_CreateRes();
        swalk_RandomWalk1(io.Gen(), res, N, n, r, s, L0, L1);
        GetPValue_Walk(io, 1, res, td.GetId(), mess.c_str());
        swalk_DeleteRes(res);
    };
}

TestCbFunc sknuth_Run_cb(long N, long n, int r, bool Up)
{
    return [=] (const TestDescr& td, BatteryIO& io) {
        sres_Chi2* res = sres_CreateChi2();
        sknuth_Run(io.Gen(), res, N, n, r, Up);
        io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_Sum]);
        sres_DeleteChi2(res);
    };
}


TestCbFunc sstring_Run_cb(long N, long n, int r, int s)
{
    return [=] (const TestDescr& td, BatteryIO& io) {
        sstring_Res3* res = sstring_CreateRes3();
        sstring_Run(io.Gen(), res, N, n, r, s);
        io.Add(td.GetId(), td.GetName(), res->NRuns->pVal2[gofw_Mean]);
        io.Add(td.GetId(), td.GetName(), res->NBits->pVal2[gofw_Mean]);
        sstring_DeleteRes3 (res);
    };
}

TestCbFunc svaria_SampleCorr_cb(long N, long n, int r, int k)
{
    return [=] (const TestDescr& td, BatteryIO& io) {
        sres_Basic* res = sres_CreateBasic();
        svaria_SampleCorr(io.Gen(), res, N, n, r, k);
        io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_Mean]);
        sres_DeleteBasic(res);
    };
}

TestCbFunc svaria_SampleProd_cb(long N, long n, int r, int t)
{
    return [=] (const TestDescr& td, BatteryIO& io) {
        sres_Basic *res = sres_CreateBasic();
        svaria_SampleProd(io.Gen(), res, N, n, r, t);
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
        svaria_SampleMean(io.Gen(), res, N, n, r);
        io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_AD]);
        sres_DeleteBasic(res);
    };
}

TestCbFunc smarsa_Savir2_cb(long N, long n, int r, long m, int t)
{
    return [=] (const TestDescr& td, BatteryIO& io) {
        auto* res = sres_CreateChi2();
        smarsa_Savir2(io.Gen(), res, N, n, r, m, t);
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
        smarsa_SerialOver(io.Gen(), res, N, n, r, d, t);
        io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_Mean]);
        sres_DeleteBasic (res);
    };
}


TestCbFunc sknuth_SimpPoker_cb(long N, long n, int r, int d, int k)
{
    return [=] (const TestDescr& td, BatteryIO& io) {
        sres_Chi2* res = sres_CreateChi2();
        sknuth_SimpPoker(io.Gen(), res, N, n, r, d, k);
        io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_Mean]);
        sres_DeleteChi2(res);
    };
}

TestCbFunc svaria_SumCollector_cb(long N, long n, int r, double g)
{
    return [=] (const TestDescr& td, BatteryIO& io) {
        sres_Chi2* res = sres_CreateChi2();
        svaria_SumCollector(io.Gen(), res, N, n, r, g);
        io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_Mean]);
        sres_DeleteChi2(res);
    };
}

TestCbFunc svaria_WeightDistrib_cb(long N, long n, int r, long k,
    double alpha, double beta)
{
    return [=] (const TestDescr& td, BatteryIO& io) {
        sres_Chi2* res = sres_CreateChi2();
        svaria_WeightDistrib(io.Gen(), res, N, n, r, k, alpha, beta);
        io.Add(td.GetId(), td.GetName(), res->pVal2[gofw_Mean]);
        sres_DeleteChi2(res);
    };
}

} // namespace testu01_threads
