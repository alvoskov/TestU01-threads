/**
 * @file dummy_shared.c
 * @brief An implementation of "dummy" PRNG that always returns 0.
 * It is used only for performance measurements!
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
#include "testu01th/cinterface.h"
#include "testu01th/dummy_module.h"

PRNG_CMODULE_PROLOG

EXPORT int gen_getinfo(GenInfoC *gi)
{
    return dummy_gen_getinfo(gi);
}
