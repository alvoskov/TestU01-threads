/**
 * @file superduper64.c
 * @brief An implementation of 64-bit combined "Super Duper" PRNG
 * by G. Marsaglia. It is a combination of 64-bit LCG and 64-bit xorshift.
 * 
 * @details Supports the 64-bit output (default or `--param=u64`) and
 * 32-bit output (`--param=u32`, upper 32 bits are returned). Only the
 * `u32` version passes all SmokeRand batteries.
 *
 * References:
 *
 * https://groups.google.com/g/comp.sys.sun.admin/c/GWdUThc_JUg/m/_REyWTjwP7EJ
 *
 * @copyright
 * (c) 2024-2025 Alexey L. Voskov, Lomonosov Moscow State University.
 * alvoskov@gmail.com
 *
 * This software is licensed under the MIT license.
 */
#include "testu01th/cinterface.h"

PRNG_CMODULE_PROLOG

/**
 * @brief SuperDuper64 PRNG state
 */
typedef struct {
    uint64_t lcg;
    uint64_t xs;
} SuperDuper64State;

static inline uint64_t superduper64_get_bits(void *state)
{
    SuperDuper64State *obj = state;
    obj->lcg = 6906969069 * obj->lcg + 1234567;
    obj->xs ^= (obj->xs << 13);
    obj->xs ^= (obj->xs >> 17);
    obj->xs ^= (obj->xs << 43);
    return obj->lcg + obj->xs;
}

static uint64_t get_bits64_raw(void *param, void *state)
{
    (void) param;
    return rotl64(superduper64_get_bits(state), 32);
}


static void *init_state()
{
    SuperDuper64State *obj = intf.malloc(sizeof(SuperDuper64State));
    obj->lcg = intf.get_seed64();
    do {
        obj->xs = intf.get_seed64();
    } while (obj->xs == 0);
    return (void *) obj;
}

MAKE_UINT64_UPTO32_PRNG("SuperDuper64", NULL);
