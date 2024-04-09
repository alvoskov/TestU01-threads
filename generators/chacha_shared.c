/**
 * @file chacha_shared.c
 * @brief ChaCha12 pseudorandom number generator. 
 * @details ChaCha12 PRNG is a modification of cryptographically secure
 * ChaCha20 with reduced number of rounds. Can be considered as CSPRNG
 * itself. ChaCha12 should pass all statistical tests from the TestU01
 * library.
 *
 * References:
 * 1. RFC 7539. ChaCha20 and Poly1305 for IETF Protocols
 *    https://datatracker.ietf.org/doc/html/rfc7539
 *
 * NOTE: This PRNG also must pass the test implemented in chacha_shared.c.
 * This test switches it to the ChaCha20 mode and compares output with
 * RFC 7539 reference values.
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
#include "chacha_shared.h"

static CallerAPI intf;

/////////////////////////////////////////////////
///// Entry point for -nostdlib compilation /////
/////////////////////////////////////////////////
SHARED_ENTRYPOINT_CODE

//////////////////////////////
///// CPU-dependent code /////
//////////////////////////////

#ifdef CHACHA_VECTOR_INTR
#include <emmintrin.h>
#include <tmmintrin.h>
#include <xopintrin.h>

static inline __m128i mm_roti_epi32_def(__m128i in, int r)
{
    return _mm_or_si128(_mm_slli_epi32(in, r), _mm_srli_epi32(in, 32 - r));
}

static inline __m128i mm_rot16_epi32_def(__m128i in)
{
    return _mm_shuffle_epi8(in,
        _mm_set_epi8(13,12,15,14,  9,8,11,10,  5,4,7,6,  1,0,3,2));
}

static inline __m128i mm_rot8_epi32_def(__m128i in)
{
    return _mm_shuffle_epi8(in,
        _mm_set_epi8(14,13,12,15,  10,9,8,11,  6,5,4,7,  2,1,0,3));
}

/**
 * @brief Vertical qround (hardware vectorization for x86-64)
 */
static inline void mm_qround_vert(__m128i *a, __m128i *b, __m128i *c, __m128i *d)
{
    *a = _mm_add_epi32(*a, *b); *d = _mm_xor_si128(*d, *a); *d = mm_rot16_epi32_def(*d);
    *c = _mm_add_epi32(*c, *d); *b = _mm_xor_si128(*b, *c); *b = mm_roti_epi32_def(*b, 12);
    *a = _mm_add_epi32(*a, *b); *d = _mm_xor_si128(*d, *a); *d = mm_rot8_epi32_def(*d);
    *c = _mm_add_epi32(*c, *d); *b = _mm_xor_si128(*b, *c); *b = mm_roti_epi32_def(*b, 7);
}
#else
static inline uint32_t rotl(uint32_t x, uint32_t r)
{
    return (x << r) | (x >> (32 - r));
}

/**
 * @brief Qround (cross-platform scalar implementation)
 */
static inline void qround(uint32_t *x, size_t ai, size_t bi, size_t ci, size_t di)
{
    x[ai] += x[bi]; x[di] ^= x[ai]; x[di] = rotl(x[di], 16);
    x[ci] += x[di]; x[bi] ^= x[ci]; x[bi] = rotl(x[bi], 12);
    x[ai] += x[bi]; x[di] ^= x[ai]; x[di] = rotl(x[di], 8);
    x[ci] += x[di]; x[bi] ^= x[ci]; x[bi] = rotl(x[bi], 7);
}
#endif

/**
 * @brief Increase the value of 128-bit PRNG counter.
 */
static inline void ChaCha_inc_counter(ChaChaState *obj)
{
    uint64_t *cnt = (uint64_t *) &obj->x[12];
    if (++cnt[0] == 0) ++cnt[1];
}



/**
 * @brief Implementation of ChaCha rounds for a 512-bit block.
 * The function is exported for debugging purposes.
 * @details The scheme of rounds are:
 *
 *     | x . . . |    | . x . . |    | . . x . |    | . . . x |
 *     | x . . . | => | . x . . | => | . . x . | => | . . . x |
 *     | x . . . |    | . x . . |    | . . x . |    | . . . x |
 *     | x . . . |    | . x . . |    | . . x . |    | . . . x |
 * 
 *     | x . . . |    | . x . . |    | . . x . |    | . . . x |
 *     | . x . . | => | . . x . | => | . . . x | => | x . . . |
 *     | . . x . |    | . . . x |    | x . . . |    | . x . . |
 *     | . . . x |    | x . . . |    | . x . . |    | . . x . |
 */
