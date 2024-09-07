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

/**
 * @brief xoroshiro128** PRNG state.
 */
typedef struct {
    uint64_t s[2];
} PrngState;


static inline uint64_t get_bits64_raw(void *param, void *state)
{
    PrngState *obj = state;
    uint64_t *s = obj->s, s0 = s[0], s1 = s[1];
	uint64_t result = rotl(s0 * 5, 7) * 9;
    (void) param;
	s1 ^= s0;
	s[0] = rotl(s0, 24) ^ s1 ^ (s1 << 16); // a, b
	s[1] = rotl(s1, 37); // c
	return result;
}

static void *init_state()
{
    const uint64_t phi = 0x9E3779B97F4A7C15ull; // Golden ratio
    PrngState *obj = intf.malloc(sizeof(PrngState));
    obj->s[0] = intf.get_seed64();
    obj->s[1] = intf.get_seed64();
    if (obj->s[0] == 0) obj->s[0] = phi;
    if (obj->s[1] == 0) obj->s[1] = phi;
    return (void *) obj;
}

MAKE_UINT64_UPTO32_PRNG("xoroshiro128**", NULL)
