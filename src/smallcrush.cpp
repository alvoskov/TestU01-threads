/**
 * @file smallcrush.cpp
 * @brief A multithreaded version of SmallCrush battery from TestU01 library.
 * Based on the `bbattery.c` file.
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
#include "testu01th/smallcrush.h"
#include "testu01th/testu01_callbacks.h"

using namespace testu01_threads;

SmallCrushBattery::SmallCrushBattery(GenFactoryFunc genf)
    : TestsBattery{"SmallCrush(mt)", genf}
{
    const int r = 0;
    int j2 = 0;

    tests.emplace_back(++j2, "BirthdaySpacings", // 1
        smarsa_BirthdaySpacings_cb(1, 5 * MILLION, r, 1073741824, 2, 1));

    tests.emplace_back(++j2, "Collision", [] (const TestDescr& td, BatteryIO& io) { // 2
        auto* res3 = sknuth_CreateRes2();
        sknuth_Collision(io.Gen()->GetPtr(), res3, 1, 5 * MILLION, 0, 65536, 2);
        io.Add(td.GetId(), td.GetName(), res3->Pois->pVal2);
        sknuth_DeleteRes2(res3);
    });

    tests.emplace_back(++j2, "Gap", // 3
        sknuth_Gap_cb(1, MILLION / 5, 22, 0.0, 1.0 / 256.0));

    tests.emplace_back(++j2, "SimpPoker", // 4
        sknuth_SimpPoker_cb(1, 2 * MILLION / 5, 24, 64, 64));

    tests.emplace_back(++j2, "CouponCollector", // 5
        sknuth_CouponCollector_cb(1, MILLION / 2, 26, 16));

    tests.emplace_back(++j2, "MaxOft", // 6-7
        sknuth_MaxOft_cb(1, 2 * MILLION, 0, MILLION / 10, 6));

    tests.emplace_back(++j2, "WeightDistrib", [] (const TestDescr& td, BatteryIO& io) { // 8
        auto* res2 = sres_CreateChi2();
        svaria_WeightDistrib (io.Gen()->GetPtr(), res2, 1, MILLION / 5, 27, 256, 0.0, 0.125);
        io.Add(td.GetId(), td.GetName(), res2->pVal2[gofw_Mean]);
        sres_DeleteChi2(res2);
    });

    tests.emplace_back(++j2, "MatrixRank", // 9
        smarsa_MatrixRank_cb(1, 20 * THOUSAND, 20, 10, 60, 60));

    tests.emplace_back(++j2, "HammingIndep",
        sstring_HammingIndep_cb(1, MILLION/2, 20, 10, 300, 0)); // 10

    tests.emplace_back(++j2, "RandomWalk", // 11-15
        smarsa_RandomWalk1_cb(1, MILLION, r, 30, 150, 150, ""));
}
