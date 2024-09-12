/**
 * @file sfc64_shared.c
 * @brief SF64 (Small Fast Chaotic 64-bit) PRNG with period at least 2^{64}.
 * @details This generator is one of the fastest because it doesn't use
 * multiplications. It slightly remembers LFSR based generators but includes
 * a lot of additions. Addition is non-linear operation in GF(2) that prevents
 * problem with MatrixRank and LinearComp tests. The theory behind SFC64
 * is not clear.
 *
 * SFC64 passes SmallCrush, Crush, ????, ?????.
 *
 * @copyright SFC64 algorithm is developed by Chris Doty-Humphrey,
 * the author of PractRand (https://sourceforge.net/projects/pracrand/).
 * Some portions of the source code were taken from PractRand that is
 * released as Public Domain.
 * 
 * Adaptation for TestU01-threads:
 * (c) 2024 Alexey L. Voskov, Lomonosov Moscow State University.
 * alvoskov@gmail.com
 *
 * All rights reserved.
 *
 * This software is provided under the Apache 2 License.
 */
#include "testu01th/cinterface.h"

PRNG_CMODULE_PROLOG

/**
 * @brief sfc64 state.
 */
typedef struct {
    uint64_t a;
    uint64_t b;
    uint64_t c;
    uint64_t counter;
    Interleaved32Buffer i32buf;
} Sfc64State;


static inline uint64_t get_bits64_raw(void *param, void *state)
{
    enum {BARREL_SHIFT = 24, RSHIFT = 11, LSHIFT = 3};
    Sfc64State *obj = state;
    (void) param;
    uint64_t tmp = obj->a + obj->b + obj->counter++;
    obj->a = obj->b ^ (obj->b >> RSHIFT);
    obj->b = obj->c + (obj->c << LSHIFT);
    obj->c = ((obj->c << BARREL_SHIFT) | (obj->c >> (64-BARREL_SHIFT))) + tmp;
    return tmp;
}

static void *init_state(void)
{
    Sfc64State *obj = intf.malloc(sizeof(Sfc64State));
    obj->a = obj->b = obj->c = intf.get_seed64();
    obj->counter = 1;
	for (int i = 0; i < 16; i++) {
        get_bits64_raw(NULL, obj);
    }
    Interleaved32Buffer_init(&obj->i32buf);
    return (void *) obj;
}

MAKE_UINT64_INTERLEAVED32_PRNG("SFC64", Sfc64State, NULL)
