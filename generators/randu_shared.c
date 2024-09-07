#include "testu01_mt_cintf.h"

PRNG_CMODULE_PROLOG

typedef struct {
    uint32_t x;
} RanduState;

static inline unsigned long get_bits32_raw(void *param, void *state)
{
    RanduState *obj = state;
    (void) param;
    obj->x = 65539 * obj->x + 12345;
    return obj->x;
}

static void *init_state()
{
    RanduState *obj = intf.malloc(sizeof(RanduState));
    obj->x = intf.get_seed64() >> 32;
    return (void *) obj;
}

MAKE_UINT32_PRNG("RANDU", NULL)
