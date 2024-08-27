#include "testu01_mt_cintf.h"

PRNG_CMODULE_PROLOG

typedef struct {
    uint32_t x;
} MinstdState;

static long unsigned int get_bits32(void *param, void *state)
{
    static const int32_t m = 2147483647, a = 16807, q = 127773, r = 2836;
    MinstdState *obj = (MinstdState *) state;
    (void) param;
    const uint32_t x = obj->x, h = x / q;
    const int32_t t = a * (x - h * q) - h * r;
    obj->x = (t < 0) ? (t + m) : t;
    return obj->x << 1;
}


static double get_u01(void *param, void *state)
{
    return uint32_to_udouble(get_bits32(param, state));
}


static void *init_state()
{
    MinstdState *obj = (MinstdState *) intf.malloc(sizeof(MinstdState));
    obj->x = intf.get_seed64() >> 32;
    return (void *) obj;
}


static void delete_state(void *param, void *state)
{
    (void) param;
    intf.free(state);
}


int EXPORT gen_getinfo(GenInfoC *gi)
{
    static const char name[] = "Minstd";
    gi->name = name;
    gi->init_state = init_state;
    gi->delete_state = delete_state;
    gi->get_u01 = get_u01;
    gi->get_bits32 = get_bits32;
    return 1;
}
