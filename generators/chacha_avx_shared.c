/**
 * @file chacha_avx_shared.c
 * @brief ChaCha12 pseudorandom number generator (AVX version). 
 * @details SIMD version of ChaCha12 generator that uses 256-bit YMM CPU
 * registers. See chacha_shared for the detailed comments about the generator.
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
#include "chacha_avx_shared.h"

static CallerAPI intf;

/////////////////////////////////////////////////
///// Entry point for -nostdlib compilation /////
/////////////////////////////////////////////////
SHARED_ENTRYPOINT_CODE

#include <emmintrin.h>
#include <tmmintrin.h>
#include <xopintrin.h>

static inline __m256i mm256_roti_epi32_def(__m256i in, int r)
{
    return _mm256_or_si256(_mm256_slli_epi32(in, r), _mm256_srli_epi32(in, 32 - r));
}

static inline __m256i mm256_rot16_epi32_def(__m256i in)
{
    return _mm256_shuffle_epi8(in,
        _mm256_set_epi8(
            29,28,31,30,  25,24,27,26, 21,20,23,22, 17,16,19,18,
            13,12,15,14,  9,8,11,10,   5,4,7,6,     1,0,3,2));
}

static inline __m256i mm256_rot8_epi32_def(__m256i in)
{
    return _mm256_shuffle_epi8(in,
        _mm256_set_epi8(
            30,29,28,31,  26,25,24,27, 22,21,20,23, 18,17,16,19,
            14,13,12,15,  10,9,8,11,   6,5,4,7,     2,1,0,3));
}


/**
 * @brief Vertical qround (hardware vectorization for x86-64)
 */
static inline void mm_qround_vert(__m256i *a, __m256i *b, __m256i *c, __m256i *d)
{
    *a = _mm256_add_epi32(*a, *b);
    *d = _mm256_xor_si256(*d, *a);
    *d = mm256_rot16_epi32_def(*d);

    *c = _mm256_add_epi32(*c, *d);
    *b = _mm256_xor_si256(*b, *c);
    *b = mm256_roti_epi32_def(*b, 12);

    *a = _mm256_add_epi32(*a, *b);
    *d = _mm256_xor_si256(*d, *a);
    *d = mm256_rot8_epi32_def(*d);

    *c = _mm256_add_epi32(*c, *d);
    *b = _mm256_xor_si256(*b, *c);
    *b = mm256_roti_epi32_def(*b, 7);
}

/**
 * @brief Increase the value of 128-bit PRNG counter.
 */
static inline void ChaCha_inc_counter(ChaChaState *obj)
{
    uint64_t *cnt1 = (uint64_t *) &obj->x[24];
    uint64_t *cnt2 = (uint64_t *) &obj->x[28];
    if (++cnt1[0] == 0) ++cnt1[1];
    if (++cnt1[0] == 0) ++cnt1[1];
    if (++cnt2[0] == 0) ++cnt2[1];
    if (++cnt2[0] == 0) ++cnt2[1];
}



/**
 * @brief Implementation of ChaCha rounds for a 512-bit block.
 * The function is exported for debugging purposes.
 * @details The scheme of rounds are:
 *
 * | x . . . |    | . x . . |    | . . x . |    | . . . x |
 * | x . . . | => | . x . . | => | . . x . | => | . . . x |
 * | x . . . |    | . x . . |    | . . x . |    | . . . x |
 * | x . . . |    | . x . . |    | . . x . |    | . . . x |
 * 
 * | x . . . |    | . x . . |    | . . x . |    | . . . x |
 * | . x . . | => | . . x . | => | . . . x | => | x . . . |
 * | . . x . |    | . . . x |    | x . . . |    | . x . . |
 * | . . . x |    | x . . . |    | . x . . |    | . . x . |
 */
void EXPORT ChaCha_block(ChaChaState *obj)
{
    __m256i a = _mm256_loadu_si256((__m256i *) obj->x);
    __m256i b = _mm256_loadu_si256((__m256i *) (obj->x + 8));
    __m256i c = _mm256_loadu_si256((__m256i *) (obj->x + 16));
    __m256i d = _mm256_loadu_si256((__m256i *) (obj->x + 24));
    __m256i ax = a, bx = b, cx = c, dx = d;
    for (size_t k = 0; k < obj->ncycles; k++) {
        /* Vertical qround */
        mm_qround_vert(&a, &b, &c, &d);
        /* Diagonal qround; the original vector is [3 2 1 0] */
        b = _mm256_shuffle_epi32(b, 0x39); // [0 3 2 1] -> 3 (or <- 1)
        c = _mm256_shuffle_epi32(c, 0x4E); // [1 0 3 2] -> 2 (or <- 2)
        d = _mm256_shuffle_epi32(d, 0x93); // [2 1 0 3] -> 1 (or <- 3)
        mm_qround_vert(&a, &b, &c, &d);
        b = _mm256_shuffle_epi32(b, 0x93);
        c = _mm256_shuffle_epi32(c, 0x4E);
        d = _mm256_shuffle_epi32(d, 0x39);
    }
    a = _mm256_add_epi32(a, ax);
    b = _mm256_add_epi32(b, bx);
    c = _mm256_add_epi32(c, cx);
    d = _mm256_add_epi32(d, dx);

    _mm256_storeu_si256((__m256i *) obj->out, a);
    _mm256_storeu_si256((__m256i *) (obj->out + 8), b);
    _mm256_storeu_si256((__m256i *) (obj->out + 16), c);
    _mm256_storeu_si256((__m256i *) (obj->out + 24), d);
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
    memcpy(obj->x + 4, obj->x, 4 * sizeof(uint32_t));
    /* Rows 1-2: seed (key) */
    /* | 8   9 10 11 | 12 13 14 15 | */
    /* | 16 17 18 19 | 20 21 22 23 | */
    memset(obj->x + 8, 0xFF, 24 * sizeof(uint32_t));
    memcpy(obj->x + 8,  seed,     4 * sizeof(uint32_t));
    memcpy(obj->x + 12, seed,     4 * sizeof(uint32_t));
    memcpy(obj->x + 16, seed + 4, 4 * sizeof(uint32_t));
    memcpy(obj->x + 20, seed + 4, 4 * sizeof(uint32_t));
    /* Row 3: counter and nonce */
    memset(obj->x + 24, 0, 8 * sizeof(uint32_t));
    {
        uint64_t *cnt = (uint64_t *) &obj->x[28];
        cnt[0] = 1;
    }
    ChaCha_inc_counter(obj);
    /* Number of rounds => Number of cycles */
    obj->ncycles = nrounds / 2;
    /* Output state */
    memset(obj->out, 0, 32 * sizeof(uint32_t));
    /* Output counter */
    obj->pos = 32;
}

static long unsigned int get_bits32(void *param, void *state)
{
    (void) param;
    ChaChaState *obj = (ChaChaState *) state;
    if (obj->pos >= 32) {
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
