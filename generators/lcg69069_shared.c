#include "testu01_mt_cintf.h"

PRNG_CMODULE_PROLOG

typedef struct {
    uint32_t x;
} Lcg32State;

static long unsigned int get_bits32_raw(void *param, void *state)
{
    Lcg32State *obj = state;
    (void) param;
    obj->x = 69069 * obj->x + 12345;
    return obj->x;
}

static void *init_state()
{
    Lcg32State *obj = intf.malloc(sizeof(Lcg32State));
    obj->x = intf.get_seed64() >> 32;
    return (void *) obj;
}

MAKE_UINT32_PRNG("LCG69069", NULL)
