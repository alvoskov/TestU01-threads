/**
 * @file xkiss16_awc.c
 * @brief xoroshiro32++ is a modification of xoroshiro++ with 32-bit state.
 * @details The parameters were taken from [1,2]. The generator has a very
 * short period around \f$ 2^{32} \f$ and shouldn't be used as a general
 * purpose generator. However, it is much better than LCG69069, minstd or
 * xorshift32 and still may be useful for retrocomputing and microcontrollers.
 *
 * References:
 *
 * 1. https://forums.parallax.com/discussion/comment/1448759/#Comment_1448759
 * 2. https://github.com/ZiCog/xoroshiro/blob/master/src/main/c/xoroshiro.h
 * 3. https://groups.google.com/g/prng/c/Ll-KDIbpO8k/m/bfHK4FlUCwAJ
 * 4. https://forums.parallax.com/discussion/comment/1423789/#Comment_1423789
 *
 * @copyright
 * (c) 2025 Alexey L. Voskov, Lomonosov Moscow State University.
 * alvoskov@gmail.com
 *
 * This software is licensed under the MIT license.
 */
#include "testu01th/cinterface.h"

PRNG_CMODULE_PROLOG

#define K16_AWC_MASK 0xffff
#define K16_AWC_SH 16
#define K16_WEYL_INC 0x9E39


typedef struct {
    uint16_t weyl; ///< Discrete Weyl sequence state.
    uint16_t s[2]; ///< xoroshiro32+ state.
    uint16_t awc_x0; ///< AWC state, \f$ x_{n-1}) \f$.
    uint16_t awc_x1; ///< AWC state, \f$ x_{n-2}) \f$.
    uint16_t awc_c; ///< AWC state, carry.
} Xkiss16State;


static inline uint16_t rotl16(uint16_t x, int r)
{
    return (x << r) | (x >> (16 - r));
}

static inline uint16_t Xkiss16State_get_bits(Xkiss16State *obj)
{
    // xorshift32+ part
    uint16_t s0 = obj->s[0], s1 = obj->s[1];
    s1 ^= s0;
    obj->s[0] = rotl16(s0, 13) ^ s1 ^ (s1 << 5); // a, b
    obj->s[1] = rotl16(s1, 10); // c
    // AWC (add with carry) part
    uint32_t t = obj->awc_x0 + obj->awc_x1 + obj->awc_c;
    obj->awc_x1 = obj->awc_x0;
    obj->awc_c  = t >> K16_AWC_SH;
    obj->awc_x0 = t & K16_AWC_MASK;
    // Discrete Weyl sequence part
    obj->weyl += K16_WEYL_INC;
    // Combined output
    return obj->awc_x0 + obj->s[0] + obj->s[1] + obj->weyl;
}

static inline unsigned long get_bits32_raw(void *param, void *state)
{
    uint32_t hi = Xkiss16State_get_bits(state);
    uint32_t lo = Xkiss16State_get_bits(state);
    (void) param;
    return (hi << 16) | lo;
}


static void *init_state()
{
    Xkiss16State *obj = intf.malloc(sizeof(Xkiss16State));
    uint64_t seed = intf.get_seed64();
    obj->s[0] = seed >> 16;
    obj->s[1] = seed & 0xFFFF;
    if (obj->s[0] == 0 && obj->s[1] == 0) {
        obj->s[0] = 0xDEAD;
        obj->s[1] = 0xBEEF;
    }
    obj->awc_x0 = 3;
    obj->awc_x1 = 2;
    obj->awc_c = 1;
    obj->weyl = 0;
    return obj;
}

MAKE_UINT32_PRNG("xkiss16", NULL)
