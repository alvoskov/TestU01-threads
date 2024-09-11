#include "testu01th/dummy_module.h"
#include <stdlib.h>

static int dummy_initlib(CallerAPI *intf)
{
    (void) intf;
    return 1;
}


static int dummy_closelib(void)
{
    return 1;
}


static double dummy_get_u01(void *param, void *state)
{
    (void) param;
    (void) state;
    return 0.0;
}


static long unsigned int dummy_get_bits32(void *param, void *state)
{
    (void) param;
    (void) state;
    return 0;
}


static uint64_t dummy_get_bits64(void *param, void *state)
{
    (void) param;
    (void) state;
    return 0;
}


static void dummy_get_array32(void *param, void *state, uint32_t *out, size_t len)
{
    (void) param;
    (void) state;
    for (size_t i = 0; i < len; i++) {
        out[i] = 0;
    }
}


static void dummy_get_array64(void *param, void *state, uint64_t *out, size_t len)
{
    (void) param;
    (void) state;
    for (size_t i = 0; i < len; i++) {
        out[i] = 0;
    }
}


static uint32_t dummy_get_sum32(void *param, void *state, size_t len)
{
    uint32_t data[] = {9338, 34516, 60623, 45281,
        9064,   60090,  62764,  5557,
        44347,  35277,  25712,  20552,
        50645,  61072,  26719,  21307};
    uint32_t sum = 0;
    (void) param; (void) state;
    for (size_t i = 0; i < len; i++) {
        sum += data[i & 0xF];
    }
    return sum;
}

static uint64_t dummy_get_sum64(void *param, void *state, size_t len)
{
    uint64_t data[] = {9338, 34516, 60623, 45281,
        9064,   60090,  62764,  5557,
        44347,  35277,  25712,  20552,
        50645,  61072,  26719,  21307};
    uint64_t sum = 0;
    (void) param; (void) state;
    for (size_t i = 0; i < len; i++) {
        sum += data[i & 0xF];
    }
    return sum;
}

static void *dummy_init_state()
{
    return NULL;
}


static void dummy_delete_state(void *param, void *state)
{
    (void) param;
    (void) state;
}


int dummy_gen_getinfo(GenInfoC *gi)
{
    gi->name = "Dummy";
    gi->init_state = dummy_init_state;
    gi->delete_state = dummy_delete_state;
    gi->get_u01 = dummy_get_u01;
    gi->get_bits32 = dummy_get_bits32;
    gi->get_bits64 = dummy_get_bits64;
    gi->get_array32 = dummy_get_array32;
    gi->get_array64 = dummy_get_array64;
    gi->get_sum32 = dummy_get_sum32;
    gi->get_sum64 = dummy_get_sum64;
    gi->run_self_test = NULL;
    return 1;
}


GenCModule dummy_init_cmodule()
{
    GenCModule dummy_cmodule;
    dummy_cmodule.gen_initlib = dummy_initlib;
    dummy_cmodule.gen_closelib = dummy_closelib;
    dummy_cmodule.gen_getinfo = dummy_gen_getinfo;
    return dummy_cmodule;
}

