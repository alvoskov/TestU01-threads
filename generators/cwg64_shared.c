/**
 * @file cwg64.c
 * @brief "Collatz-Weyl" generator.
 * @details A fast 64-bit generator with period not less than 2^{64}
 *
 * References:
 * 
 * 1. T.R. Dziala. Collatz-Weyl Generators: High Quality and High Throughput
 *    Parameterized Pseudorandom Number Generators
 *    https://arxiv.org/abs/2312.17043
 *
 * @copyright
 * (c) 2024-2025 Alexey L. Voskov, Lomonosov Moscow State University.
 * alvoskov@gmail.com
 *
 * This software is licensed under the MIT license.
 */
#include "testu01th/cinterface.h"

PRNG_CMODULE_PROLOG

typedef struct {    
    uint64_t x;
    uint64_t a;
    uint64_t w;
} Cwg64State;

static inline unsigned long get_bits32_raw(void *param, void *state)
{    
    Cwg64State *obj = state;
    (void) param;
    obj->w += 0x9E3779B97F4A7C15;
    obj->a += obj->x;
    obj->x = (obj->x >> 1) * (obj->x | 1) ^ obj->w;
    uint64_t u = (obj->a >> 48) ^ obj->x;
    return u >> 32;
}

static void *init_state()
{
    Cwg64State *obj = intf.malloc(sizeof(Cwg64State));
    obj->x = intf.get_seed64();
    obj->a = intf.get_seed64();
    obj->w = intf.get_seed64();
    for (int i = 0; i < 48; i++) {
        (void) get_bits32_raw(NULL, obj);
    }
    return (void *) obj;
}

MAKE_UINT32_PRNG("CWG64", NULL)
