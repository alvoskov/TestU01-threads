/**
 * @file alfib_mod_shared.c
 * @brief A shared library that implements the modified additive
 * Lagged Fibbonaci generator \f$ LFib(2^{64}, 55, 24, +) \f$.
 * @details It uses the next recurrent formula:
 *
 * \f[
 *   Y_{n} = Y_{n - 55} + Y_{n - 24} \mod 2^{64}
 * \f]
 *
 * \f[
 *   W_{n} = W_{n - 1} + \gamma \mod 2^{64}
 * \f]
 *
 * \f[
 *   X_{n} = Y_{n} XOR W_{n}
 * \f]
 *
 * and returns either higher 32 bits (as unsigned integer) or higher
 * 52 bits (as double). The initial values in the ring buffer are filled
 * by the 64-bit PCG generator.
 *
 * It passes SmallCrush, Crush and BigCrush batteries.
 */
#include "testu01_mt_cintf.h"

PRNG_CMODULE_PROLOG

#define LFIB_A 607
#define LFIB_B 273

typedef struct {
    uint64_t U[LFIB_A + 1]; /**< Ring buffer (only values 1..17 are used) */
    uint64_t w; /**< "Weyl sequence */
    int i;
    int j;
} ALFib_State;

static inline uint64_t get_bits64_raw(void *param, void *state)
{
    ALFib_State *obj = (ALFib_State *) state;
    (void) param;
    uint64_t x = obj->U[obj->i] + obj->U[obj->j];
    obj->U[obj->i] = x;
    obj->w += 0x9E3779B97F4A7C15;//UINT64_C(0xd1342543de82ef95)*obj->w + 1;
    if(--obj->i == 0) obj->i = LFIB_A;
	if(--obj->j == 0) obj->j = LFIB_A;
    return x ^ obj->w;
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
    obj->w = pcg_bits64(&state);
    return (void *) obj;
}

MAKE_UINT64_UPTO32_PRNG("ALFib_mod", NULL);
