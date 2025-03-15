/**
 * @file xoshiro128p.c
 * @brief xoshiro128+ pseurorandom number generator.
 * @details The implementation is based on public domain code by D.Blackman
 * and S.Vigna (vigna@acm.org). Its lowest bit has low linear complexity.
 *
 * References:
 * 1. D. Blackman, S. Vigna. Scrambled Linear Pseudorandom Number Generators
 *    // ACM Transactions on Mathematical Software (TOMS). 2021. V. 47. N 4.
 *    Article No.: 36, P.1-32. https://doi.org/10.1145/3460772
 * 2. D. Lemire, M. E. O'Neill. Xorshift1024*, xorshift1024+, xorshift128+
 *    and xoroshiro128+ fail statistical tests for linearity // Journal of
 *    Computational and Applied Mathematics. 2019. V.350. P.139-142.
 *    https://doi.org/10.1016/j.cam.2018.10.019.
 * 3. xoshiro / xoroshiro generators and the PRNG shootout
 *    https://prng.di.unimi.it/
 * @copyright The xoshiro128+ algorithm and its public domain implementation
 * are suggested by D. Blackman and S. Vigna.
 *
 * Adaptation for SmokeRand:
 *
 * (c) 2024-2025 Alexey L. Voskov, Lomonosov Moscow State University.
 * alvoskov@gmail.com
 *
 * This software is licensed under the MIT license.
 */
#include "testu01th/cinterface.h"

PRNG_CMODULE_PROLOG


typedef struct {
    uint32_t s[4];
} Xoshiro128PState;


static uint32_t get_bits32_raw(void *param, void *state)
{
    Xoshiro128PState *obj = state;
	const uint32_t result = obj->s[0] + obj->s[3];
    const uint32_t t = obj->s[1] << 9;
    obj->s[2] ^= obj->s[0];
    obj->s[3] ^= obj->s[1];
    obj->s[1] ^= obj->s[2];
    obj->s[0] ^= obj->s[3];
    obj->s[2] ^= t;
    obj->s[3] = rotl32(obj->s[3], 11);
    (void) param;
    return result;
}

static void *init_state()
{
    Xoshiro128PState *obj = intf.malloc(sizeof(Xoshiro128PState));
    uint64_t seed0, seed1;
    do {
        seed0 = intf.get_seed64();
        seed1 = intf.get_seed64();
    } while (seed0 == 0 && seed1 == 0);
    obj->s[0] = (uint32_t) seed0;
    obj->s[1] = (uint32_t) (seed0 >> 32);
    obj->s[2] = (uint32_t) seed1;
    obj->s[3] = (uint32_t) (seed0 >> 32);
    return obj;
}

int run_self_test()
{
    Xoshiro128PState obj = {
        {0x12345678, 0x87654321, 0xDEADBEEF, 0xF00FC7C8}};
    static const uint32_t u_ref = 0x57CF9D80;
    uint32_t u;
    for (int i = 0; i < 10000; i++) {
        u = get_bits32_raw(&obj, NULL);
    }
    intf.printf("Output: 0x%lX; reference value: 0x%lX\n",
        (unsigned long) u, (unsigned long) u_ref);
    return u == u_ref;
}

MAKE_UINT32_PRNG("xoshiro128+", run_self_test);
