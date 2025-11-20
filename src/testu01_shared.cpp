#include "testu01th/testu01_plugin.h"
#include "testu01th/batteries.h"
#include <memory>
#include <iostream>
using namespace testu01_threads;


void print_help()
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

extern "C" BatteryExitCode EXPORT battery_func(const GeneratorInfo *gen,
    const CallerAPI *intf, const BatteryOptions *opts)
{
    auto create_gen = [gen, intf] () -> std::shared_ptr<UniformGeneratorPlugin> {
        return std::shared_ptr<UniformGeneratorPlugin>(new UniformGeneratorPlugin(gen, intf));
    };

    const std::string battery(opts->param);
    if (battery == "") {
        print_help();
        return BATTERY_PASSED;
    }

    if (opts->nthreads == 1) {
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
        }
    } else {
        if (battery == "SmallCrush" || battery == "") {
            SmallCrushBattery bat(create_gen);
            auto results = bat.RunTest(opts->testid);
            std::cout << results.report;
        } else if (battery == "Crush") {
            CrushBattery bat(create_gen);
            auto results = bat.RunTest(opts->testid);
            std::cout << results.report;
        } else if (battery == "BigCrush") {
            BigCrushBattery bat(create_gen);
            auto results = bat.RunTest(opts->testid);
            std::cout << results.report;
        } else if (battery == "pseudoDIEHARD") {
            PseudoDiehardBattery bat(create_gen);
            auto results = bat.RunTest(opts->testid);
            std::cout << results.report;
        }
    }
    return BATTERY_PASSED;
}
