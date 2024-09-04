/**
 * @file coveyou64_shared.c
 * @brief Coveyou64 PRNG.
 * @details Passes SmallCrush but fails the next two tests
 * from Crush:
 * - 17  BirthdaySpacings, t = 8
 * - 26  SimpPoker, d = 64
 */

#include "testu01_mt_cintf.h"

PRNG_CMODULE_PROLOG

typedef struct {
    uint64_t x;
} Coveyou64State;



static inline uint32_t get_bits32_raw(Coveyou64State *obj)
{
    obj->x = (obj->x + 1) * obj->x;
    return obj->x >> 32;
}



static long unsigned int get_bits32(void *param, void *state)
{
    Coveyou64State *obj = (Coveyou64State *) state;
    (void) param;
    return get_bits32_raw(obj);
}


static void get_array32(void *param, void *state, uint32_t *out, size_t len)
{
    Coveyou64State *obj = (Coveyou64State *) state;
    (void) param;
    for (size_t i = 0; i < len; i++) {
        out[i] = get_bits32_raw(obj);
    }
}


static double get_u01(void *param, void *state)
{
    static const double INV32 = 2.3283064365386963E-10;
    return get_bits32(param, state) * INV32;
}

static void *init_state()
{
    Coveyou64State *obj = (Coveyou64State *) intf.malloc(sizeof(Coveyou64State));
    obj->x = intf.get_seed64();
    return (void *) obj;
}

static void delete_state(void *param, void *state)
{
    (void) param;
    intf.free(state);
}


EXPORT int gen_getinfo(GenInfoC *gi)
{
    static const char name[] = "Coveyou64";
    gi->name = name;
    gi->init_state = init_state;
    gi->delete_state = delete_state;
    gi->get_u01 = get_u01;
    gi->get_bits32 = get_bits32;
    gi->get_array32 = get_array32;
    return 1;
}

