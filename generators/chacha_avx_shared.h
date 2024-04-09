/**
 * @file chacha_avx_shared.h
 * @brief ChaCha12 pseudorandom number generator (AVX version). 
 * @details SIMD version of ChaCha12 generator that uses 256-bit YMM CPU
 * registers. Usage of AVX instructions gives about 2x speedup.
 * See chacha_shared.h for the detailed comments about the generator.
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
#ifndef __CHACHA_AVX_SHARED_H
#define __CHACHA_AVX_SHARED_H
#include "testu01_mt_cintf.h"


/**
 * @brief Contains the state for two parallel ChaCha states.
 * @details The next memory layout in 1D array is used:
 *
 * | 0   1  2  3 |  4  5  6  7 |
 * | 8   9 10 11 | 12 13 14 15 |
 * | 16 17 18 19 | 20 21 22 23 |
 * | 24 25 26 27 | 28 29 30 31 |
 */
typedef struct {
    uint32_t x[32]; /**< Working state */
    uint32_t out[32]; /**< Output state */
    size_t ncycles; /**< Number of rounds / 2 */
    size_t pos;
} ChaChaState;

void EXPORT ChaCha_block(ChaChaState *obj);
void EXPORT ChaCha_init(ChaChaState *obj, size_t nrounds, const uint32_t *seed);
int EXPORT gen_initlib(CallerAPI *api);
int EXPORT gen_closelib();
int EXPORT gen_getinfo(GenInfoC *gi);

#endif
