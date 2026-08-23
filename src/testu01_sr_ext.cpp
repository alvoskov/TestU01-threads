/**
 * @file testu01th_sr_ext.cpp
 * @brief Extension (plugin) for SmokeRand that contains TestU01 batteries.
 * It gets pseudorandom number generators from SmokeRand test suite: it
 * can either upload PRNG from shared library or get its output from stdin.
 *
 * @copyright
 * (c) 2024-2026 Alexey L. Voskov, Lomonosov Moscow State University.
 * alvoskov@gmail.com
 *
 * All rights reserved.
 *
 * This software is provided under the Apache 2 License.
 *
 * In scientific publications which used this software, a reference to it
 * would be appreciated.
 */
#include "testu01th/testu01_plugin.h"
#include "testu01th/batteries.h"
#include <cstring>
#include <memory>
#include <iostream>
#include <fstream>
#include <random>
using namespace testu01_threads;


static void print_help()
{
    static const std::string helptext(
    "This is a plugin for SmokeRand PRNG test suite containing a multi-threaded\n"
    "version of TestU01. The parallel mode allows to use all cores of CPU for\n"
    "computations and used its own dispatcher. The serial version runs in\n"
    "one-threaded mode and just runs the batteries from TestU01 without\n"
    "modification.\n\n"
    "Usage: smokerand s=testu01_shared.so genlib --batparam=battery [options]\n"
    "  battery: SmallCrush, Crush, BigCrush, pseudoDIEHARD\n"
    "  gen_lib: A shared library with generator, plugin for SmokeRand");
    std::cout << helptext << std::endl << std::endl;
}

static bool SaveProtocol(const BatteryResults& results, const std::string& filename = "report.txt")
{
    std::ofstream outfile;
    std::cout << results.report << std::endl;
    outfile.open(filename);
    if (outfile.is_open()) {
        outfile << results.ToString() << std::endl;
        return true;
    } else {
        std::cerr << "Cannot open output file" << std::endl;
        return false;
    }
}

/**
 * @brief Runs the original unmodified TestU01 batteries (SmallCrush, Crush,
 * BigCrush, pseudoDIEHARD). No TestU01-threads dispatchers/custom batteries/
 * report generators are involved.
 */
static BatteryExitCode
run_original_testu01(const BatteryOptions* opts,
    std::function<std::shared_ptr<UniformGeneratorPlugin>()> create_gen)
{
    const std::string battery{opts->param};
    if (battery == "SmallCrush") {
        auto objptr = create_gen();
        bbattery_SmallCrush(objptr->GetPtr());
    } else if (battery == "Crush") {
        auto objptr = create_gen();
        bbattery_Crush(objptr->GetPtr());
    } else if (battery == "BigCrush") {
        auto objptr = create_gen();
        bbattery_BigCrush(objptr->GetPtr());
    } else if (battery == "pseudoDIEHARD") {
        auto objptr = create_gen();
        bbattery_pseudoDIEHARD(objptr->GetPtr());
    } else {
        std::cerr << "Unknown battery " << battery << std::endl;
        return BATTERY_ERROR;
    }
    return BATTERY_PASSED;
}

/**
 * @brief Runs the parallel (modified) versions of TestU01 batteries
 * (SmallCrush, Crush, BigCrush, pseudoDIEHARD).
 */
static BatteryExitCode
run_parallel_testu01(const BatteryOptions* opts,
    const CallerAPI* intf,
    std::function<std::shared_ptr<UniformGeneratorPlugin>()> create_gen)
{
    const std::string battery{opts->param};
    const unsigned int testid{opts->test.id};
    // a) Obtain seeds for the internal permutations in the batteries
    uint32_t seed[8];
    seeds_to_array_u32(intf, seed, 8);
    std::seed_seq seq(seed, seed + 8);
    // b) Run battery
    if (battery == "SmallCrush") {
        SmallCrushBattery bat(create_gen);
        auto results = bat.RunTest(testid, &seq, opts->nthreads);
        SaveProtocol(results);
    } else if (battery == "Crush") {
        CrushBattery bat(create_gen);
        auto results = bat.RunTest(testid, &seq, opts->nthreads);
        SaveProtocol(results);
    } else if (battery == "BigCrush") {
        BigCrushBattery bat(create_gen);
        auto results = bat.RunTest(testid, &seq, opts->nthreads);
        SaveProtocol(results);
    } else if (battery == "pseudoDIEHARD") {
        PseudoDiehardBattery bat(create_gen);
        auto results = bat.RunTest(testid, &seq, opts->nthreads);
        SaveProtocol(results);
    } else {
        std::cerr << "Unknown battery " << battery << std::endl;
        return BATTERY_ERROR;
    }
    return BATTERY_PASSED;
}

/**
 * @brief SmokeRand plugin entry point.
 */
extern "C" BatteryExitCode EXPORT battery_func(const GeneratorInfo* gen,
    const CallerAPI* intf, const BatteryOptions* opts)
{
    auto create_gen = [gen, intf] () -> std::shared_ptr<UniformGeneratorPlugin> {
        return std::shared_ptr<UniformGeneratorPlugin>(new UniformGeneratorPlugin(gen, intf));
    };
    // A battery is not selected: show help.
    if (std::strlen(opts->param) == 0) {
        print_help();
        return BATTERY_PASSED;
    }
    if (opts->test.name != nullptr) {
        std::cerr << "--testname=name key is not supported" << std::endl;
        return BATTERY_ERROR;
    }

    if (opts->nthreads == 1 && opts->test.id == TESTS_ALL) {
        // Run an unmodified battery. It doesn't support tests selection
        // by ID, so if a user passed `testid` we should use our own
        // dispatcher.
        return run_original_testu01(opts, create_gen);
    } else {
        // Run a battery using our own multithreaded dispatcher.
        return run_parallel_testu01(opts, intf, create_gen);
    }
    return BATTERY_PASSED;
}
