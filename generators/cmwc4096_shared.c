/**
 * @file cmwc4096_shared.c
 * @brief CMWC4096 ("Mother-of-All") PRNG implementation.
 * It has good statistical properties, huge period and high performance.
 * @details The CMWC4096 algorithm is developed by G.Marsaglia:
 *
 * - George Marsaglia. Random Number Generators // Journal of Modern Applied
 *   Statistical Methods. 2003. V. 2. N 1. P. 2-13.
 *   https://doi.org/10.22237/jmasm/1051747320
 *
 * @copyright Based on scientific publication by G.Marsaglia.
 *
 * Implementation for TestU01-threads:
 * (c) 2024 Alexey L. Voskov, Lomonosov Moscow State University.
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

typedef struct {
    uint32_t Q[4096];
    uint32_t c;
    uint32_t i;
} Cmwc4096State;

static inline unsigned long get_bits32_raw(void *param, void *state)
{
    const uint64_t a = 18782ull;
    Cmwc4096State *obj = state;
    (void) param;
    obj->i = (obj->i + 1) & 4095;
    uint64_t t = a * obj->Q[obj->i] + obj->c;
    obj->c = t >> 32;
    uint32_t x = t + obj->c;
    if (x < obj->c) {
        x++;
        obj->c++;
    }
    return obj->Q[obj->i] = 0xfffffffe - x;
}

static void *init_state()
{
    Cmwc4096State *obj = intf.malloc(sizeof(Cmwc4096State));
    uint32_t state = intf.get_seed64();
    for (size_t i = 0; i < 4096; i++) {
        state = 69069 * state + 1;
        obj->Q[i] = state;
    }
    obj->c = 123;
    obj->i = 4095;
    return (void *) obj;
}

MAKE_UINT32_PRNG("CMWC4096", NULL)
