/**
 * @file dummy_shared.c
 * @brief An implementation of "dummy" PRNG that always returns 0.
 * It is used only for performance measurements!
 */
#include "testu01_mt_cintf.h"
#include "dummy_module.h"

PRNG_CMODULE_PROLOG

EXPORT int gen_getinfo(GenInfoC *gi)
{
    return dummy_gen_getinfo(gi);
}
