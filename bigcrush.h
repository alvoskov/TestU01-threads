/**
 * @file crush.h
 * @brief A multithreaded version of BigCrush battery from TestU01 library.
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
#ifndef __BIGCRUSH_H
#define __BIGCRUSH_H
#include "testu01_mt.h"

/**
 * @brief A multi-threaded version of BigCrush test battery.
 */
class BigCrushBattery : public TestsBattery
{
public:
    BigCrushBattery(GenFactoryFunc genf);
};

#endif
