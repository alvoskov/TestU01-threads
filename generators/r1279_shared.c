/**
 * @file r1279_shared.c
 * @brief Implementation of XOR-based lagged Fibbonaci generator
 * \f$ LFib(2^{32}, 1279, 1063 ) \f$.
 * @details It uses the next recurrent formula:
 * \f[
 * X_{n} = X_{n - 1279} XOR X_{n - 1063}
 * \f]
 * 
 * References:
 *
 * 1. 10.1016/j.cpc.2007.10.002
 * 2. https://doi.org/10.1103/PhysRevLett.69.3382
 * 3. https://doi.org/10.1103/PhysRevE.52.3205
 *
 * @copyright (c) 2024 Alexey L. Voskov, Lomonosov Moscow State University.
 * alvoskov@gmail.com
 *
 * This software is licensed under the MIT license.
 */
#include "testu01th/cinterface.h"

PRNG_CMODULE_PROLOG

/*
#define RGEN_A 1279
#define RGEN_B 1063
*/


#define RGEN_A 250
#define RGEN_B 103


typedef struct {
    uint32_t x[RGEN_A + 1]; ///< Ring buffer (u[0] is not used)
    int i;
    int j;
} RGenState;

static inline unsigned long get_bits32_raw(void *param, void *state)
{
    RGenState *obj = state;
    uint32_t x = obj->x[obj->i] ^ obj->x[obj->j];
    (void) param;
    obj->x[obj->i] = x;
    if (--obj->i == 0) obj->i = RGEN_A;
	if (--obj->j == 0) obj->j = RGEN_A;
    return x;
}

static void *init_state()
{
    RGenState *obj = intf.malloc(sizeof(RGenState));
    // pcg_rxs_m_xs64 for initialization
    uint64_t state = intf.get_seed64();
    for (size_t k = 1; k <= RGEN_A; k++) {    
        obj->x[k] = pcg_bits64(&state);
    }
    obj->i = RGEN_A; obj->j = RGEN_B;
    return (void *) obj;
}

MAKE_UINT32_PRNG("R1279", NULL)
