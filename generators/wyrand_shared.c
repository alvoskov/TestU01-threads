/**
 * @file wyrand_shared.c
 * @brief wyrand pseudorandom number generator. Passes BigCrush and PractRand
 * batteries of statistical tests. Required 128-bit integers.
 * @details References:
 * - Wang Yi. wyhash project, public domain (Unlicense).
 *   https://github.com/wangyi-fudan/wyhash/blob/master/wyhash.h
 * - testingRNG, wyrand.h file by D. Lemire (Apache 2.0 license)
 *   https://github.com/lemire/testingRNG/blob/master/source/wyrand.h
 *
 * @copyright (c) 2024 Alexey L. Voskov, Lomonosov Moscow State University.
 * alvoskov@gmail.com
 *
 * All rights reserved.
 *
 * This software is provided under the Apache 2 License.
 */
#include "testu01th/cinterface.h"

PRNG_CMODULE_PROLOG

typedef struct {
    uint64_t x;
} WyRandState;


uint64_t get_bits64_raw(void *param, void *state)
{
    uint64_t hi, lo;
    WyRandState *obj = state;
    (void) param;
    obj->x += 0xa0761d6478bd642f;
    lo = unsigned_mul128(obj->x, obj->x ^ 0xe7037ed1a0b428db, &hi);
    return lo ^ hi;
}

static void *init_state(void)
{
    WyRandState *obj = intf.malloc(sizeof(WyRandState));
    obj->x = intf.get_seed64();
    return (void *) obj;
}

MAKE_UINT64_UPTO32_PRNG("WyRand", NULL);
