/**
 * @brief KISS93 pseudorandom number generator. It passes SmallCrush
 * but fails the LinearComp (r = 29) test in the Crush battery.
 */
#include "testu01_mt_cintf.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    uint32_t S1;
    uint32_t S2;
    uint32_t S3;
} KISS93State;

static long unsigned int get_bits32(void *param, void *state)
{
    KISS93State *obj = (KISS93State *) state;
    (void) param;
    obj->S1 = 69069 * obj->S1 + 23606797;
    uint32_t b = obj->S2 ^ (obj->S2 << 17);
    obj->S2 = (b >> 15) ^ b;
    b = ((obj->S3 << 18) ^ obj->S3) & 0x7fffffffU;
    obj->S3 = (b >> 13) ^ b;
    return obj->S1 + obj->S2 + obj->S3;
}

static double get_u01(void *param, void *state)
{
    static const double INV32 = 2.3283064365386963E-10;
    return get_bits32(param, state) * INV32;
}

static void *init_state()
{
    KISS93State *obj = (KISS93State *) malloc(sizeof(KISS93State));
    obj->S1 = 12345;
    obj->S2 = 6789;
    obj->S3 = 111213;
    return (void *) obj;
}

static void delete_state(void *param, void *state)
{
    (void) param;
    free(state);
}

int EXPORT gen_initlib(uint64_t seed, void *data)
{
    (void) seed;
    (void) data;
    return 1;
}

int EXPORT gen_closelib()
{
    return 1;
}

int EXPORT gen_getinfo(GenInfoC *gi)
{
    static const char name[] = "KISS93";
    gi->name = name;
    gi->init_state = init_state;
    gi->delete_state = delete_state;
    gi->get_u01 = get_u01;
    gi->get_bits32 = get_bits32;
    return 1;
}
