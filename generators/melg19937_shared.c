/**
 * @file melg19937.c
 * @brief An implementation of MELG19937-64: a GFSR that resembles Mersenne
 * Twister.
 * @details Test values were obtained from the reference implementation
 * provided by the authors [2]. This module is a simplified version of it.
 * It doesn't include transition matrix but all global variables were removed.
 *
 * References:
 *
 * 1. Hasare S., Kimoto T. Implementing 64-bit Maximally Equidistributed
 *    F2-Linear Generators with Mersenne Prime Period // ACM Trans. Math.
 *    Softw. 2018. V. 44. N 3. ID 30. 11 pages. https://doi.org/10.1145/3159444
 * 2. https://github.com/sharase/melg-64
 *
 * @copyright
 *
 * (c) 2021 Shin Harase (Ritsumeikan University, harase @ fc.ritsumei.ac.jp),
 * Takamitsu Kimoto
 *
 * (c) 2025 Alexey L. Voskov, Lomonosov Moscow State University.
 * alvoskov@gmail.com
 *
 * This software is licensed under the MIT license.
 */
#include "testu01th/cinterface.h"

PRNG_CMODULE_PROLOG

#define NN 311 // N-1
#define MM 81 // M
#define MATRIX_A 0x5c32e06df730fc42ULL
#define P 33 // W-r
#define W 64
#define MASKU (0xffffffffffffffffULL << (W-P))
#define MASKL (~MASKU)
#define MAT3NEG(t, v) (v ^ (v << ((t))))
#define MAT3POS(t, v) (v ^ (v >> ((t))))
#define LAG1 19 // L
#define SHIFT1 16 // s_3
#define MASK1 0x6aede6fd97b338ecULL // b
#define LAG1over 292 // NN-LAG1

static uint64_t mag01[2]={0ULL, MATRIX_A};

typedef struct MelgState {
    uint64_t lung;
    uint64_t melg[NN];
    int pos;
    uint64_t (*function_p)(struct MelgState *obj);
} MelgState;


static uint64_t case_1(MelgState *obj);
static uint64_t case_2(MelgState *obj);
static uint64_t case_3(MelgState *obj);
static uint64_t case_4(MelgState *obj);

/* initializes melg[NN] and lung with a seed */
void MelgState_init(MelgState *obj, uint64_t seed)
{
    uint64_t *x = obj->melg;
    obj->melg[0] = seed;
    for (obj->pos = 1; obj->pos < NN; obj->pos++) {
        x[obj->pos] = (6364136223846793005ULL * (x[obj->pos-1] ^ (x[obj->pos-1] >> 62)) + obj->pos);
    }
    obj->lung = (6364136223846793005ULL * (x[obj->pos-1] ^ (x[obj->pos-1] >> 62)) + obj->pos);
    obj->pos = 0;
    obj->function_p = case_1;
}

static inline uint64_t case_generic(MelgState *obj, int sh1, int sh2, int sh3,
    uint64_t (*new_function_p)(MelgState *obj))
{
    uint64_t *st = obj->melg;
    uint64_t x = (st[obj->pos] & MASKU) | (st[obj->pos+1] & MASKL);
    obj->lung = (x >> 1) ^ mag01[(int)(x & 1ULL)] ^ st[obj->pos + sh1] ^ MAT3NEG(23, obj->lung);
    st[obj->pos] = x ^ MAT3POS(33, obj->lung);
    x = st[obj->pos] ^ (st[obj->pos] << SHIFT1);
    x = x ^ (st[obj->pos + sh2] & MASK1);
    ++obj->pos;
    if (obj->pos == sh3) obj->function_p = new_function_p;
    return x;
}

static uint64_t case_1(MelgState *obj)
{
    return case_generic(obj, MM, LAG1, NN - MM, case_2);
}

static uint64_t case_2(MelgState *obj)
{
    return case_generic(obj, MM - NN, LAG1, LAG1over, case_3);
}

static uint64_t case_3(MelgState *obj)
{
    return case_generic(obj, MM - NN, -LAG1over, NN - 1, case_4);
}

static uint64_t case_4(MelgState *obj)
{
    uint64_t x = (obj->melg[NN-1] & MASKU) | (obj->melg[0] & MASKL);
    obj->lung = (x >> 1) ^ mag01[(int)(x & 1ULL)] ^ obj->melg[MM-1] ^ MAT3NEG(23, obj->lung);
    obj->melg[NN-1] = x ^ MAT3POS(33, obj->lung);
    x = obj->melg[obj->pos] ^ (obj->melg[obj->pos] << SHIFT1);
    x = x ^ (obj->melg[obj->pos - LAG1over] & MASK1);
    obj->pos = 0;
    obj->function_p = case_1;
    return x;
}


static uint64_t get_bits64_raw(void *param, void *state)
{
    MelgState *obj = state;
    (void) param;
    return obj->function_p(obj);
}

static void *init_state(void)
{
    MelgState *obj = intf.malloc(sizeof(MelgState));
    MelgState_init(obj, intf.get_seed64());
    return obj;
}

static int run_self_test()
{
    static const uint64_t u_ref[8] = {
        0xFD8CB9CF4BC4559C, 0x87F27BBAB1783F0C,
        0x20CBC729903405F5, 0x34F9EEC9F703423F,
        0xD087B4E4254D402A, 0xED3ED50A1544AF3A,
        0x42FE02FAEC84E745, 0x9077EF2F4F577294
    };
    int is_ok = 1;
    MelgState *obj = intf.malloc(sizeof(MelgState));
    MelgState_init(obj, 1234567890);
    for (int i = 0; i < 100000; i++) {
        (void) get_bits64_raw(NULL, obj);
    }
    intf.printf("%16s %16s\n", "Output", "Reference");
    for (int i = 0; i < 8; i++) {
        uint64_t u = get_bits64_raw(NULL, obj);
        intf.printf("%16llX %16llX\n",
            (unsigned long long) u, (unsigned long long) u_ref[i]);
        if (u != u_ref[i]) {
            is_ok = 0;
        }
    }
    return is_ok;
}

MAKE_UINT64_UPTO32_PRNG("Melg19937", run_self_test)
