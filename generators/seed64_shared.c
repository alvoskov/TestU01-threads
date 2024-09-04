/**
 * @file mwc64x_shared.c
 */
#include "testu01_mt_cintf.h"

PRNG_CMODULE_PROLOG

/**
 * @brief For conversion of 64-bit seeds to 32-bit integers
 */
typedef struct {
    union {
        uint64_t u64val; ///< To be splitted to two 32-bit values.
        uint32_t u32val[2]; ///< 32-bit values.
    } buf32; ///< Internal buffer for 32-bit outputs.
    int pos32; ///< Output position for 32-bit output.
} Seed64State;

static unsigned long get_bits32(void *param, void *state)
{
    Seed64State *obj = state;
    (void) param;
    if (obj->pos32 == 2) {
        obj->buf32.u64val = intf.get_seed64();
        obj->pos32 = 0;
    }
    return obj->buf32.u32val[obj->pos32++];
}

uint64_t get_bits64(void *param, void *state)
{
    (void) param; (void) state;
    return intf.get_seed64();
}

static double get_u01(void *param, void *state)
{
    return uint64_to_udouble(get_bits64(param, state));
}

static void *init_state()
{
    Seed64State *obj = intf.malloc(sizeof(Seed64State));
    obj->pos32 = 2;
    return (void *) obj;
}

static void delete_state(void *param, void *state)
{
    (void) param;
    intf.free(state);
}


int EXPORT gen_getinfo(GenInfoC *gi)
{
    static const char name[] = "seed64";
    gi->name = name;
    gi->init_state = init_state;
    gi->delete_state = delete_state;
    gi->get_u01 = get_u01;
    gi->get_bits32 = get_bits32;
    gi->get_bits64 = get_bits64;
    return 1;
}
