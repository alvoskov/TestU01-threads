/**
 * @file mwc1616x_shared.c
 * @brief KISS99 pseudorandom number generator by George Marsaglia.
 * It passes SmallCrush, Crush and BigCrush batteries, has period about 2^123
 * and doesn't require 64-bit arithmetics.
 * @details Description by George Marsaglia:
 *
 * The KISS generator, (Keep It Simple Stupid), is designed to combine
 * the two multiply-with-carry generators in MWC with the 3-shift register
 * SHR3 and the congruential generator CONG, using addition and exclusive-or.
 * Period about 2^123. It is one of my favorite generators.
 * 
 * References:
 *
 * - https://groups.google.com/group/sci.stat.math/msg/b555f463a2959bb7/
 * - http://www0.cs.ucl.ac.uk/staff/d.jones/GoodPracticeRNG.pdf
 *
 * @copyright (c) 2024 Alexey L. Voskov, Lomonosov Moscow State University.
 * alvoskov@gmail.com
 *
 * This software is licensed under the MIT license.
 *
 * The KISS99 algorithm is developed by George Marsaglia.
 */
#include "testu01th/cinterface.h"

PRNG_CMODULE_PROLOG

/**
 * @brief MWC1616X state.
 */
typedef struct {
    uint16_t z_hi;
    uint16_t z_lo;
    uint16_t w_hi;
    uint16_t w_lo;
} Mwc1616xShared;


static inline uint32_t get_bits32_raw(void *param, void *state)
{
    Mwc1616xShared *obj = state;
    (void) param;
    // MWC generators
    uint32_t z_prod = 61578 * (obj->z_lo) + (obj->z_hi);
    obj->z_hi = z_prod >> 16; obj->z_lo = z_prod & 0xFFFF;
    uint32_t w_prod = 63885 * (obj->w_lo) + (obj->w_hi);
    obj->w_hi = w_prod >> 16; obj->w_lo = w_prod & 0xFFFF;
    
    uint32_t mwc = ((obj->z_lo ^ obj->w_hi) << 16) | (obj->w_lo ^ obj->z_hi);
    return mwc;
}


static void *init_state(void)
{
    Mwc1616xShared *obj = intf.malloc(sizeof(Mwc1616xShared));
    uint32_t seed0 = intf.get_seed64();
    obj->z_hi = 1;
    obj->z_lo = seed0 >> 16;
    obj->w_hi = 1;
    obj->w_lo = seed0;
    return (void *) obj;
}


MAKE_UINT32_PRNG("Mwc1616x", NULL)
