/**
 * @brief KISS93 pseudorandom number generator. It passes SmallCrush
 * but fails the LinearComp (r = 29) test in the Crush battery.
 */
#include "testu01_mt_cintf.h"

PRNG_CMODULE_PROLOG

typedef struct {
    uint32_t S1;
    uint32_t S2;
    uint32_t S3;
} KISS93State;

static inline unsigned long get_bits32_raw(void *param, void *state)
{
    KISS93State *obj = (KISS93State *) state;
    (void) param;
    obj->S1 = 69069 * obj->S1 + 23606797;
    uint32_t b = obj->S2 ^ (obj->S2 << 17);
    obj->S2 = (b >> 15) ^ b;
    b = ((obj->S3 << 18) ^ obj->S3) & 0x7fffffffU;
    obj->S3 = (b >> 13) ^ b;
    return obj->S1 + obj->S2 + obj->S3;
}


static void *init_state()
{
    KISS93State *obj = (KISS93State *) intf.malloc(sizeof(KISS93State));
    obj->S1 = 12345;
    obj->S2 = 6789;
    obj->S3 = 111213;
    return (void *) obj;
}

MAKE_UINT32_PRNG("KISS93", NULL)
