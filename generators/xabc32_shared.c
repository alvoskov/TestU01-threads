// https://www.electro-tech-online.com/threads/ultra-fast-pseudorandom-number-generator-for-8-bit.124249/
// https://www.stix.id.au/wiki/Fast_8-bit_pseudorandom_number_generator
// https://eternityforest.com/doku/doku.php?id=tech:the_xabc_random_number_generator

#include "testu01th/cinterface.h"

PRNG_CMODULE_PROLOG


typedef struct {
    uint32_t x;
    uint32_t a;
    uint32_t b;
    uint32_t c;
} Xabc32State;



static inline unsigned long get_bits32_raw(void *param, void *state)
{
    Xabc32State *obj = state;    
    obj->a ^= obj->c ^ (obj->x += 0x9E3779B9);
    obj->b += obj->a;
    obj->c = (obj->c + rotr32(obj->b, 9)) ^ obj->a;
    (void) param;
    return obj->c ^ obj->b;

}

static void *init_state(void)
{
    Xabc32State *obj = intf.malloc(sizeof(Xabc32State));
    obj->a = intf.get_seed64();
    obj->b = intf.get_seed64();
    obj->c = intf.get_seed64();
    obj->x = intf.get_seed64();
    for (int i = 0; i < 32; i++) {
        (void) get_bits32_raw(NULL, obj);
    }
    return obj;
}


MAKE_UINT32_PRNG("xabc32", NULL)
