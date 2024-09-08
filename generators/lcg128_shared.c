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
    Lcg128State *obj = state;
    (void) param;
    obj->x = obj->x * 18000690696906969069ull + 1;
//    obj->x = obj->x * 0xdefba91144f2b375; 
    return obj->x >> 64;
}


static void *init_state()
{
    Lcg128State *obj = intf.malloc(sizeof(Lcg128State));
    obj->x = intf.get_seed64() | 0x1;
    return (void *) obj;
}

MAKE_UINT64_UPTO32_PRNG("Lcg128", NULL)
