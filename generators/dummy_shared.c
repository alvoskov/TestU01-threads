////////////////////////////////////////////////////////////////////
///// An implementation of "dummy" PRNG that always returns 0. /////
///// It is used only for performance measurements!            /////
////////////////////////////////////////////////////////////////////


#include "testu01_mt_cintf.h"

PRNG_CMODULE_PROLOG


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


static void *dummy_init_state()
{
    return NULL;
}

static void dummy_delete_state(void *param, void *state)
{
    (void) param;
    intf.free(state);
}


EXPORT int gen_getinfo(GenInfoC *gi)
{
    static const char name[] = "Dummy";
    static GenInfoC dummy_info;
    dummy_info.name = name;
    dummy_info.init_state = dummy_init_state;
    dummy_info.delete_state = dummy_delete_state;
    dummy_info.get_u01 = dummy_get_u01;
    dummy_info.get_bits32 = dummy_get_bits32;
    dummy_info.get_bits64 = dummy_get_bits64;
    dummy_info.get_array32 = dummy_get_array32;
    dummy_info.get_array64 = dummy_get_array64;
    dummy_info.run_self_test = NULL;
    *gi = dummy_info;
    return 1;
}
