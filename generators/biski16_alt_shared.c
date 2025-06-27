// https://github.com/danielcota/biski64/tree/main
#include "testu01th/cinterface.h"

PRNG_CMODULE_PROLOG

typedef struct {
    uint16_t loop_mix;
    uint16_t mix;
    uint16_t ctr;
} Biski16State;



static inline uint16_t rotl16(uint16_t x, int r)
{
    return (x << r) | (x >> (16 - r));
}

// biski64 generator function
static inline uint16_t Biski16State_get_bits(Biski16State *obj)
{
    uint16_t output = obj->mix + obj->loop_mix;
    uint16_t old_loop_mix = obj->loop_mix;
    obj->loop_mix = obj->ctr ^ obj->mix;
    obj->mix = (obj->mix ^ rotl16(obj->mix, 4)) + rotl16(old_loop_mix, 9);
    obj->ctr += 0x9999;
    return output;
}


static inline unsigned long get_bits32_raw(void *param, void *state)
{
    (void) param;
    uint32_t hi = Biski16State_get_bits(state);
    uint32_t lo = Biski16State_get_bits(state);
    return (hi << 16) | lo;
}

static void *init_state()
{
    Biski16State *obj = intf.malloc(sizeof(Biski16State));
    obj->loop_mix = intf.get_seed64();
    obj->mix = intf.get_seed64();
    obj->ctr = intf.get_seed64();
    return obj;
}

MAKE_UINT32_PRNG("biski16_alt", NULL)
