/**
 * @file splitmix32cbc.c
 * @brief 32-bit modification of SplitMix (mainly for SmokeRand testing)
 * @details References:
 *
 * 1. https://github.com/umireon/my-random-stuff/blob/master/xorshift/splitmix32.c
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
 * @brief SplitMix32 32-bit PRNG state.
 */
typedef struct {
    uint32_t s_prev; /**< Previous value */
    uint32_t w; /**< "Weyl sequence" counter state */
} SplitMix32State;


static inline unsigned long get_bits32_raw(void *param, void *state)
{
    SplitMix32State *obj = state;
    const uint32_t c = 0x9E3779B9;
    uint32_t s = (obj->w += c) ^ obj->s_prev;
    (void) param;
    s ^= s >> 16;
    s *= 0x85ebca6b;
    s ^= s >> 13;
    s *= 0xc2b2ae35;
    s ^= s >> 16;
    obj->s_prev = s;
    
    return s;
}

static void *init_state(void)
{
    SplitMix32State *obj = intf.malloc(sizeof(SplitMix32State));
    obj->w = intf.get_seed64();
    obj->s_prev = intf.get_seed64();
    return (void *) obj;
}


MAKE_UINT32_PRNG("SplitMix32CBC", NULL)
