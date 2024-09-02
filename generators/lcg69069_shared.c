#include "testu01_mt_cintf.h"

PRNG_CMODULE_PROLOG

typedef struct {
    uint32_t x;
} Lcg32State;

static long unsigned int get_bits32(void *param, void *state)
{
    Lcg32State *obj = state;
    (void) param;
    obj->x = 69069 * obj->x + 12345;
    return obj->x;
}


static double get_u01(void *param, void *state)
{
    return uint32_to_udouble(get_bits32(param, state));
}


static void *init_state()
{
    Lcg32State *obj = intf.malloc(sizeof(Lcg32State));
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
    static const char name[] = "LCG69069";
    gi->name = name;
    gi->init_state = init_state;
    gi->delete_state = delete_state;
    gi->get_u01 = get_u01;
    gi->get_bits32 = get_bits32;
    return 1;
}
