/**
 * @file bigcrush.h
 * @brief A multithreaded version of BigCrush battery from TestU01 library.
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
#ifndef __TESTU01_BIGCRUSH_H
#define __TESTU01_BIGCRUSH_H
#include "testu01_mt.h"

namespace testu01_threads {

/**
 * @brief BigCrush battery from TestU01 optimized for multithreading.
 */
class BigCrushBattery : public TestsBattery
{
public:
    BigCrushBattery(GenFactoryFunc genf);
};

} // namespace testu01_threads

#endif // __TESTU01_BIGCRUSH_H
