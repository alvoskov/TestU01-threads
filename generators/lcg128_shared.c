/**
 * @file lcg128_shared.c
 * @brief Just 128-bit LCG with \f$ m = 2^{128}\f$ and easy to memorize
 * multiplier 18000 69069 69069 69069 (suggested by A.L. Voskov)
 * @details It passes SmallCrush, Crush and BigCrush. However, its higher
 * 64 bits fail PractRand 0.94 at 128GiB sample. Usage of slightly better
 * (but hard to memorize) multiplier 0xfc0072fa0b15f4fd from 
 * https://doi.org/10.1002/spe.3030 doesn't improve PractRand 0.94 results.
 *
 * (c) 2024 Alexey L. Voskov, Lomonosov Moscow State University.
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

/**
 * @brief 128-bit LCG state.
 */
typedef struct {
    unsigned __int128 x;
} Lcg128State;


static inline uint64_t get_bits64_raw(void *param, void *state)
{
    Lcg128State *obj = state;
    const uint64_t a = 1800690696906969069ull;
    (void) param;
    obj->x = a * obj->x  + 1; 
    return (uint64_t) (obj->x >> 64);
}


static void *init_state(void)
{
    Lcg128State *obj = intf.malloc(sizeof(Lcg128State));
    obj->x = intf.get_seed64() | 0x1;
    return (void *) obj;
}


/**
 * @brief Self-test to prevent problems during re-implementation
 * in MSVC and other plaforms that don't support int128. It also
 * doesn't initialize i32buf: we don't need them anyway.
 */
static int run_self_test(void)
{
    Lcg128State obj = {.x = 1234567890};
    uint64_t u, u_ref = 0xA2BDB4061ECC6BF2;
    for (size_t i = 0; i < 1000000; i++) {
        u = get_bits64_raw(NULL, &obj);
    }
    intf.printf("Result: %llX; reference value: %llX\n", u, u_ref);
    return u == u_ref;
}


MAKE_UINT64_UPTO32_PRNG("Lcg128", run_self_test)
