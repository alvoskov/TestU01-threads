/**
 * @file crush.h
 * @brief A multithreaded version of Crush battery from TestU01 library.
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
#ifndef __CRUSH_H
#define __CRUSH_H
#include "testu01_mt.h"

void mt_bat_Crush (std::function<std::shared_ptr<UniformGenerator>()> create_gen);
#endif
