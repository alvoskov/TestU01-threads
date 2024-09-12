/**
 * @file philox32_shared.c
 * @brief An implementation of Philox4x32x10 PRNG.
 * @details Philox PRNG is inspired by Threefish cipher but uses 64-bit
 * multiplication instead of cyclic shifts and uses reduced number of rounds.
 * Even 7 rounds is enough to pass BigCrush.
 *
 * References:
 *
 * 1. J. K. Salmon, M. A. Moraes, R. O. Dror and D. E. Shaw, "Parallel random
 *    numbers: As easy as 1, 2, 3," SC '11: Proceedings of 2011 International
 *    Conference for High Performance Computing, Networking, Storage and
 *    Analysis, Seattle, WA, USA, 2011, pp. 1-12.
 *    https://doi.org/10.1145/2063384.2063405.
 * 2. Random123: a Library of Counter-Based Random Number Generators
 *    https://github.com/girving/random123/blob/main/tests/kat_vectors
 *
 * @copyright Philox algorithm is proposed by J. K. Salmon, M. A. Moraes,
 * R. O. Dror and D. E. Shaw.
 *
 * Implementation for TestU01-threads:
 * (c) 2024 Alexey L. Voskov, Lomonosov Moscow State University.
 * alvoskov@gmail.com
 *
 * All rights reserved.
 *
 * This software is provided under the Apache 2 License.
 *
 * In scientific publications which used this software, a reference to it
 * would be appreciated.
 */
#include "testu01th/cinterface.h"

#define Nw 4

PRNG_CMODULE_PROLOG

/////////////////////////////////
///// Philox implementation /////
/////////////////////////////////


typedef struct {
    uint32_t key[Nw]; // Key (+ extra word)
    uint32_t ctr[Nw]; // Counter ("plain text")
    uint32_t out[Nw]; // Output buffer
    size_t pos;
} Philox32State;


static void Philox32State_init(Philox32State *obj, const uint32_t *key)
{
    for (size_t i = 0; i < Nw; i++) {
        obj->key[i] = key[i];
        obj->ctr[i] = 0;
    }
}

static inline void philox_bumpkey(uint32_t *key)
{
    key[0] += 0x9E3779B9; // Golden ratio
    key[1] += 0xBB67AE85; // sqrt(3) - 1
}

static inline void philox_round(uint32_t *out, const uint32_t *key)
{
    uint64_t mul0 = out[0] * 0xD2511F53ull;
    uint64_t mul1 = out[2] * 0xCD9E8D57ull;
    uint32_t hi0 = mul0 >> 32, lo0 = mul0;
    uint32_t hi1 = mul1 >> 32, lo1 = mul1;
    out[0] = hi1 ^ out[1] ^ key[0]; out[1] = lo1;
    out[2] = hi0 ^ out[3] ^ key[1]; out[3] = lo0;
}


EXPORT void Philox32State_block10(Philox32State *obj)
{
    uint32_t key[Nw], out[Nw];
    for (size_t i = 0; i < Nw; i++) {
        key[i] = obj->key[i];
        out[i] = obj->ctr[i];
    }

    {                      philox_round(out, key); } // Round 0
    { philox_bumpkey(key); philox_round(out, key); } // Round 1
    { philox_bumpkey(key); philox_round(out, key); } // Round 2
    { philox_bumpkey(key); philox_round(out, key); } // Round 3
    { philox_bumpkey(key); philox_round(out, key); } // Round 4
    { philox_bumpkey(key); philox_round(out, key); } // Round 5
    { philox_bumpkey(key); philox_round(out, key); } // Round 6
    { philox_bumpkey(key); philox_round(out, key); } // Round 7
    { philox_bumpkey(key); philox_round(out, key); } // Round 8
    { philox_bumpkey(key); philox_round(out, key); } // Round 9

    for (size_t i = 0; i < Nw; i++) {
        obj->out[i] = out[i];
    }
}

static inline void Philox32State_inc_counter(Philox32State *obj)
{
    if (++obj->ctr[0] == 0) obj->ctr[1]++;
}

///////////////////////////////
///// Internal self-tests /////
///////////////////////////////

static int self_test_compare(const uint32_t *out, const uint32_t *ref)
{
    intf.printf("OUT: ");
    int is_ok = 1;
    for (size_t i = 0; i < Nw; i++) {
        intf.printf("%X ", out[i]);
        if (out[i] != ref[i])
            is_ok = 0;
    }
    intf.printf("\n");
    intf.printf("REF: ");
    for (size_t i = 0; i < Nw; i++) {
        intf.printf("%X ", ref[i]);
    }
    intf.printf("\n");
    return is_ok;
}


/**
 * @brief An internal self-test. Test vectors are taken
 * from Random123 library.
 */
static int run_self_test(void)
{
    Philox32State obj;
    const uint32_t k0_m1[4] = {-1, -1, -1, -1};
    const uint32_t ref_m1[4] = {0x408f276d, 0x41c83b0e, 0xa20bc7c6, 0x6d5451fd};
    const uint32_t k0_pi[4]  = {0xa4093822, 0x299f31d0, 0x082efa98, 0xec4e6c89};
    const uint32_t ref_pi[4] = {0xd16cfe09, 0x94fdcceb, 0x5001e420, 0x24126ea1};

    Philox32State_init(&obj, k0_m1);
    obj.ctr[0] = -1; obj.ctr[1] = -1;
    obj.ctr[2] = -1; obj.ctr[3] = -1;

    intf.printf("Philox4x64x10 ('-1' example)\n");
    Philox32State_block10(&obj);
    if (!self_test_compare(obj.out, ref_m1)) {
        return 0;
    }

    Philox32State_init(&obj, k0_pi);
    obj.ctr[0] = 0x243f6a88; obj.ctr[1] = 0x85a308d3;
    obj.ctr[2] = 0x13198a2e; obj.ctr[3] = 0x03707344;
    intf.printf("Philox4x64x10 ('pi' example)\n");
    Philox32State_block10(&obj);
    if (!self_test_compare(obj.out, ref_pi)) {
        return 0;
    }
    return 1;
}


/////////////////////////////////////
///// Module external interface /////
/////////////////////////////////////

static inline uint32_t get_bits32_raw(void *param, void *state)
{
    Philox32State *obj = state;
    (void) param;
    if (obj->pos >= Nw) {
        Philox32State_inc_counter(obj);
        Philox32State_block10(obj);
        obj->pos = 0;
    }
    return obj->out[obj->pos++];
}

static void *init_state(void)
{
    uint32_t k[Nw];
    Philox32State *obj = intf.malloc(sizeof(Philox32State));
    for (size_t i = 0; i < Nw; i += 2) {
        uint64_t seed = intf.get_seed64();
        k[i] = seed;
        k[i + 1] = seed >> 32;
    }
    Philox32State_init(obj, k);
    return (void *) obj;
}


MAKE_UINT32_PRNG("Philox4x32x10", run_self_test)
