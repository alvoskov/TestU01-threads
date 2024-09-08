#include "splitmix_gen.h"
#include <stdlib.h>

static inline uint64_t get_bits64_raw(void *param, void *state)
{
    SplitMixState *obj = state;
    (void) param;
    const uint64_t gamma = 0x9E3779B97F4A7C15;
    uint64_t z = (obj->x += gamma);
    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9;
    z = (z ^ (z >> 27)) * 0x94D049BB133111EB;
    return z ^ (z >> 31);
}


static uint64_t get_bits64(void *param, void *state)
{
    return get_bits64_raw(param, state);
}


static uint64_t get_sum64(void *param, void *state, size_t len)
{
    size_t sum = 0;
    for (size_t i = 0; i < len; i++) {
        sum += get_bits64_raw(param, state);
    }
    return sum;
}


static void get_array64(void *param, void *state, uint64_t *out, size_t len)
{
    for (size_t i = 0; i < len; i++) {
        out[i] = get_bits64_raw(param, state);
    }
}


static double get_u01(void *param, void *state)
{
    double u = uint64_to_udouble(get_bits64(param, state));
    return u;
}

static long unsigned int get_bits32(void *param, void *state)
{
    uint64_t x = get_bits64(param, state);
    return x >> 32;
}

static void *init_state()
{
    SplitMixState *obj = malloc(sizeof(SplitMixState));
    obj->x = 0;
    return (void *) obj;
}

static void delete_state(void *param, void *state)
{
    (void) param;
    free(state);
}


int splitmix_get_geninfo(GenInfoC *gi)
{
    static const char name[] = "SplitMix64";
    gi->name = name;
    gi->init_state = init_state;
    gi->delete_state = delete_state;
    gi->get_u01 = get_u01;
    gi->get_bits32 = get_bits32;
    gi->get_bits64 = get_bits64;
    gi->get_array64 = get_array64;
    gi->get_sum64 = get_sum64;
    return 1;
}
