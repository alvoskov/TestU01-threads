/**
 * @file testu01th_demo.cpp
 * @brief Runs TestU01 batteries for some pre-defined pseudorandom
 * number generators. Cannot load tests from DLLs.
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
#include <stdlib.h>
#include <stdio.h>

#include <string>
#include <map>
#include <cstring>
#include <functional>
#include <memory>
#include <vector>
#include <thread>
#include <mutex>
#include <algorithm>
#include <iostream>

using namespace testu01_threads;

static void print_help(const std::map<std::string, GenFactoryFunc>& gen_map)
{
    std::cout << "Usage: testu01th_demo battery generator [--full-report]\n"
              << "battery: battery name; possible variants are:\n"
              << "  SmallCrush, Crush, BigCrush, pseudoDIEHARD, stdout\n"
              << "generator: PRNG name. The supported generators are:\n";
    std::vector<std::string> gnames;
    for (const auto& kv : gen_map) {
        gnames.push_back(kv.first);
    }

    std::sort(gnames.begin(), gnames.end());
    for (const auto& n : gnames) {
        std::cout << "  " << n << "\n";
    }
}

static void run_battery(TestsBattery& bat, bool full_report)
{
    const auto res = bat.Run();
    std::cout << ((full_report) ? res.ToString() : res.report) << std::endl;
}

int main(int argc, char* argv[]) 
{
    static const std::map<std::string, GenFactoryFunc> gen_map = {
        {"LCG", [] () -> std::shared_ptr<UniformGenerator> {
            return std::make_shared<LcgGenerator>();
        }},
        {"LCG59", [] () -> std::shared_ptr<UniformGenerator> {
            return std::make_shared<LcgGenerator59>();
        }},
        {"LFIBMUL-17-5", [] () -> std::shared_ptr<UniformGenerator> {
            return std::make_shared<LFibMulGenerator<17,5>>();
        }},
        {"KISS93", [] () -> std::shared_ptr<UniformGenerator> {
            return std::make_shared<KISS93Generator>();
        }},
        {"MT19937", [] () -> std::shared_ptr<UniformGenerator> {
            return std::make_shared<MT19937Generator>();
        }},
        {"SPLITMIX", [] () -> std::shared_ptr<UniformGenerator> {
            return std::make_shared<SplitMixGenerator>();
        }}
    };

    if (argc < 3 || argc > 4) {
        print_help(gen_map);
        return 0;
    }

    bool full_report{false};
    if (argc >= 4) {
        if (std::string(argv[3]) != "--full-report") {
            std::cerr << "Unknown argument" << argv[3] << std::endl;
            return 1;
        } else {
            full_report = true;
        }
    }

    const std::string battery{argv[1]};
    const std::string generator{argv[2]};

    if (gen_map.count(generator) == 0) {
        std::cerr << "Unknown generator " << generator << std::endl;
        return 1;
    }

    auto create_gen = gen_map.at(generator);    
    if (battery == "SmallCrush") {
        SmallCrushBattery bat(create_gen);
        run_battery(bat, full_report);
    } else if (battery == "Crush") {
        CrushBattery bat(create_gen);
        run_battery(bat, full_report);
    } else if (battery == "BigCrush") {
        BigCrushBattery bat(create_gen);
        run_battery(bat, full_report);
    } else if (battery == "pseudoDIEHARD") {
        PseudoDiehardBattery bat(create_gen);
        run_battery(bat, full_report);
    } else if (battery == "stdout") {
        prng_bits32_to_file(create_gen());
    } else {
        std::cerr << "Unknown battery " << battery << std::endl;
    }
    return 0;
}
