/**
 * @file kiss99_shared
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
 * The KISS99 algorithm is developed by George Marsaglia.
 *
 * All rights reserved.
 *
 * This software is provided under the Apache 2 License.
 */
#include "testu01_mt_cintf.h"

PRNG_CMODULE_PROLOG

/**
 * @brief KISS99 PRNG state.
 * @details Contains states of 3 PRNG: LCG, SHR3, MWC.
 * z, w and jsr musn't be initialized with zeros.
 */
typedef struct {
    uint32_t z;     ///< MWC state 1
    uint32_t w;     ///< MWC state 2
    uint32_t jsr;   ///< SHR3 state
    uint32_t jcong; ///< LCG state
} KISS99State;


static inline unsigned long get_bits32_raw(void *param, void *state)
{
    KISS99State *obj = state;
    (void) param;
    // LCG generator
    obj->jcong = 69069 * obj->jcong + 1234567;
    // MWC generator
    obj->z = 36969 * (obj->z & 0xFFFF) + (obj->z >> 16);
    obj->w = 18000 * (obj->w & 0xFFFF) + (obj->w >> 16);
    uint32_t mwc = (obj->z << 16) + obj->w;
    // SHR3 generator
    uint32_t jsr = obj->jsr;
    jsr ^= (jsr << 17);
    jsr ^= (jsr >> 13);
    jsr ^= (jsr << 5);
    obj->jsr = jsr;
    // Output (combination of generators)
    return (mwc ^ obj->jcong) + jsr;
}


static void *init_state()
{
    KISS99State *obj = intf.malloc(sizeof(KISS99State));
    do { obj->z = intf.get_seed64(); } while (obj->z == 0 || obj->z == 0xFFFFFFFF);
    do { obj->w = intf.get_seed64(); } while (obj->w == 0 || obj->z == 0xFFFFFFFF);
    do { obj->jsr = intf.get_seed64(); } while (obj->jsr == 0);
    obj->jcong = intf.get_seed64();
    return (void *) obj;
}


/**
 * @brief An internal self-test, taken from Marsaglia post.
 */
static int run_self_test()
{
    const uint32_t refval = 1372460312U;
    uint32_t val = 0;
    KISS99State obj;
    obj.z   = 12345; obj.w     = 65435;
    obj.jsr = 34221; obj.jcong = 12345; 
    for (size_t i = 1; i < 1000001 + 256; i++) {
        val = get_bits32_raw(NULL, &obj);
    }
    intf.printf("Reference value: %u\n", refval);
    intf.printf("Obtained value:  %u\n", val);
    intf.printf("Difference:      %u\n", refval - val);
    return refval == val;
}


MAKE_UINT32_PRNG("KISS99", run_self_test)
