/**
 * @file alfib_shared.c
 * @brief A shared library that implements the additive
 * Lagged Fibbonaci generator \f$ LFib(2^{64}, 607, 273, +) \f$.
 * @details It uses the next recurrent formula:
 * \f[
 * X_{n} = X_{n - 17} + X_{n - 5}
 * \f]
 * and returns either higher 32 bits (as unsigned integer) or higher
 * 52 bits (as double). The initial values in the ring buffer are filled
 * by the 64-bit PCG generator.
 *
 * It passes SmallCrush test battery but fails Crush and BigCrush.
 * It also fails PractRand 0.94 at >128 GiB of data.
 *
 * - The failed tests in Crush: `Gap, r = 0`; `Gap, r = 22`
 * - The failed tests in BigCrush: `Gap, r = 0`; `Gap, r = 20`
 */
#include "testu01_mt_cintf.h"

PRNG_CMODULE_PROLOG

#define LFIB_A 607
#define LFIB_B 273

typedef struct {
    uint64_t U[LFIB_A + 1]; ///< Ring buffer (only values 1..17 are used)
    int i;
    int j;
} ALFib_State;

static CallerAPI intf;

static inline uint64_t get_bits64_raw(void *param, void *state)
{
    ALFib_State *obj = state;
    (void) param;
    uint64_t x = obj->U[obj->i] + obj->U[obj->j];
    obj->U[obj->i] = x;
    if(--obj->i == 0) obj->i = LFIB_A;
	if(--obj->j == 0) obj->j = LFIB_A;
    return x;
}

static void *init_state()
{
    ALFib_State *obj = (ALFib_State *) intf.malloc(sizeof(ALFib_State));
    // pcg_rxs_m_xs64 for initialization
    uint64_t state = intf.get_seed64();
    for (size_t k = 1; k <= LFIB_A; k++) {    
        obj->U[k] = pcg_bits64(&state);
    }
    obj->i = LFIB_A; obj->j = LFIB_B;
    return (void *) obj;
}

MAKE_UINT64_UPTO32_PRNG("ALFib", NULL)
