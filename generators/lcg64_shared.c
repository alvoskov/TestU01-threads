/**
 * @file lcg64_shared.c
 * @brief Just 64-bit LCG.
 * @details
 *
 *       Test                          p-value
 * ----------------------------------------------
 *  8  CollisionOver, t = 8          9.3e-151
 * 10  CollisionOver, t = 20          5.6e-10
 * 13  BirthdaySpacings, t = 4         1.4e-5
 * 15  BirthdaySpacings, t = 7       7.4e-289
 * 16  BirthdaySpacings, t = 8          eps
 * 17  BirthdaySpacings, t = 8          eps
 * ----------------------------------------------
 * All other tests were passed
 *
 * @copyright (c) 2024 Alexey L. Voskov, Lomonosov Moscow State University.
 * alvoskov@gmail.com
 *
 * The KISS99 algorithm is developed by George Marsaglia.
 *
 * All rights reserved.
 *
 * This software is provided under the Apache 2 License.
 */
#include "testu01th/cinterface.h"

PRNG_CMODULE_PROLOG

typedef struct {
    uint64_t x;
} Lcg64State;

static inline unsigned long get_bits32_raw(void *param, void *state)
{
    Lcg64State *obj = state;
    (void) param;
    obj->x = obj->x * 6906969069 /*0xd1342543de82ef95*/ + 1;
    return obj->x >> 32;
}

static void *init_state()
{
    Lcg64State *obj = intf.malloc(sizeof(Lcg64State));
    obj->x = intf.get_seed64();
    return (void *) obj;
}

MAKE_UINT32_PRNG("LCG64", NULL)
