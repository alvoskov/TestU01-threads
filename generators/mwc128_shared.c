#include "testu01_mt_cintf.h"

PRNG_CMODULE_PROLOG

// Written in 2023 by Sebastiano Vigna (vigna@acm.org)
// https://prng.di.unimi.it/MWC128.c
// https://www.grc.com/otg/Marsaglia_MWC_Generators.pdf

#define MWC_A1 0xffebb71d94fcdaf9

/* The state must be initialized so that 0 < c < MWC_A1 - 1.
   For simplicity, we suggest to set c = 1 and x to a 64-bit seed. */
typedef struct {
    uint64_t x;
    uint64_t c;
} MWC128State;

static inline uint64_t get_bits64_raw(void *param, void *state)
{
    MWC128State *obj = state;
    const uint64_t result = /*obj->x ^ (obj->x << 32);*/ obj->x; // Or, result = x ^ (x << 32) (see above)
    const __uint128_t t = MWC_A1 * (__uint128_t) obj->x + obj->c;
    (void) param;
    obj->x = t;
    obj->c = t >> 64;
    return result;
}

static uint64_t get_bits64(void *param, void *state)
{
    return get_bits64_raw(param, state);
}


static long unsigned int get_bits32(void *param, void *state)
{
    return get_bits64_raw(param, state);
}


static double get_u01(void *param, void *state)
{
    return uint64_to_udouble(get_bits64_raw(param, state));
}


static void *init_state()
{
    MWC128State *obj = (MWC128State *) intf.malloc(sizeof(MWC128State));
    uint64_t seed = intf.get_seed64();
    obj->x = seed >> 1;
    obj->c = seed & 0x1;
    return (void *) obj;
}

static void get_array64(void *param, void *state, uint64_t *out, size_t len)
{
    for (size_t i = 0; i < len; i++) {
        out[i] = get_bits64_raw(param, state);
    }
}



static void delete_state(void *param, void *state)
{
    (void) param;
    intf.free(state);
}


int EXPORT gen_getinfo(GenInfoC *gi)
{
    static const char name[] = "MWC128";
    gi->name = name;
    gi->init_state = init_state;
    gi->delete_state = delete_state;
    gi->get_u01 = get_u01;
    gi->get_bits32 = get_bits32;
    gi->get_bits64 = get_bits64;
    gi->get_array64 = get_array64;
    return 1;
}
