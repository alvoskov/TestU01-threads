/**
 * @file mlfib17_5_shared.c
 * @brief A shared library that implements the multiplicative
 * Lagged Fibbonaci generator \f$ LFib(2^{64}, 17, 5, *) \f$.
 * @details It uses the next recurrent formula:
 * \f[
 * X_{n} = X_{n - 17} * X_{n - 5}
 * \f]
 * and returns either higher 32 bits (as unsigned integer) or higher
 * 52 bits (as double). The initial values in the ring buffer are filled
 * by the 64-bit PCG generator.
 *
 * It should pass SmallCrush, Crush, BigCrush and
 * pseudoDIEHARD test batteries.
 */
#include "testu01_mt_cintf.h"

#define LFIB_A 17
#define LFIB_B 5

PRNG_CMODULE_PROLOG

/**
 * @brief LFIB(LFIB_A, LFIB_B, *) PRNG state.
 */
typedef struct {
    uint64_t U[LFIB_A + 1]; ///< Ring buffer (only values 1..17 are used)
    int i;
    int j;
} MLFib17_5_State;


static inline uint64_t get_bits64_raw(void *param, void *state)
{
    MLFib17_5_State *obj = state;
    (void) param;
    uint64_t x = obj->U[obj->i] * obj->U[obj->j];
    obj->U[obj->i] = x;
    if(--obj->i == 0) obj->i = LFIB_A;
	if(--obj->j == 0) obj->j = LFIB_A;
    return x;
}


static void *init_state()
{
    MLFib17_5_State *obj = intf.malloc(sizeof(MLFib17_5_State));
    // pcg_rxs_m_xs64 for initialization
    uint64_t state = intf.get_seed64();
    for (size_t k = 1; k <= LFIB_A; k++) {    
        obj->U[k] = pcg_bits64(&state) | 0x1;
    }
    obj->i = LFIB_A; obj->j = LFIB_B;
    return (void *) obj;
}

MAKE_UINT64_UPTO32_PRNG("MLFib17_5", NULL)
