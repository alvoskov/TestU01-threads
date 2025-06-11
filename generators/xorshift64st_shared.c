/**
 * @file xorshift64st.c
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
} Xorshift64StState;


static inline unsigned long get_bits32_raw(void *param, void *state)
{
    Xorshift64StState *obj = state;
    (void) param;
    obj->v ^= obj->v >> 12;
    obj->v ^= obj->v << 25;
    obj->v ^= obj->v >> 27;
    uint64_t u = obj->v * 2685821657736338717ULL;
    return u >> 32;//& 0xFFFFFFFF;
}


void *init_state()
{
    Xorshift64StState *obj = intf.malloc(sizeof(Xorshift64StState));
    do {
        obj->v = intf.get_seed64();
    } while (obj->v == 0);
    (void) get_bits32_raw(NULL, obj);
    return obj;
}


MAKE_UINT32_PRNG("xorshift64*", NULL)
