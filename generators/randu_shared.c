/**
 * @file randu_shared.c
 * @brief An implementation of RANDU - low-quality 32-bit LCG.
 * @copyright (c) 2024 Alexey L. Voskov, Lomonosov Moscow State University.
 * alvoskov@gmail.com
 *
 * All rights reserved.
 *
 * This software is provided under the Apache 2 License.
 *
 * In scientific publications which used this software, a reference to it
 * would be appreciated.
 */
#include "testu01th/cinterface.h"

PRNG_CMODULE_PROLOG

/**
 * @brief RANDU PRNG state
 */
typedef struct {
    uint32_t x;
} RanduState;


static inline unsigned long get_bits32_raw(void *param, void *state)
{
    RanduState *obj = state;
    (void) param;
    obj->x = 65539 * obj->x + 12345;
    return obj->x;
}


static void *init_state()
{
    RanduState *obj = intf.malloc(sizeof(RanduState));
    obj->x = intf.get_seed64() >> 32;
    return (void *) obj;
}


MAKE_UINT32_PRNG("RANDU", NULL)
