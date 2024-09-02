/**
 * @brief Just 64-bit LCG.
 * @details
 *
 *       Test                          p-value
 * ----------------------------------------------
 *  8  CollisionOver, t = 8          9.3e-151
 * 10  CollisionOver, t = 20          5.6e-10
 * 13  BirthdaySpacings, t = 4         1.4e-5
 * 15  BirthdaySpacings, t = 7       7.4e-289
 * 16  BirthdaySpacings, t = 8          eps
 * 17  BirthdaySpacings, t = 8          eps
 * ----------------------------------------------
 * All other tests were passed
 */
#include "testu01_mt_cintf.h"

PRNG_CMODULE_PROLOG

typedef struct {
    uint64_t x;
} Lcg64State;



static long unsigned int get_bits32(void *param, void *state)
{
    Lcg64State *obj = (Lcg64State *) state;
    (void) param;
    obj->x = obj->x * 6906969069 /*0xd1342543de82ef95*/ + 1;
    return obj->x >> 32;
}


static double get_u01(void *param, void *state)
{
    return uint32_to_udouble(get_bits32(param, state));
}


static void *init_state()
{
    Lcg64State *obj = (Lcg64State *) intf.malloc(sizeof(Lcg64State));
    obj->x = intf.get_seed64();
    return (void *) obj;
}


static void delete_state(void *param, void *state)
{
    (void) param;
    intf.free(state);
}


int EXPORT gen_getinfo(GenInfoC *gi)
{
    static const char name[] = "Lcg64";
    gi->name = name;
    gi->init_state = init_state;
    gi->delete_state = delete_state;
    gi->get_u01 = get_u01;
    gi->get_bits32 = get_bits32;
    return 1;
}

