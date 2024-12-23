/**
 * @file lfsr258_shared.c
 * @brief Combined 64-bit LFSR generator. Fails linear complexity
 * and matrix rank tests.
 *
 * References:
 * 1. L'Ecuyer P. Tables of Maximally Equidistributed Combined LFSR
 *    Generators // Mathematics of Computation. 1999. V. 68. N 225.
 *    P.261-269
 * 2. https://www-labs.iro.umontreal.ca/~simul/rng/lfsr258.c
 */
#include "testu01th/cinterface.h"

PRNG_CMODULE_PROLOG

/**
 * @brief LFSR113 PRNG state.
 */
typedef struct {
    uint64_t y[5];
} Lfsr258State;

/**
 * @brief Creates LFSR example with taking into account
 * limitations on seeds (initial state).
 */
void *init_state()
{
    uint64_t seeds_lb[] = {0x1, 0x1FF, 0xFFF, 0x1FFFF, 0x7FFFFF};
    Lfsr258State *obj = intf.malloc(sizeof(Lfsr258State));
    for (int i = 0; i < 5; i++) {
        do {
            obj->y[i] = intf.get_seed64();
        } while (obj->y[i] <= seeds_lb[i]);
    }
    return obj;
}


#define ROUND(i, shl1, shr, mask, shl2) \
    b = ((y[(i)] << (shl1)) ^ y[(i)]) >> (shr); \
    y[(i)] = ((y[(i)] & mask) << (shl2)) ^ b;

static inline uint64_t get_bits64_raw(void *param, void *state)
{
    Lfsr258State *obj = state;
    uint64_t *y = obj->y, b;
    (void) param;
    ROUND(0,  1, 53, 0xFFFFFFFFFFFFFFFE, 10)
    ROUND(1, 24, 50, 0xFFFFFFFFFFFFFE00, 5)
    ROUND(2,  3, 23, 0xFFFFFFFFFFFFF000, 29)
    ROUND(3,  5, 24, 0xFFFFFFFFFFFE0000, 23)
    ROUND(4,  3, 33, 0xFFFFFFFFFF800000, 8);
    return y[0] ^ y[1] ^ y[2] ^ y[3] ^ y[4];
}

/**
 * @brief Internal self-test based on the original code 
 * by P.L'Ecuyer.
 */
static int run_self_test()
{
    const uint64_t seed = 123456789123456789ull;
    Lfsr258State obj = {{seed, seed, seed, seed, seed}};
    uint64_t u_ref[] = {
        0xEB3C31E8FDA1078C, 0xE2EE79241DC0EBF1,
        0x18E38AA3FC7562DB, 0x5A0DB4C898770E81,
        0xE9AC291C6241F0C4, 0xA98DD55E73FBDC7A,
        0x861718EE328C0912, 0xA4F9821B624D0E78
    };
    int is_ok = 1;
    for (int i = 0; i < 10000; i++) {
        get_bits64_raw(NULL, &obj);
    }
    intf.printf("%16s %16s\n", "Output", "Reference");
    for (int i = 0; i < 8; i++) {
        uint64_t u = get_bits64_raw(NULL, &obj);
        intf.printf("0x%.16llX 0x%.16llX\n", u, u_ref[i]);
        if (u != u_ref[i]) {
            is_ok = 0;
        }
    }
    return is_ok;
}



MAKE_UINT64_UPTO32_PRNG("LFSR258", run_self_test)
