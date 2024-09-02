/**
 * @brief Just 128-bit LCG.
 */
#include "testu01_mt_cintf.h"

PRNG_CMODULE_PROLOG

typedef struct {
    unsigned __int128 x;
} Lcg128State;


static inline uint64_t get_bits64_raw(void *param, void *state)
{
 //   static const unsigned __int128 a = ((unsigned __int128) 6906969069) * 100000 * 100000 + 6906969069;
    //static const unsigned __int128 a = (((unsigned __int128) 0xdb36357734e34abb) << 64) | 0x0050d0761fcdfc15ull;
    Lcg128State *obj = (Lcg128State *) state;
    (void) param;
    obj->x = obj->x * 18000690696906969069ull + 1;
//    obj->x = obj->x * 0xdefba91144f2b375; 
    return obj->x >> 64;
}


static uint64_t get_bits64(void *param, void *state)
{
    return get_bits64_raw(param, state);
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
    Lcg128State *obj = intf.malloc(sizeof(Lcg128State));
    obj->x = intf.get_seed64() | 0x1;
    return (void *) obj;
}


static void delete_state(void *param, void *state)
{
    (void) param;
    intf.free(state);
}


int EXPORT gen_getinfo(GenInfoC *gi)
{
    static const char name[] = "Lcg128";
    gi->name = name;
    gi->init_state = init_state;
    gi->delete_state = delete_state;
    gi->get_u01 = get_u01;
    gi->get_bits32 = get_bits32;
    gi->get_bits64 = get_bits64;
    return 1;
}

