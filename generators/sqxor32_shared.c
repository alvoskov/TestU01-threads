/**
 * @file sqxor32_shared.c
 * @brief PRNG inspired by the Von Neumann middle squares method and
 * its modification by B.Widynski. This version has reduced 32-bit
 * state and reduced period (2^{32}).
 * @details "Weyl sequence" variant passes SmallCrush but fails
 * the next tests in Crush:
 * - 76  LongestHeadRun, r = 0          1.6e-10
 * - 78  PeriodsInStrings, r = 0         1.1e-8
 * It also fails PractRand after generating 32GiB of data.
 *
 * "Counter" variant passes SmallCrush but fails a lot of tests
 * in Crush battery:
 * -  2  SerialOver, t = 4                eps
 * -  7  CollisionOver, t = 8             eps
 * -  8  CollisionOver, t = 8             eps
 * -  9  CollisionOver, t = 20            eps
 * - 10  CollisionOver, t = 20          1.0e-97
 * - 25  SimpPoker, d = 64                eps
 * - 28  CouponCollector, d = 4           eps
 * - 29  CouponCollector, d = 16          eps
 * - 30  CouponCollector, d = 16         1.4e-8
 * - 35  Run of U01, r = 0               2.2e-4
 * - 39  CollisionPermut, r = 0         1.1e-10
 * - 62  Savir2                          4.8e-5
 * - 76  LongestHeadRun, r = 0          2.5e-10
 * - 78  PeriodsInStrings, r = 0        2.5e-10
 * "Counter" variant rapidly fails PractRand.
 * @copyright (c) 2024 Alexey L. Voskov, Lomonosov Moscow State University.
 * alvoskov@gmail.com
 *
 * All rights reserved.
 *
 * This software is provided under the Apache 2 License.
 */
#include "testu01th/cinterface.h"

PRNG_CMODULE_PROLOG

/**
 * @brief SQXOR 32-bit PRNG state.
 */
typedef struct {
    uint32_t w; /**< "Weyl sequence" counter state */
} SqXor32State;


static unsigned long get_bits32_raw(void *param, void *state)
{
    const uint32_t s = 0x9E3779B9;
    SqXor32State *obj = (SqXor32State *) state;
    (void) param;
    uint32_t ww = obj->w += s; // "Weyl sequence" variant
    //uint32_t ww = ++obj->w ^ s; // "Counter" variant
    // Round 1
    uint64_t sq = ((uint64_t) ww) * ww; // |16bit|16bit||16bit|16bit||
    uint32_t x = (sq >> 32) ^ sq; // Middle squares (32 bits) + XORing
    // Round 2
    sq = (uint64_t) x * ww;
    x = (sq >> 32) ^ sq; // Middle squares (64 bits) + XORing
    // Return the result
    return x;
}


static void *init_state(void)
{
    SqXor32State *obj = intf.malloc(sizeof(SqXor32State));
    obj->w = intf.get_seed64();
    return (void *) obj;
}


MAKE_UINT32_PRNG("SqXor32", NULL)
