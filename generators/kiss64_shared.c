/**
 * @file kiss64_shared
 * @brief 64-bit version of KISS pseudorandom number generator
 * by George Marsaglia. It passes SmallCrush, Crush and BigCrush batteries,
 * has period about 2^123
 * @details Description by George Marsaglia:
 *
 * References:
 *
 * - https://groups.google.com/g/comp.lang.fortran/c/qFv18ql_WlU
 * - https://www.thecodingforums.com/threads/64-bit-kiss-rngs.673657/
 * - https://ssau.ru/pagefiles/sbornik_pit_2021.pdf
 */
#include "testu01_mt_cintf.h"

PRNG_CMODULE_PROLOG

/**
 * @brief KISS64 PRNG state.
 * @details Contains states of 3 PRNG: LCG, XSH, MWC.
 * z, w and jsr musn't be initialized with zeros.
 */
typedef struct {
    uint64_t x;  ///< MWC state 1
    uint64_t c;  ///< MWC state 2
    uint64_t y;  ///< XSH state
    uint64_t z;  ///< LCG state
} KISS64State;


static uint64_t get_bits64_raw(void *param, void *state)
{
    KISS64State *obj = state;
    (void) param;
    // MWC generator
    uint64_t t = (obj->x << 58) + obj->c;
    obj->c = obj->x >> 6;
    obj->x += t;
    obj->c += (obj->x < t);
    // XSH generator
    uint64_t y = obj->y;
    y ^= (y << 13);
    y ^= (y >> 17);
    y ^= (y << 43);
    obj->y = y;
    // LCG generator
    obj->z = 6906969069LL * obj->z + 1234567;
    // Output (combination of generators)
    return (obj->x + obj->y + obj->z);
}


static void *init_state()
{
    const uint64_t mask58 = 0x3FFFFFFFFFFFFFFULL;
    KISS64State *obj = (KISS64State *) intf.malloc(sizeof(KISS64State));
    do { obj->x = intf.get_seed64(); } while (obj->x == 0);
    do { obj->c = intf.get_seed64() & mask58; } while (obj->c == 0);
    do { obj->y = intf.get_seed64(); } while (obj->y == 0);
    obj->z = intf.get_seed64();
    return (void *) obj;
}


/**
 * @brief An internal self-test, taken from Marsaglia post.
 */
static int run_self_test()
{
    const uint64_t refval = 1666297717051644203ULL;
    KISS64State obj;
    obj.x = 1234567890987654321ULL; obj.c = 123456123456123456ULL;
    obj.y = 362436362436362436ULL;  obj.z = 1066149217761810ULL;
    uint64_t val;
    for (size_t i = 0; i < 100000000; i++) {
        val = get_bits64_raw(NULL, &obj);
    }
    intf.printf("Reference value: %llu\n", refval);
    intf.printf("Obtained value:  %llu\n", val);
    intf.printf("Difference:      %llu\n", refval - val);
    return refval == val;
    return 1;
}


MAKE_UINT64_UPTO32_PRNG("KISS64", run_self_test)
