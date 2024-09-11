/**
 * @file pdiehard.h
 * @brief A multithreaded version of pseudoDIEHARD battery from TestU01 library.
 * Based on the `bbattery.c` file.
 * @copyright (c) 2024 Alexey L. Voskov, Lomonosov Moscow State University.
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
#ifndef __PDIEHARD_H
#define __PDIEHARD_H
#include "testu01_mt.h"

namespace testu01_threads {

/**
 * @brief As close as possible to the DIEHARD test suite.
 */
class PseudoDiehardBattery : public TestsBattery
{
public:
    PseudoDiehardBattery(GenFactoryFunc genf);
};

} // namespace testu01_threads

#endif