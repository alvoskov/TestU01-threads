/**
 * @file testu01th_demo.cpp
 * @brief Runs TestU01 batteries for some pre-defined pseudorandom
 * number generators. Cannot load tests from DLLs.
 */
#include "testu01_mt.h"
#include "batteries.h"
#include "generators.h"

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


int main(int argc, char *argv[]) 
{
    const std::map<std::string, GenFactoryFunc> gen_map = {
        {"LCG", [] () -> std::shared_ptr<UniformGenerator> {
            return std::shared_ptr<UniformGenerator>(new LcgGenerator());
        }},
        {"LCG59", [] () -> std::shared_ptr<UniformGenerator> {
            return std::shared_ptr<UniformGenerator>(new LcgGenerator59());
        }},
        {"LFIBMUL-17-5", [] () -> std::shared_ptr<UniformGenerator> {
            return std::shared_ptr<UniformGenerator>(new LFibMulGenerator<17,5>());
        }},
        {"KISS93", [] () -> std::shared_ptr<UniformGenerator> {
            return std::shared_ptr<UniformGenerator>(new KISS93Generator());
        }},
        {"MT19937", [] () -> std::shared_ptr<UniformGenerator> {
            return std::shared_ptr<UniformGenerator>(new MT19937Generator());
        }},
        {"SPLITMIX", [] () -> std::shared_ptr<UniformGenerator> {
            return std::shared_ptr<UniformGenerator>(new SplitMixGenerator());
        }}
    };

    if (argc != 3) {
        printf("Usage: testu01th_demo battery generator\n");
        printf("battery: battery name (SmallCrush, Crush, BigCrush, pseudoDIEHARD)\n");
        printf("generator: PRNG name. The supported generators are:\n");
        std::vector<std::string> gnames;
        for (auto &kv : gen_map) {
            gnames.push_back(kv.first);
        }

        std::sort(gnames.begin(), gnames.end());
        for (auto &n : gnames) {
            printf("  %s\n", n.c_str());
        }
        return 0;
    }

    std::string battery(argv[1]), generator(argv[2]);

    if (gen_map.count(generator) == 0) {
        std::cerr << "Unknown generator " << generator << std::endl;
        return 1;
    }

    auto create_gen = gen_map.at(generator);    
    if (battery == "SmallCrush") {
        SmallCrushBattery bat(create_gen);
        bat.Run();
    } else if (battery == "Crush") {
        CrushBattery bat(create_gen);
        bat.Run();
    } else if (battery == "BigCrush") {
        BigCrushBattery bat(create_gen);
        bat.Run();
    } else if (battery == "pseudoDIEHARD") {
        PseudoDiehardBattery bat(create_gen);
        bat.Run();
    } else if (battery == "practrand32") {
        prng_bits32_to_file(create_gen());
    } else {
        std::cerr << "Unknown battery " << battery << std::endl;
    }
    return 0;
}
