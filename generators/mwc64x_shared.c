/**
 * @file mwc64x_shared.c
 * @details
 * birthdayspacing t=3 (N12): without XORing
 * References:
 * 1. https://cas.ee.ic.ac.uk/people/dt10/research/rngs-gpu-mwc64x.html
 * 2. https://prng.di.unimi.it/MWC128.c
 * 3. https://www.grc.com/otg/Marsaglia_MWC_Generators.pdf
 */

#include "testu01_mt_cintf.h"

PRNG_CMODULE_PROLOG

typedef struct {
    uint64_t data;
} MWC64XState;

static inline uint32_t get_bits32_raw(void *param, void *state)
{
    const uint64_t A0 = 0xff676488; // 2^32 - 10001272
    MWC64XState *obj = state;
    (void) param;
    uint32_t c = obj->data >> 32;
    uint32_t x = obj->data & 0xFFFFFFFF;
    obj->data = A0 * x + c;
    return x ^ c; // XORing masks issue with BirthdaySpacing (t=3, Crush N12)
}

EXPORT long unsigned int get_bits32(void *param, void *state)
{
    return get_bits32_raw(param, state);
}


static double get_u01(void *param, void *state)
{
    return uint32_to_udouble(get_bits32_raw(param, state));
}

static void get_array32(void *param, void *state, uint32_t *out, size_t len)
{
    for (size_t i = 0; i < len; i++) {
        out[i] = get_bits32_raw(param, state);
    }
}

static void *init_state()
{
    MWC64XState *obj = intf.malloc(sizeof(MWC64XState));
    do {
        obj->data = intf.get_seed64();
    } while (obj->data == 0 || obj->data == 0xFFFFFFFFFFFFFFFF);
    return (void *) obj;
}

static void delete_state(void *param, void *state)
{
    (void) param;
    intf.free(state);
}


int EXPORT gen_getinfo(GenInfoC *gi)
{
    static const char name[] = "MWC64X";
    gi->name = name;
    gi->init_state = init_state;
    gi->delete_state = delete_state;
    gi->get_u01 = get_u01;
    gi->get_bits32 = get_bits32;
    gi->get_array32 = get_array32;
    return 1;
}
