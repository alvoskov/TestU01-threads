#include "testu01th/testu01_plugin.h"
#include "testu01th/batteries.h"
#include <memory>
#include <iostream>
using namespace testu01_threads;


extern "C" BatteryExitCode EXPORT battery_func(const GeneratorInfo *gen,
    const CallerAPI *intf, const BatteryOptions *opts)
{
    (void) opts;
    auto create_gen = [gen, intf] () -> std::shared_ptr<UniformGeneratorPlugin> {
        return std::shared_ptr<UniformGeneratorPlugin>(new UniformGeneratorPlugin(gen, intf));
    };

    const std::string battery(opts->param);
    if (opts->nthreads == 1) {
        if (battery == "SmallCrush" || battery == "") {
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
/*
    if (battery == "SmallCrush") {
        SmallCrushBattery bat(create_gen);
        RunBattery(bat, test_id, entropy);
    } else if (battery == "Crush") {
        CrushBattery bat(create_gen);
        RunBattery(bat, test_id, entropy);
    } else if (battery == "BigCrush") {
        BigCrushBattery bat(create_gen);
        RunBattery(bat, test_id, entropy);
    } else if (battery == "pseudoDIEHARD") {
        PseudoDiehardBattery bat(create_gen);
        RunBattery(bat, test_id, entropy);
    } else if (battery == "SmallCrush_ser") {
        auto objptr = create_gen();
        bbattery_SmallCrush(objptr->GetPtr());
    } else if (battery == "Crush_ser") {
        auto objptr = create_gen();
        bbattery_Crush(objptr->GetPtr());
    } else if (battery == "BigCrush_ser") {
        auto objptr = create_gen();
        bbattery_BigCrush(objptr->GetPtr());
    } else if (battery == "pseudoDIEHARD_ser") {
        auto objptr = create_gen();
        bbattery_pseudoDIEHARD(objptr->GetPtr());
*/


//    SmallCrushBattery bat(create_gen);
//    std::cout << bat.Run().report << std::endl;
    return BATTERY_PASSED;
}
