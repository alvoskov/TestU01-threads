/**
 * @file kiss93_shared.c
 * @brief KISS93 pseudorandom number generator. It passes SmallCrush
 * but fails the LinearComp (r = 29) test in the Crush battery (N72).
 * @copyright (c) 2024 Alexey L. Voskov, Lomonosov Moscow State University.
 * alvoskov@gmail.com
 *
 * The KISS93 algorithm is developed by George Marsaglia.
 *
 * All rights reserved.
 *
 * This software is provided under the Apache 2 License.
 */
#include "testu01th/cinterface.h"

PRNG_CMODULE_PROLOG

typedef struct {
    uint32_t S1;
    uint32_t S2;
    uint32_t S3;
} KISS93State;

static inline unsigned long get_bits32_raw(void *param, void *state)
{
    KISS93State *obj = state;
    (void) param;
    obj->S1 = 69069 * obj->S1 + 23606797;
    uint32_t b = obj->S2 ^ (obj->S2 << 17);
    obj->S2 = (b >> 15) ^ b;
    b = ((obj->S3 << 18) ^ obj->S3) & 0x7fffffffU;
    obj->S3 = (b >> 13) ^ b;
    return obj->S1 + obj->S2 + obj->S3;
}


static void *init_state()
{
    KISS93State *obj = intf.malloc(sizeof(KISS93State));
    obj->S1 = 12345;
    obj->S2 = 6789;
    obj->S3 = intf.get_seed64();// Default is 111213;
    return (void *) obj;
}

MAKE_UINT32_PRNG("KISS93", NULL)
