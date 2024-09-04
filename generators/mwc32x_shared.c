/**
 * @file mwc32x_shared.c
 * @details
 */

#include "testu01_mt_cintf.h"

PRNG_CMODULE_PROLOG

typedef struct {
    uint32_t data;
} MWC32XState;

static inline uint32_t get_bits16(void *param, void *state)
{
    const uint16_t A0 = 65184;
    MWC32XState *obj = state;
    (void) param;
    uint16_t c = obj->data >> 16;
    uint16_t x = obj->data & 0xFFFF;
    obj->data = A0 * x + c;
    return x ^ c;
}

static inline uint32_t get_bits32_raw(void *param, void *state)
{
    uint32_t hi = get_bits16(param, state);
    uint32_t lo = get_bits16(param, state);    
    return (hi << 16) | lo;
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
    MWC32XState *obj = intf.malloc(sizeof(MWC32XState));
    do {
        obj->data = intf.get_seed64();
    } while (obj->data == 0 || obj->data == 0xFFFFFFFF);
    return (void *) obj;
}

static void delete_state(void *param, void *state)
{
    (void) param;
    intf.free(state);
}


int EXPORT gen_getinfo(GenInfoC *gi)
{
    static const char name[] = "MWC32X";
    gi->name = name;
    gi->init_state = init_state;
    gi->delete_state = delete_state;
    gi->get_u01 = get_u01;
    gi->get_bits32 = get_bits32;
    gi->get_array32 = get_array32;
    return 1;
}
