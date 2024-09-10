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
 */
#include "testu01_mt_cintf.h"

PRNG_CMODULE_PROLOG

typedef struct {
    uint64_t x;
} WyRandState;


uint64_t get_bits64_raw(void *param, void *state)
{
    const uint64_t c = 0xe7037ed1a0b428db;
    (void) param;
    WyRandState *obj = state;
    obj->x += 0xa0761d6478bd642f;
    __uint128_t t = (__uint128_t) obj->x * (obj->x ^ c);
    return (t >> 64) ^ t;
}

static void *init_state()
{
    WyRandState *obj = intf.malloc(sizeof(WyRandState));
    obj->x = intf.get_seed64();
    return (void *) obj;
}

MAKE_UINT64_UPTO32_PRNG("WyRand", NULL);
