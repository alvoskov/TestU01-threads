/**
 * @file sqxor_shared.c
 * @brief PRNG inspired by the Von Neumann middle squares method and
 * its modification by B.Widynski. Requires 128-bit integers.
 * @details Passes SmallCrush, Crush and BigCrush batteries
 * ("Weyl sequence" variant).
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
 * @brief SQXOR 64-bit PRNG state.
 */
typedef struct {
    uint64_t w; /**< "Weyl sequence" counter state */
    Interleaved32Buffer i32buf;
} SqXorState;


static inline uint64_t get_bits64_raw(void *param, void *state)
{
    const uint64_t s = 0x9E3779B97F4A7C15;
    SqXorState *obj = state;
    (void) param;
    uint64_t ww = obj->w += s; // "Weyl sequence" variant
    //uint64_t ww = ++obj->w ^ s; // "Counter" variant
    // Round 1
    __int128 sq = ((__int128) ww) * ww; // |32bit|32bit||32bit|32bit||
    uint64_t x = (sq >> 64) ^ sq; // Middle squares (64 bits) + XORing
    // Round 2
    //sq = ((__int128) x) * ww; // Slower but a little more reliable
    sq = ((__int128) x) * x;
    x = (sq >> 64) ^ sq; // Middle squares (64 bits) + XORing
    // Return the result
    return x;
}


static void *init_state(void)
{
    SqXorState *obj = intf.malloc(sizeof(SqXorState));
    obj->w = intf.get_seed64();
    Interleaved32Buffer_init(&obj->i32buf);
    return (void *) obj;
}

MAKE_UINT64_INTERLEAVED32_PRNG("SqXor", SqXorState, NULL)
