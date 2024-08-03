/**
 * @brief An implementation of Philox4x64x10 PRNG.
 * @details Philox PRNG is inspired by Threefish cipher but uses 128-bit
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
 * @copyright (c) 2024 Alexey L. Voskov, Lomonosov Moscow State University.
 * alvoskov@gmail.com
 *
 * All rights reserved.
 *
 * This software is provided under the Apache 2 License.
 *
 * In scientific publications which used this software, a reference to it
 * would be appreciated.
 */

#include "testu01_mt_cintf.h"

#define Nw 4

PRNG_CMODULE_PROLOG

/////////////////////////////////
///// Philox implementation /////
/////////////////////////////////


typedef struct {
    uint64_t key[Nw]; // Key (+ extra word)
    uint64_t ctr[Nw]; // Counter ("plain text")
    uint64_t out[Nw]; // Output buffer
    size_t pos;

    union {
        uint32_t u32[2];
        uint64_t u64;
    } bits32_buf;
    size_t bits32_pos;
} PhiloxState;


static void PhiloxState_init(PhiloxState *obj, const uint64_t *key)
{
    for (size_t i = 0; i < Nw; i++) {
        obj->key[i] = key[i];
        obj->ctr[i] = 0;
    }
}

static inline void philox_bumpkey(uint64_t *key)
{
    key[0] += 0x9E3779B97F4A7C15ull; // Golden ratio
    key[1] += 0xBB67AE8584CAA73Bull; // sqrt(3) - 1
}

static inline void philox_round(uint64_t *out, const uint64_t *key)
{
    static const uint64_t PHILOX_M4x64_0 = 0xD2E7470EE14C6C93ull;
    static const uint64_t PHILOX_M4x64_1 = 0xCA5A826395121157ull;
    __int128 mul0 = ((__int128) out[0]) * PHILOX_M4x64_0;
    __int128 mul1 = ((__int128) out[2]) * PHILOX_M4x64_1;
    uint64_t hi0 = mul0 >> 64, lo0 = mul0;
    uint64_t hi1 = mul1 >> 64, lo1 = mul1;
    out[0] = hi1 ^ out[1] ^ key[0]; out[1] = lo1;
    out[2] = hi0 ^ out[3] ^ key[1]; out[3] = lo0;
}


EXPORT void PhiloxState_block10(PhiloxState *obj)
{
    uint64_t key[Nw], out[Nw];
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

static inline void PhiloxState_inc_counter(PhiloxState *obj)
{
    if (++obj->ctr[0] == 0) obj->ctr[1]++;
}

///////////////////////////////
///// Internal self-tests /////
///////////////////////////////

static int self_test_compare(const uint64_t *out, const uint64_t *ref)
{
    intf.printf("OUT: ");
    int is_ok = 1;
    for (size_t i = 0; i < Nw; i++) {
        intf.printf("%llX ", out[i]);
        if (out[i] != ref[i])
            is_ok = 0;
    }
    intf.printf("\n");
    intf.printf("REF: ");
    for (size_t i = 0; i < Nw; i++) {
        intf.printf("%llX ", ref[i]);
    }
    intf.printf("\n");
    return is_ok;
}

/**
 * @brief An internal self-test. Test vectors are taken
 * from Random123 library.
 */
static int run_self_test()
{
    PhiloxState obj;
    static const uint64_t k0_m1[4] = {-1, -1, -1, -1};
    static const uint64_t ref_m1[4] = {0x87b092c3013fe90bull,
        0x438c3c67be8d0224ull, 0x9cc7d7c69cd777b6ull, 0xa09caebf594f0ba0ull};

    static const uint64_t k0_pi[4] = {0x452821e638d01377ull,
        0xbe5466cf34e90c6cull, 0xbe5466cf34e90c6cull, 0xc0ac29b7c97c50ddull};
    static const uint64_t ref_pi[4] = {0xa528f45403e61d95ull,
        0x38c72dbd566e9788ull, 0xa5a1610e72fd18b5ull, 0x57bd43b5e52b7fe6ull};

    PhiloxState_init(&obj, k0_m1);
    obj.ctr[0] = -1; obj.ctr[1] = -1;
    obj.ctr[2] = -1; obj.ctr[3] = -1;

    intf.printf("Philox4x64x10 ('-1' example)\n");
    PhiloxState_block10(&obj);
    if (!self_test_compare(obj.out, ref_m1)) {
        return 0;
    }

    PhiloxState_init(&obj, k0_pi);
    obj.ctr[0] = 0x243f6a8885a308d3; obj.ctr[1] = 0x13198a2e03707344;
    obj.ctr[2] = 0xa4093822299f31d0; obj.ctr[3] = 0x082efa98ec4e6c89;

    intf.printf("Philox4x64x10 ('pi' example)\n");
    PhiloxState_block10(&obj);
    if (!self_test_compare(obj.out, ref_pi)) {
        return 0;
    }
    return 1;
}


/////////////////////////////////////
///// Module external interface /////
/////////////////////////////////////



static uint64_t get_bits64(void *param, void *state)
{
    (void) param;
    PhiloxState *obj = (PhiloxState *) state;
    if (obj->pos >= Nw) {
        PhiloxState_inc_counter(obj);
        PhiloxState_block10(obj);
        obj->pos = 0;
    }
    return obj->out[obj->pos++];
}


static void get_array64(void *param, void *state, uint64_t *out, size_t len)
{
    (void) param;
    PhiloxState *obj = (PhiloxState *) state;
    size_t pos = 0;
    // Returns blocks of Nw uint64s.
    for (size_t i = 0; i < len / Nw; i++) {
        PhiloxState_inc_counter(obj);
        PhiloxState_block10(obj);
        for (size_t j = 0; j < Nw; j++) {
            out[pos++] = obj->out[j];
        }
    }
    // Returns the tail with less than 16 uint32s (if needed)
    size_t tail_len = len % Nw;
    if (tail_len != 0) {
        PhiloxState_inc_counter(obj);
        PhiloxState_block10(obj);
        for (size_t j = 0; j < tail_len; j++) {
            out[pos++] = obj->out[j];
        }
    }
    // Invalidate cache for future get_bits32
    obj->pos = Nw;
}


unsigned long EXPORT get_bits32(void *param, void *state)
{

    return get_bits64(param, state) >> 32;
}


static double get_u01(void *param, void *state)
{
    double u = uint64_to_udouble(get_bits64(param, state));
    return u;
}


static void *init_state()
{
    uint64_t k[Nw];
    PhiloxState *obj = (PhiloxState *) intf.malloc(sizeof(PhiloxState));
    for (size_t i = 0; i < Nw; i++) {
        k[i] = intf.get_seed64();
    }
    PhiloxState_init(obj, k);
    return (void *) obj;
}


static void delete_state(void *param, void *state)
{
    (void) param;
    intf.free(state);
}


int EXPORT gen_getinfo(GenInfoC *gi)
{
    static const char name[] = "Philox4x64x10";
    gi->name = name;
    gi->init_state = init_state;
    gi->delete_state = delete_state;
    gi->get_u01 = get_u01;
    gi->get_bits32 = get_bits32;
    gi->get_bits64 = get_bits64;
    gi->get_array64 = get_array64;
    gi->run_self_test = run_self_test;
    return 1;
}
