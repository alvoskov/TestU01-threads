/**
Marsaglia G., Zaman A. Some portable very-long-period random number generators //
Comput. Phys. 1994. V. 8. N 1. P. 117-121. https://doi.org/10.1063/1.168514},
*/


#include "testu01th/cinterface.h"


PRNG_CMODULE_PROLOG


typedef struct {
    uint32_t x;
    uint32_t y;
    uint32_t z;
    uint32_t c;
    uint32_t n;
} Mzran13State;


static unsigned long get_bits32_raw(void *param, void *state)
{
    Mzran13State *obj = state;
    uint32_t s;
    (void) param;
    if (obj->y > obj->x + obj->c) {
        s = obj->y - (obj->x + obj->c);      obj->c = 0;
    } else {
        s = obj->y - (obj->x + obj->c) - 18; obj->c = 1;
    }
    obj->x = obj->y; obj->y = obj->z; obj->z = s;
    obj->n = 69069u * obj->n  + 1013904243u;
    return obj->z + obj->n;    
}


static void *init_state()
{
    Mzran13State *obj = intf.malloc(sizeof(Mzran13State));
    uint64_t seed0 = intf.get_seed64();
    uint64_t seed1 = intf.get_seed64();
    obj->x = seed0 & 0x7FFFFFFF;
    obj->y = (seed0 >> 32) & 0x7FFFFFFF;
    obj->z = seed1 & 0x7FFFFFFF;
    obj->c = 1;
    obj->n = seed1 >> 32;
    return obj;
}

MAKE_UINT32_PRNG("Mzran13", NULL)
