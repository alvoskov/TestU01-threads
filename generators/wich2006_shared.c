/**
 * @file wich2006.c
 * @brief Wichmann-Hill generator (2006 version).
 * @details This implementation is based on integer arithmetics and returns
 * 32-bit unsigned integers instead of single-precision floats. It passes
 * SmokeRand test batteries.
 *
 * References:
 *
 * 1. B.A. Wichmann, I.D. Hill. Generating good pseudo-random numbers //
 *    Computational Statistics & Data Analysis. 2006. V. 51. N 3. P.1614-1622.
 *    https://doi.org/10.1016/j.csda.2006.05.019.
 *
 * @copyright The algorithm was suggested by B.A. Wichmann and I.D. Hill.
 * 
 * Implementation for SmokeRand:
 *
 * (c) 2025 Alexey L. Voskov, Lomonosov Moscow State University.
 * alvoskov@gmail.com
 *
 * This software is licensed under the MIT license.
 */
#include "testu01th/cinterface.h"

PRNG_CMODULE_PROLOG

typedef struct {
    uint32_t s[4];
} Wich2006State;

enum {
    WH06_A0 = 11600, WH06_MOD0 = 2147483579,
    WH06_A1 = 47003, WH06_MOD1 = 2147483543,
    WH06_A2 = 23000, WH06_MOD2 = 2147483423,
    WH06_A3 = 33000, WH06_MOD3 = 2147483123
};

static unsigned long get_bits32_raw(void *param, void *state)
{
    Wich2006State *obj = state;
    (void) param;
    // Update generator state
    uint64_t s[4] = {obj->s[0], obj->s[1], obj->s[2], obj->s[3]};
    s[0] = (WH06_A0 * s[0]) % WH06_MOD0;
    s[1] = (WH06_A1 * s[1]) % WH06_MOD1;
    s[2] = (WH06_A2 * s[2]) % WH06_MOD2;
    s[3] = (WH06_A3 * s[3]) % WH06_MOD3;
    obj->s[0] = s[0]; obj->s[1] = s[1]; obj->s[2] = s[2]; obj->s[3] = s[3];    
    // Output function
    s[0] = (s[0] << 32) / WH06_MOD0;
    s[1] = (s[1] << 32) / WH06_MOD1;
    s[2] = (s[2] << 32) / WH06_MOD2;
    s[3] = (s[3] << 32) / WH06_MOD3;
    return (s[0] + s[1] + s[2] + s[3]) & 0xFFFFFFFF;
}

static void *init_state(void)
{
    Wich2006State *obj = intf.malloc(sizeof(Wich2006State));
    uint64_t s1 = intf.get_seed64();
    uint64_t s2 = intf.get_seed64();
    obj->s[0] = 1 + (s1 % (WH06_MOD0 - 1));
    obj->s[1] = 1 + ((s1 >> 32) % (WH06_MOD1 - 1));
    obj->s[2] = 1 + (s2 % (WH06_MOD2 - 1));
    obj->s[3] = 1 + ((s2 >> 32) % (WH06_MOD3 - 1));
    return obj;
}

MAKE_UINT32_PRNG("Wich2006", NULL)