void EXPORT ChaCha_block(ChaChaState *obj)
{
#ifdef CHACHA_VECTOR_INTR
    __m128i a = _mm_loadu_si128((__m128i *) obj->x);
    __m128i b = _mm_loadu_si128((__m128i *) (obj->x + 4));
    __m128i c = _mm_loadu_si128((__m128i *) (obj->x + 8));
    __m128i d = _mm_loadu_si128((__m128i *) (obj->x + 12));
    __m128i ax = a, bx = b, cx = c, dx = d;
    for (size_t k = 0; k < obj->ncycles; k++) {
        /* Vertical qround */
        mm_qround_vert(&a, &b, &c, &d);
        /* Diagonal qround; the original vector is [3 2 1 0] */
        b = _mm_shuffle_epi32(b, 0x39); // [0 3 2 1] -> 3 (or <- 1)
        c = _mm_shuffle_epi32(c, 0x4E); // [1 0 3 2] -> 2 (or <- 2)
        d = _mm_shuffle_epi32(d, 0x93); // [2 1 0 3] -> 1 (or <- 3)
        mm_qround_vert(&a, &b, &c, &d);
        b = _mm_shuffle_epi32(b, 0x93);
        c = _mm_shuffle_epi32(c, 0x4E);
        d = _mm_shuffle_epi32(d, 0x39);
    }
    a = _mm_add_epi32(a, ax);
    b = _mm_add_epi32(b, bx);
    c = _mm_add_epi32(c, cx);
    d = _mm_add_epi32(d, dx);

    _mm_storeu_si128((__m128i *) obj->out, a);
    _mm_storeu_si128((__m128i *) (obj->out + 4), b);
    _mm_storeu_si128((__m128i *) (obj->out + 8), c);
    _mm_storeu_si128((__m128i *) (obj->out + 12), d);
#else
    for (size_t k = 0; k < 16; k++) {
        obj->out[k] = obj->x[k];
    }

    for (size_t k = 0; k < obj->ncycles; k++) {
        /* Vertical qrounds */
        qround(obj->out, 0, 4, 8,12);
        qround(obj->out, 1, 5, 9,13);
        qround(obj->out, 2, 6,10,14);
        qround(obj->out, 3, 7,11,15);
        /* Diagonal qrounds */
        qround(obj->out, 0, 5,10,15);
        qround(obj->out, 1, 6,11,12);
        qround(obj->out, 2, 7, 8,13);
        qround(obj->out, 3, 4, 9,14);
    }
    for (size_t i = 0; i < 16; i++) {
        obj->out[i] += obj->x[i];
    }
#endif
}

/**
 * @brief Initialize the state of ChaCha CSPRNG.
 * The function is exported for debugging purposes.
 * @param obj     The state to be initialized.
 * @param nrounds Number of rounds (8, 12, 20).
 * @param seed    Pointer to array of 8 uint32_t values (seeds).
 */
void EXPORT ChaCha_init(ChaChaState *obj, size_t nrounds, const uint32_t *seed)
{
    /* Constants: the upper row of the matrix */
    obj->x[0] = 0x61707865; obj->x[1] = 0x3320646e;
    obj->x[2] = 0x79622d32; obj->x[3] = 0x6b206574;
    /* Rows 1-2: seed (key) */
    for (size_t i = 0; i < 8; i++) {
        obj->x[i + 4] = seed[i];
    }
    /* Row 3: counter and nonce */
    for (size_t i = 12; i <= 15; i++) {
        obj->x[i] = 0;
    }
    ChaCha_inc_counter(obj);
    /* Number of rounds => Number of cycles */
    obj->ncycles = nrounds / 2;
    /* Output state */
    for (size_t i = 0; i < 16; i++) {
        obj->out[i] = 0;
    }
    /* Output counter */
    obj->pos = 16;
}

static long unsigned int get_bits32(void *param, void *state)
{
    (void) param;
    ChaChaState *obj = (ChaChaState *) state;
    if (obj->pos >= 16) {
        ChaCha_inc_counter(obj);
        ChaCha_block(obj);
        obj->pos = 0;
    }
    return obj->out[obj->pos++];
}

static double get_u01(void *param, void *state)
{
    static const double INV32 = 2.3283064365386963E-10;
    return get_bits32(param, state) * INV32;
}


static void *init_state()
{
    ChaChaState *obj = (ChaChaState *) intf.malloc(sizeof(ChaChaState));
    uint32_t seeds[8];
    for (size_t i = 0; i < 4; i++) {
        uint64_t s = intf.get_seed64();
        seeds[2*i] = s & 0xFFFFFFF;
        seeds[2*i + 1] = s >> 32;
    }

    ChaCha_init(obj, 12, seeds);
    return (void *) obj;
}

static void delete_state(void *param, void *state)
{
    (void) param;
    intf.free(state);
}

/////////////////////////////////////////////////
///// Exported functions (module interface) /////
/////////////////////////////////////////////////

int EXPORT gen_initlib(CallerAPI *intf_)
{
    intf = *intf_;
    return 1;
}

int EXPORT gen_closelib()
{
    return 1;
}

int EXPORT gen_getinfo(GenInfoC *gi)
{
    static const char name[] = "ChaCha12";
    gi->name = name;
    gi->init_state = init_state;
    gi->delete_state = delete_state;
    gi->get_u01 = get_u01;
    gi->get_bits32 = get_bits32;
    return 1;
}
