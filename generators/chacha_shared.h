/**
 * @file chacha_shared.h
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
#ifndef __CHACHA_SHARED_H
#define __CHACHA_SHARED_H
#include "testu01_mt_cintf.h"

#ifdef  __AVX__
#define CHACHA_VECTOR_INTR
#endif

typedef struct {
    uint32_t x[16]; /**< Working state */
    uint32_t out[16]; /**< Output state */
    size_t ncycles; /**< Number of rounds / 2 */
    size_t pos;
} ChaChaState;

void EXPORT ChaCha_block(ChaChaState *obj);
void EXPORT ChaCha_init(ChaChaState *obj, size_t nrounds, const uint32_t *seed);
int EXPORT gen_initlib(CallerAPI *api);
int EXPORT gen_closelib();
int EXPORT gen_getinfo(GenInfoC *gi);

#endif
