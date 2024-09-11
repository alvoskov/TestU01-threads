/**
 * @file minstd_shared.c
 * @brief Obsolete "minimal standard" 31-bit LCG with prime modulus.
 * It is \f$ LCG(2^{31} - 1, 16807, 0) \f$. Fails SmallCrush, Crush,
 * BigCrush and PractRand and fairly slow on modern 64-bit processors. 
 * @details This implementation of minstd is based on "Integer version 2"
 * from [1]. It uses Schrage's method to be able to use only 32-bit
 * arithmetics.
 *
 * Reference:
 *
 * 1. S. K. Park, K. W. Miller. Random number generators: good ones
 *    are hard to find // Communications of the ACM. 1988. V. 31. N 10.
 *    P.1192-1201. https://doi.org/10.1145/63039.63042
 *
 * @copyright (c) 2024 Alexey L. Voskov, Lomonosov Moscow State University.
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
    uint32_t x;
} MinstdState;


static inline unsigned long get_bits32_raw(void *param, void *state)
{
    static const int32_t m = 2147483647, a = 16807, q = 127773, r = 2836;
    MinstdState *obj = state;
    (void) param;
    const uint32_t hi = obj->x / q;
    const uint32_t lo = obj->x - q * hi; // It is x mod q
    const int32_t t = a * lo - r * hi;
    obj->x = (t < 0) ? (t + m) : t;
    return obj->x << 1; // 
}


static void *init_state()
{
    MinstdState *obj = intf.malloc(sizeof(MinstdState));
    obj->x = intf.get_seed64() >> 33;
    return (void *) obj;
}


int run_self_test()
{
    const uint32_t x_ref = 1043618065;
    MinstdState obj;
    obj.x = 1;
    for (size_t i = 0; i < 10000; i++) {
        get_bits32_raw(NULL, &obj);
    }
    intf.printf("The current state is %d, reference value is %d\n",
        obj.x, x_ref);
    return obj.x == x_ref;
}


MAKE_UINT32_PRNG("Minstd", run_self_test)
