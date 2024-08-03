/**
 * @file xoroshiro128stst_shared.c
 * @brief xoroshiro128** 1.0 PRNG. Designed by David Blackman and
 * Sebastiano Vigna (vigna@acm.org).
 */

#include "testu01_mt_cintf.h"

PRNG_CMODULE_PROLOG

static inline uint64_t rotl(const uint64_t x, int k) {
	return (x << k) | (x >> (64 - k));
}

typedef struct {
    uint64_t s[2];
} PrngState;


static inline uint64_t get_bits64_inline(PrngState *obj)
{
    uint64_t *s = obj->s, s0 = s[0], s1 = s[1];
	uint64_t result = rotl(s0 * 5, 7) * 9;
	s1 ^= s0;
	s[0] = rotl(s0, 24) ^ s1 ^ (s1 << 16); // a, b
	s[1] = rotl(s1, 37); // c
	return result;
}


uint64_t get_bits64(void *param, void *state)
{
    (void) param;
    return get_bits64_inline((PrngState *) state);
}


void get_array64(void *param, void *state, uint64_t *out, size_t len)
{    
    (void) param;
    PrngState *obj = (PrngState *) state;
    for (size_t i = 0; i < len; i++) {
        out[i] = get_bits64_inline(obj);
    }
}

static long unsigned int get_bits32(void *param, void *state)
{
    return get_bits64(param, state) >> 32;
}

static double get_u01(void *param, void *state)
{
    return uint64_to_udouble(get_bits64(param, state));
}

static void *init_state()
{
    const uint64_t phi = 0x9E3779B97F4A7C15ull; // Golden ratio
    PrngState *obj = (PrngState *) intf.malloc(sizeof(PrngState));
    obj->s[0] = intf.get_seed64();
    obj->s[1] = intf.get_seed64();
    if (obj->s[0] == 0) obj->s[0] = phi;
    if (obj->s[1] == 0) obj->s[1] = phi;
    return (void *) obj;
}

static void delete_state(void *param, void *state)
{
    (void) param;
    intf.free(state);
}


int EXPORT gen_getinfo(GenInfoC *gi)
{
    static const char name[] = "xoroshiro128**";
    gi->name = name;
    gi->init_state = init_state;
    gi->delete_state = delete_state;
    gi->get_u01 = get_u01;
    gi->get_bits32 = get_bits32;
    gi->get_bits64 = get_bits64;
    gi->get_array64 = get_array64;
    return 1;
}

