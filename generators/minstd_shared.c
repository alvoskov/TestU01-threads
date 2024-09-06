#include "testu01_mt_cintf.h"

PRNG_CMODULE_PROLOG

typedef struct {
    uint32_t x;
} MinstdState;

static inline unsigned long get_bits32_raw(void *param, void *state)
{
    static const int32_t m = 2147483647, a = 16807, q = 127773, r = 2836;
    MinstdState *obj = (MinstdState *) state;
    (void) param;
    const uint32_t x = obj->x, h = x / q;
    const int32_t t = a * (x - h * q) - h * r;
    obj->x = (t < 0) ? (t + m) : t;
    return obj->x << 1;
}

static void *init_state()
{
    MinstdState *obj = (MinstdState *) intf.malloc(sizeof(MinstdState));
    obj->x = intf.get_seed64() >> 32;
    return (void *) obj;
}


MAKE_UINT32_PRNG("Minstd")
