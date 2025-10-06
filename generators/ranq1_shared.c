/**
 * @file ranq1.c
 * @brief `Ranq1` pseudorandom number generator from "Numerical Recipes.
 * The Art of Scientific Computation" (3rd edition). It is a modification
 * of classical xorshift64* PRNG. Its lower bits have a low linear
 * complexity. It also fails the `bspace32_2d` test and the 64-bit birthday
 * paradox test.
 * @copyright The original xorshift64 generator was invented by G.Marsaglia.
 * The xorshift64* modiciation with non-linear output was suggesed by S.Vigna.
 * The `Ranq1` variant was suggested by the authors of "Numerical Recipes".
 *
 * Thread-safe reimplementation for SmokeRand:
 *
 * (c) 2024-2025 Alexey L. Voskov, Lomonosov Moscow State University.
 * alvoskov@gmail.com
 *
 * This software is licensed under the MIT license.
 */
#include "testu01th/cinterface.h"

PRNG_CMODULE_PROLOG

/**
 * @brief RanQ1 PRNG state. The generator is taken
 * from "Numerical Recipes" (3rd edition).
 */
typedef struct {
    uint64_t v; ///< xorshift64 state
} RanQ1State;


static inline unsigned long get_bits32_raw(void *param, void *state)
{
    RanQ1State *obj = state;
    (void) param;
    obj->v ^= obj->v >> 21;
    obj->v ^= obj->v << 35;
    obj->v ^= obj->v >> 4;
    return (obj->v * 2685821657736338717ULL) >> 32;//& 0xFFFFFFFF;
}


void *init_state()
{
    RanQ1State *obj = intf.malloc(sizeof(RanQ1State));
    uint64_t seed = intf.get_seed64();
    obj->v = 4101842887655102017ULL;
    if (seed != obj->v) {
        obj->v ^= seed;
    }
    (void) get_bits32_raw(NULL, obj);
    return obj;
}


MAKE_UINT32_PRNG("RanQ1", NULL)
