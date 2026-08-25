/**
 * @file testu01th_pipes.cpp
 * @brief Reads pseudorandom numbers from stdin as uint32_t in binary form
 * and sends it to one-threaded version of TestU01.
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
#include "testu01_threads.h"
#include "testu01th/fileio.h"
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>

using namespace testu01_threads;

static void print_help()
{
    static const std::string helptext{
    "Runs TestU01 batteries from binary data from stdin. Data are\n"
    "processed as unsigned 32-bit integers. All batteries are working\n"
    "in one-threaded mode. Multi-threaded mode is available only for\n"
    "C modules and testu01th_run\n\n"
    "Usage: test01th_pipes battery [--threads]\n"
    "  battery: SmallCrush, Crush, BigCrush, pseudoDIEHARD\n\n"
    "Example:\n"
    "  testu01th_demo.exe stdout KISS93 | testu01th_pipes.exe SmallCrush\n\n"
    };
    std::cout << helptext << std::endl;
}


static int run_original(const std::string& battery)
{
    Stdin32Collector stdin_prng;
    if (battery == "SmallCrush") {
        original::battery::SmallCrush(stdin_prng);
    } else if (battery == "Crush") {
        original::battery::Crush(stdin_prng);
    } else if (battery == "BigCrush") {
        original::battery::BigCrush(stdin_prng);
    } else if (battery == "pseudoDIEHARD") {
        original::battery::pseudoDIEHARD(stdin_prng);
    } else {
        std::cerr << "Unknown battery " << battery << std::endl;
        return 1;
    }
    return 0;
}


static int run_parallel(const std::string& battery)
{
    Stdin32ParallelCollector stdin_par_factory;
    if (battery == "SmallCrush") {
        SmallCrushBattery bat(stdin_par_factory.CreateGenFactoryFunc());
        auto results = bat.Run();
        std::cout << results << std::endl;
    } else if (battery == "Crush") {
        CrushBattery bat(stdin_par_factory.CreateGenFactoryFunc());
        auto results = bat.Run();
        std::cout << results << std::endl;
    } else if (battery == "BigCrush") {
        BigCrushBattery bat(stdin_par_factory.CreateGenFactoryFunc());
        auto results = bat.Run();
        std::cout << results << std::endl;
    } else if (battery == "pseudoDIEHARD") {
        PseudoDiehardBattery bat(stdin_par_factory.CreateGenFactoryFunc());
        auto results = bat.Run();
        std::cout << results << std::endl;
    } else {
        std::cerr << "Unknown battery " << battery << std::endl;
        return 1;
    }
    return 0;
}



/**
 * @brief Program entry point.
 */
int main(int argc, char* argv[]) 
{
    Stdin32Collector stdin_prng;
    Stdin32ParallelCollector stdin_par_factory;
    if (argc < 2) {
        print_help();
        return 0;
    }
    std::string battery(argv[1]);
    if (argc >= 3) {
        if (!std::strcmp(argv[2], "--threads")) {
            return run_parallel(battery);
        } else {
            std::cerr << "Unknown argument " << argv[2] << std::endl;
            return 1;
        }
    } else {
        return run_original(battery);
    }
}
