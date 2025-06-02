// https://www.electro-tech-online.com/threads/ultra-fast-pseudorandom-number-generator-for-8-bit.124249/
// https://www.stix.id.au/wiki/Fast_8-bit_pseudorandom_number_generator
// https://eternityforest.com/doku/doku.php?id=tech:the_xabc_random_number_generator

#include "testu01th/cinterface.h"

PRNG_CMODULE_PROLOG


typedef struct {
    uint8_t x;
    uint8_t a;
    uint8_t b;
    uint8_t c;
} Xabc8State;



static inline uint8_t get_bits8(Xabc8State *obj)
{
    obj->a ^= obj->c ^ (obj->x += 151);
    obj->b += obj->a;
    obj->c = (obj->c + ((obj->b << 7) | (obj->b >> 1))) ^ obj->a;
    return obj->c ^ obj->b;
}


static inline unsigned long get_bits32_raw(void *param, void *state)
{
    union {
        uint8_t  u8[4];
        uint32_t u32;
    } out;
    for (int i = 0; i < 4; i++) {
        out.u8[i] = get_bits8(state);
    }
    (void) param;
    return out.u32;
}


static void *init_state(void)
{
    Xabc8State *obj = intf.malloc(sizeof(Xabc8State));
    uint64_t s = intf.get_seed64();
    obj->a = s & 0xFF;
    obj->b = (s >> 8) & 0xFF;
    obj->c = (s >> 16) & 0xFF;
    obj->x = (s >> 24) & 0xFF;
    for (int i = 0; i < 32; i++) {
        (void) get_bits32_raw(NULL, obj);
    }
    return obj;
}


MAKE_UINT32_PRNG("xabc8", NULL)
