/**
 * @file sapparot2_shared.c
 * @brief http://www.literatecode.com/sapparot
 * @details
 *
 * @copyright (c) 2024 Alexey L. Voskov, Lomonosov Moscow State University.
 * alvoskov@gmail.com
 *
 * This software is licensed under the MIT license.
 */
#include "testu01th/cinterface.h"

PRNG_CMODULE_PROLOG

typedef struct {
    uint32_t a;
    uint32_t b;
    uint32_t c;
} Sapparot2State;

#define C_RTR 7
#define C_SH 27
#define PHI 0x9e3779b9

static inline unsigned long get_bits32_raw(void *param, void *state)
{
    uint32_t m;
    Sapparot2State *obj = state;
    (void) param;
    obj->c += obj->a;
    obj->c = rotl32(obj->c, obj->b >> C_SH);
    obj->b = (obj->b + ((obj->a << 1) + 1)) ^ rotl32(obj->b, 5);
    obj->a += PHI;
    obj->a = rotl32(obj->a, C_RTR);
    m = obj->a;
    obj->a = obj->b;
    obj->b = m;
    return obj->c ^ obj->b ^ obj->a;
}

static void *init_state()
{
    Sapparot2State *obj = intf.malloc(sizeof(Sapparot2State));
    uint64_t seed = intf.get_seed64();    
    obj->a = seed >> 32;
    obj->b = seed & 0xFFFFFFFF;
    obj->c = intf.get_seed64();
    return (void *) obj;
}

MAKE_UINT32_PRNG("sapparot2", NULL)
