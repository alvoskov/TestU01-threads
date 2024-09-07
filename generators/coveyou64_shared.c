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

static inline uint32_t get_bits32_raw(void *param, void *state)
{
    Coveyou64State *obj = (Coveyou64State *) state;
    (void) param;
    obj->x = (obj->x + 1) * obj->x;
    return obj->x >> 32;
}

static void *init_state()
{
    Coveyou64State *obj = intf.malloc(sizeof(Coveyou64State));
    obj->x = intf.get_seed64();
    return (void *) obj;
}

MAKE_UINT32_PRNG("Coveyou64", NULL)
