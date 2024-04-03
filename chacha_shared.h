/**
 * @brief ChaCha8 pseudorandom number generator. It is a modification of
 * cryptographically secure ChaCha20 with reduced number of rounds.
 * It should pass all statistical tests from the TestU01 library.
 */
#ifndef __CHACHA_SHARED_H
#define __CHACHA_SHARED_H
#include "testu01_mt_cintf.h"

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
