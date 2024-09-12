/**
 * @file xorwow_shared.c
 * @brief xorwow pseudorandom number generator.
 * @details Fails the next test from BigCrush:
 * - 81  LinearComp, r = 29             1 - eps1
 *
 * References:
 *
 * 1. Marsaglia G. Xorshift RNGs // Journal of Statistical Software. 2003.
 *    V. 8. N 14. P. 1-6. https://doi.org/10.18637/jss.v008.i14
 * 2. cuRAND library programming guide.
 *    https://docs.nvidia.com/cuda/curand/testing.html
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
 * @brief xorwow PRNG state.
 */
typedef struct {
    uint32_t x; ///< Xorshift register
    uint32_t y; ///< Xorshift register
    uint32_t z; ///< Xorshift register
    uint32_t w; ///< Xorshift register
    uint32_t v; ///< Xorshift register
    uint32_t d; ///< "Weyl sequence" counter
} XorWowState;


static inline unsigned long get_bits32_raw(void *param, void *state)
{
    const uint32_t d_inc = 362437;
    XorWowState *obj = (XorWowState *) state;
    (void) param;
    uint32_t t = (obj->x ^ (obj->x >> 2));
    obj->x = obj->y;
    obj->y = obj->z;
    obj->z = obj->w;
    obj->w = obj->v;
    obj->v = (obj->v ^ (obj->v << 4)) ^ (t ^ (t << 1));
    return (obj->d += d_inc) + obj->v;
}

static void *init_state(void)
{
    XorWowState *obj = intf.malloc(sizeof(XorWowState));
    uint64_t s1 = intf.get_seed64();
    uint64_t s2 = intf.get_seed64();
    uint64_t s3 = intf.get_seed64();
    obj->x = (uint32_t) s1;
    obj->y = (uint32_t) (s1 >> 32);
    obj->z = (uint32_t) s2;
    obj->w = (uint32_t) (s2 >> 32);
    obj->v = (uint32_t) s3;
    obj->d = (uint32_t) (s3 >> 32);
    return (void *) obj;
}

MAKE_UINT32_PRNG("xorwow", NULL)
