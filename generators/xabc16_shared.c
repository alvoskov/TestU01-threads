// https://www.electro-tech-online.com/threads/ultra-fast-pseudorandom-number-generator-for-8-bit.124249/
// https://www.stix.id.au/wiki/Fast_8-bit_pseudorandom_number_generator
// https://eternityforest.com/doku/doku.php?id=tech:the_xabc_random_number_generator

#include "testu01th/cinterface.h"

PRNG_CMODULE_PROLOG


typedef struct {
    uint16_t x;
    uint16_t a;
    uint16_t b;
    uint16_t c;
} Xabc16State;



static inline uint16_t get_bits16(Xabc16State *obj)
{
    obj->a ^= obj->c ^ (obj->x += 0x9E37);
    obj->b += obj->a;
    obj->c = (obj->c + ((obj->b << 11) | (obj->b >> 5))) ^ obj->a;
    return obj->c ^ obj->b;
}


static inline unsigned long get_bits32_raw(void *param, void *state)
{
    uint32_t hi = get_bits16(state);
    uint32_t lo = get_bits16(state);
    (void) param;
    return (hi << 16) | lo;
}


static void *init_state(void)
{
    Xabc16State *obj = intf.malloc(sizeof(Xabc16State));
    uint64_t s = intf.get_seed64();
    obj->a = s & 0xFFFF;
    obj->b = (s >> 16) & 0xFFFF;
    obj->c = (s >> 32) & 0xFFFF;
    obj->x = s >> 48;
    for (int i = 0; i < 32; i++) {
        (void) get_bits32_raw(NULL, obj);
    }
    return obj;
}


MAKE_UINT32_PRNG("xabc16", NULL)
