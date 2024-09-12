/**
 * @file shr3_shared.c
 * @brief An implementation of SHR3 - classic 32-bit LSFR generator
 * proposed by G. Marsaglia.
 * @details Fails SmallCrush:
 *
 *           Test                          p-value
 *     ----------------------------------------------
 *      1  BirthdaySpacings                 eps
 *      2  Collision                        eps
 *      6  MaxOft                           eps
 *      8  MatrixRank                       eps
 *     10  RandomWalk1 H                    eps
 *     ----------------------------------------------
 *     All other tests were passed
 *
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
} SHR3State;


static inline unsigned long get_bits32_raw(void *param, void *state)
{
    SHR3State *obj = state;
    (void) param;
    obj->x ^= (obj->x << 17);
    obj->x ^= (obj->x >> 13);
    obj->x ^= (obj->x << 5);
    return obj->x;
}


static void *init_state(void)
{
    SHR3State *obj = intf.malloc(sizeof(SHR3State));
    obj->x = (intf.get_seed64() >> 32) | 0x1;
    return (void *) obj;
}


MAKE_UINT32_PRNG("SHR3", NULL)
