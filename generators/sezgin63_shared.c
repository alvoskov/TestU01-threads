/**
 * @file sezgin63_shared.c
 * @brief Implementation of 63-bit LCG with prime modulus.
 * @details Passes SmallCrush and Crush, but systematically gives
 * suspect p-values at BigCrush test N13 (BirthdaySpacings, t = 2).
 *
 * References:
 * - F. Sezgin, T.M. Sezgin. Finding the best portable
 *   congruential random number generators // Computer Physics
 *   Communications. 2013. V. 184. N 8. P. 1889-1897.
 *   https://doi.org/10.1016/j.cpc.2013.03.013.
 *
 * @copyright (c) 2024 Alexey L. Voskov, Lomonosov Moscow State University.
 * alvoskov@gmail.com
 *
 * All rights reserved.
 *
 * This software is provided under the Apache 2 License.
 */
#include "testu01th/cinterface.h"

PRNG_CMODULE_PROLOG

/**
 * @brief 63-bit LCG state.
 */
typedef struct {
    int64_t x; ///< Must be signed!
} Lcg63State;


static inline int64_t get_bits63_raw(void *param, void *state)
{
    static const int64_t m = 9223372036854775783LL; // 2^63 - 25
    static const int64_t a = 3163036175LL; // See Line 4 in Table 1
    static const int64_t b = 2915986895LL;
    static const int64_t c = 2143849158LL;
    Lcg63State *obj = state;    
    (void) param;
    obj->x = a * (obj->x % b) - c*(obj->x / b);
    if (obj->x < 0LL) {
        obj->x += m;
    }
    return obj->x;
}

static inline unsigned long get_bits32(void *param, void *state)
{
    return get_bits63_raw(param, state) >> 31;
}

static void get_array32(void *param, void *state, uint32_t *out, size_t len)
{
    for (size_t i = 0; i < len; i++) {
        out[i] = get_bits63_raw(param, state) >> 31;
    }
}


static double get_u01(void *param, void *state)
{
    uint64_t x = get_bits63_raw(param, state);
    return uint64_to_udouble(x << 1);
}

static void *init_state()
{
    Lcg63State *obj = intf.malloc(sizeof(Lcg63State));
    obj->x = intf.get_seed64();
    return (void *) obj;
}


static void delete_state(void *param, void *state)
{
    (void) param;
    intf.free(state);
}


int EXPORT gen_getinfo(GenInfoC *gi)
{
    static const char name[] = "Sezgin63";
    gi->name = name;
    gi->init_state = init_state;
    gi->delete_state = delete_state;
    gi->get_u01 = get_u01;
    gi->get_bits32 = get_bits32;
    gi->get_array32 = get_array32;
    return 1;
}
