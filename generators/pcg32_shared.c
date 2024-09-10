/**
 * @file pcg32_shared.c
 * @brief PCG32 PRNG implementation.
 * @details PCG (permuted congruental generators) is a family of pseudorandom
 * number generators invented by M.E. O'Neill. The PCG32 is a version with
 * 32-bit output and with 64-bit state. It passes SmallCrush, Crush
 * and BigCrush batteries.
 * 
 * @copyright The original implementation:
 * (c) 2014 M.E. O'Neill (https://pcg-random.org).
 *
 * Adaptation for TestU01-threads:
 * (c) 2024 Alexey L. Voskov, Lomonosov Moscow State University.
 * alvoskov@gmail.com
 *
 * All rights reserved.
 *
 * This software is provided under the Apache 2 License.
 *
 * In scientific publications which used this software, a reference to it
 * would be appreciated.
 */
#include "testu01_mt_cintf.h"

PRNG_CMODULE_PROLOG

typedef struct {
    uint64_t x;
} Pcg32State;

static inline unsigned long get_bits32_raw(void *param, void *state)
{
    Pcg32State *obj = state;
    (void) param;
    uint32_t xorshifted = ((obj->x >> 18u) ^ obj->x) >> 27u;
    uint32_t rot = obj->x >> 59u;
    obj->x = obj->x * 6364136223846793005ULL + 12345;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

static void *init_state()
{
    Pcg32State *obj = intf.malloc(sizeof(Pcg32State));
    obj->x = intf.get_seed64();
    return (void *) obj;
}

MAKE_UINT32_PRNG("PCG32", NULL)
