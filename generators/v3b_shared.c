/**
 * @file v3b.c
 * @brief Velox 3b (v3b) nonlinear PNG by Elias Yarrkov.
 * @details Resembles 128-bit block ARX cipher in the CBC mode, passes `express`,
 * `brief`, `default` and `full` batteries. Mustn't be used in the CTR mode:
 * it won't pass even the `express` battery.
 *
 * Text from the original manual:
 *
 * Guaranteed period for 128-bit blocks is 2^128.[1]
 * Seeds can't begin to overlap before generating at least 2^128 blocks.[2]
 * The expected average period for 128-bit blocks is 2^255.[3]
 *
 * [1] Proof that the minimum period for 128-bit output blocks is 2^128:
 *
 * The structure of this algorithm is
 *   output(x = f(x + (ctr = ctr + 1)));
 *
 * Let's say the output blocks go into an array out[].
 * If there is a cycle, then at points `a` and `b` in time,
 *     out[a] == out[b].
 * Now, the next blocks are as follows:
 *     out[a+1] == f(out[a]+a+1)
 * and
 *     out[b+1] == f(out[b]+b+1).
 * As f is a 128-bit bijective function, we get that a == b (mod 2^128). This can
 * only happen when the difference between a and b is a multiple of 2^128.
 *
 * [2] Proof of non-overlapping of streams by different seeds:
 *
 * Any particular fixed number of state update iterations gives a bijection of the
 * initial block. The seed is placed in the initial block. With two different
 * seeds, two streams can thus never generate the same 128-bit block at the same
 * stream position. If they generate the same 128-bit block at different positions,
 * the next block will necesarily be different unless the counters also equal,
 * which can only happen after generating at least 2^128 blocks, as explained in
 * the previous proof.
 *
 * [3] Explanation of expected period 2^255:
 *
 * The possibility for cycling happens every 2^128 blocks. 2^128 rounds of the
 * block mix is expected to be a pseudorandom permutation. The average period for
 * an n-bit (n not small) pseudorandom permutation is about 2^(n-1).
 *
 * This gives expected total average period 2^128 * 2^(128-1) == 2^255.
 *
 *
 * @copyright The v3b algorithm is developed by Elias Yarrkov
 * (http://cipherdev.org/v3b.c), the original implementation is released into
 * the public domain through CC0. No rights reserved.
 * http://creativecommons.org/publicdomain/zero/1.0/
 * 
 * Reimplementation for SmokeRand:
 *
 * (c) 2025 Alexey L. Voskov, Lomonosov Moscow State University.
 * alvoskov@gmail.com
 *
 * This software is licensed under the MIT license.
 */
#include "testu01th/cinterface.h"

PRNG_CMODULE_PROLOG

typedef struct {
    uint32_t v[4];
    uint32_t ctr[4];
    int pos;
} V3bState;


static inline void v3b_mixer(uint32_t *v, int shift1, int shift2)
{
    v[0] = rotl32(v[0] + v[3], shift1);
    v[1] = rotl32(v[1], shift2) + v[2];
    v[2] = v[2] ^ v[0];
    v[3] = v[3] ^ v[1];
}


static inline unsigned long get_bits32_raw(void *param, void *state)
{
    V3bState *obj = state;
    if (obj->pos == 0) {
        v3b_mixer(obj->v, 21, 12);
        v3b_mixer(obj->v, 19, 24);
        v3b_mixer(obj->v, 7,  12);
        v3b_mixer(obj->v, 27, 17);
        for (int i = 0; i < 4; i++) {
            obj->v[i] += obj->ctr[i];
        }
        for (int i = 0; i < 4; i++) {
            if (++obj->ctr[i]) break;
        }
        obj->pos = 4;
    }
    (void) param;
    return obj->v[--obj->pos];
}


static void V3bState_init(V3bState *obj, uint64_t seed)
{
    for (int i = 0; i < 4; i++) {
        obj->v[i] = obj->ctr[i] = i * 0x9e3779b9;
    }
    obj->v[0] = seed;
    obj->pos = 0;
    for (int i = 0; i < 16; i++) {
        (void) get_bits32_raw(NULL, obj);
    }
}

static void *init_state(void)
{
    V3bState *obj = intf.malloc(sizeof(V3bState));
    V3bState_init(obj, intf.get_seed64());
    return obj;
}


static int run_self_test(void)
{
    uint32_t x = 0, x_ref = 0x3EE2E740;
    V3bState obj;
    V3bState_init(&obj, 0);
    for (unsigned int i = 0; i < 1 << 20; i++) {
        x ^= get_bits32_raw(NULL, &obj);
    }
    intf.printf("Test value: %08X -- %s\n",
        x, x == x_ref ? "ok!" : "FAILED!");
    return x == x_ref;
}

MAKE_UINT32_PRNG("v3b", run_self_test)
