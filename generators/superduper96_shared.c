/**
 * @file superduper96.c
 * @brief An implementation of 32-bit combined "Super Duper" PRNG
 * by G. Marsaglia et al.
 * @details SuperDuper73 is a combined generator based on 32-bit "69069"
 * MCG and xorshift32 LFSR. Proposed in the next work:
 *
 * - Marsaglia G.,, Ananthanarayanan K., Paul N. 1973. How to use the McGill
 *   random number package SUPER-DUPER. Tech. rep., School of Computer
 *   Science, McGill University, Montreal, Canada.
 *
 * @copyright SuperDuper96 PRNG was proposed by Marsagla et al.
 *
 * Implementation for SmokeRand:
 *
 * (c) 2024-2025 Alexey L. Voskov, Lomonosov Moscow State University.
 * alvoskov@gmail.com
 *
 * This software is licensed under the MIT license.
 */
#include "testu01th/cinterface.h"

PRNG_CMODULE_PROLOG

/**
 * @brief SuperDuper96 PRNG state
 */
typedef struct {
    uint32_t lcg;
    uint32_t xs;
} SuperDuper96State;


static inline unsigned long get_bits32_raw(void *param, void *state)
{
    SuperDuper96State *obj = state;
    obj->lcg = 69069u * obj->lcg + 12345u;
    obj->xs ^= obj->xs >> 13;
    obj->xs ^= obj->xs << 17;
    obj->xs ^= obj->xs >> 5;
    (void) param;
    return obj->lcg + obj->xs;
}


static void *init_state()
{
    SuperDuper96State *obj = intf.malloc(sizeof(SuperDuper96State));
    uint64_t seed = intf.get_seed64();
    obj->lcg = (seed & 0xFFFFFFFF) | 0x1;
    obj->xs = seed >> 32;
    if (obj->xs == 0) {
        obj->xs = 0xDEADBEEF;
    }
    return (void *) obj;
}

MAKE_UINT32_PRNG("SuperDuper96", NULL)
