/**
 * @file speedtest.h
 * @brief PRNG performance test battery.
 * @copyright (c) 2024 Alexey L. Voskov, Lomonosov Moscow State University.
 * alvoskov@gmail.com
 *
 * All rights reserved.
 *
 * This software is provided under the Apache 2 License.
 *
 * In scientific publications which used this software, a reference to it
 * would be appreciated.
 */

#ifndef __SPEEDTEST_H
#define __SPEEDTEST_H
#include "testu01_mt.h"

namespace testu01_threads {

void test_battery_speed(const testu01_threads::GenFactoryFunc &create_gen,
    const GenInfoC &geninfo);

} // namespace testu01_threads

#endif
