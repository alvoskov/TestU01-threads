/**
 * @file mwc128x_shared.c
 * @brief MWC128X - 128-bit PRNG based on MWC method.
 * @details Multiply-with-carry PRNG with a simple output function x ^ c.
 * Has period about 2^127. Passes SmallCrush, Crush and BigCrush tests.
 * It is a slight modification of MWC128 suggested by Sebastiano Vigna.
 *
 * Without XORing this PRNG may fail specially developed 3D birthday
 * spacing tests (noticed by S.Vigna). The output XOR is different from
 * used by Vigna and similar to used in MWC64X suggested by David B. Thomas.
 *
 * MWC_A1 multiplier was found by S. Vigna.
 *
 * References:
 * 1. David B. Thomas. The MWC64X Random Number Generator.
 *    https://cas.ee.ic.ac.uk/people/dt10/research/rngs-gpu-mwc64x.html
 * 2. G. Marsaglia "Multiply-With-Carry (MWC) generators" (from DIEHARD
 *    CD-ROM) https://www.grc.com/otg/Marsaglia_MWC_Generators.pdf
 * 3. Sebastiano Vigna. MWC128. https://prng.di.unimi.it/MWC128.c
 *
 * @copyright (c) 2024 Alexey L. Voskov, Lomonosov Moscow State University.
 * alvoskov@gmail.com
 *
 * All rights reserved.
 *
 * This software is provided under the Apache 2 License.
 */
#include "testu01th/cinterface.h"

PRNG_CMODULE_PROLOG

/**
 * @brief MWC128X state. Cannot be initialized to (0, 0) or to
 * (2^64 - 1, 2^64 - 1). Default initialization is (seed, 1) as suggested
 * by S. Vigna.
 */
typedef struct {
    uint64_t x;
    uint64_t c;
    Interleaved32Buffer i32buf;
} MWC128State;

/**
 * @brief MWC128X PRNG implementation.
 * @details A slight modification of PRNG by S.Vigna: output function
 * is x ^ c instead of (x << 32) ^ x. The multiplier is suggested
 * by Vigna.
 */
static inline uint64_t get_bits64_raw(void *param, void *state)
{
    static const uint64_t MWC_A1 = 0xffebb71d94fcdaf9;
    MWC128State *obj = state;
    const __uint128_t t = MWC_A1 * (__uint128_t) obj->x + obj->c;
    (void) param;
    obj->x = t;
    obj->c = t >> 64;
    return obj->x ^ obj->c;
}


static void *init_state(void)
{
    MWC128State *obj = intf.malloc(sizeof(MWC128State));
    obj->x = intf.get_seed64();
    obj->c = 1;
    Interleaved32Buffer_init(&obj->i32buf);
    return (void *) obj;
}


int run_self_test(void)
{
    MWC128State obj = {.x = 12345, .c = 67890};
    uint64_t u, u_ref = 0xDE4919065DBF6449;
    for (size_t i = 0; i < 1000000; i++) {
        u = get_bits64_raw(NULL, &obj);
    }
    intf.printf("Result: %llX; reference value: %llX\n", u, u_ref);
    return u == u_ref;
}


MAKE_UINT64_INTERLEAVED32_PRNG("MWC128X", MWC128State, run_self_test)
