/**
 * @file mwc1616_shared.c
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
 * @brief MWC1616 PRNG state.
 */
typedef struct {
    uint32_t z; ///< MWC state 1: c - upper half, x - lower half
    uint32_t w; ///< MWC state 2: c - upper half, x - lower half
} Mwc1616State;


static inline uint32_t get_bits32_raw(void *param, void *state)
{
    Mwc1616State *obj = state;
    (void) param;
    // MWC generators
    obj->z = 36969 * (obj->z & 0xFFFF) + (obj->z >> 16);
    obj->w = 18000 * (obj->w & 0xFFFF) + (obj->w >> 16);
    uint32_t mwc = (obj->z << 16) + obj->w;
    return mwc;
}


static void *init_state()
{
    Mwc1616State *obj = intf.malloc(sizeof(Mwc1616State));
    uint64_t seed0 = intf.get_seed64(); // For MWC
    obj->z = (seed0 & 0xFFFF) | 0x10000; // MWC generator 1: prevent bad seeds
    obj->w = ((seed0 >> 16) & 0xFFFF) | 0x10000; // MWC generator 2: prevent bad seeds
    return (void *) obj;
}


MAKE_UINT32_PRNG("MWC1616", NULL)
