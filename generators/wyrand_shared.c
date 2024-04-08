/**
 * @brief wyrand pseudorandom number generator.
 * @details
 * References:
 * - https://github.com/lemire/testingRNG/blob/master/source/wyrand.h
 * - https://github.com/wangyi-fudan/wyhash/blob/master/wyhash.h
 *
 */
#include "testu01_mt_cintf.h"

/////////////////////////////////////////////////
///// Entry point for -nostdlib compilation /////
/////////////////////////////////////////////////
SHARED_ENTRYPOINT_CODE

static CallerAPI intf;

typedef struct {
    uint64_t x;
} WyRandState;


uint64_t get_bits64(void *param, void *state)
{
    (void) param;
    const uint64_t c = UINT64_C(0xe7037ed1a0b428db);
    WyRandState *obj = (WyRandState *) state;
    obj->x += UINT64_C(0xa0761d6478bd642f);
    __uint128_t t = (__uint128_t) obj->x * (obj->x ^ c);
    return (t >> 64) ^ t;
}

static long unsigned int get_bits32(void *param, void *state)
{
    return get_bits64(param, state) >> 32;
}

static double get_u01(void *param, void *state)
{
    return uint64_to_udouble(get_bits64(param, state));
}

static void *init_state()
{
    WyRandState *obj = (WyRandState *) intf.malloc(sizeof(WyRandState));
    obj->x = intf.get_seed64();
    return (void *) obj;
}

static void delete_state(void *param, void *state)
{
    (void) param;
    intf.free(state);
}

int EXPORT gen_initlib(CallerAPI *intf_)
{
    intf = *intf_;
    return 1;
}

int EXPORT gen_closelib()
{
    return 1;
}

int EXPORT gen_getinfo(GenInfoC *gi)
{
    static const char name[] = "WyRand";
    gi->name = name;
    gi->init_state = init_state;
    gi->delete_state = delete_state;
    gi->get_u01 = get_u01;
    gi->get_bits32 = get_bits32;
    gi->get_bits64 = get_bits64;
    return 1;
}
