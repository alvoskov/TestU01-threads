/**
 * @file mlfib17_5_shared.c
 * @brief A shared library that implements the multiplicative
 * Lagged Fibbonaci generator \f$ LFib(2^{64}, 17, 5, *) \f$.
 * @details It uses the next recurrent formula:
 * \f[
 * X_{n} = X_{n - 17} + X_{n - 5}
 * \f]
 * and returns either higher 32 bits (as unsigned integer) or higher
 * 52 bits (as double). The initial values in the ring buffer are filled
 * by the 64-bit PCG generator.
 *
 * It should pass SmallCrush, Crush, BigCrush and
 * pseudoDIEHARD test batteries.
 */
#include "testu01_mt_cintf.h"
#include <stdio.h>
#include <stdlib.h>

#define LFIB_A 17
#define LFIB_B 5

typedef struct {
    uint64_t U[LFIB_A + 1]; /**< Ring buffer (only values 1..17 are used) */
    int i;
    int j;
} MLFib17_5_State;


static uint64_t get_bits64(void *param, void *state)
{
    MLFib17_5_State *obj = (MLFib17_5_State *) state;
    uint64_t x = obj->U[obj->i] * obj->U[obj->j];
    obj->U[obj->i] = x;
    if(--obj->i == 0) obj->i = LFIB_A;
	if(--obj->j == 0) obj->j = LFIB_A;
    return x;
}


static long unsigned int get_bits32(void *param, void *state)
{
    return get_bits64(param, state) >> 32;
}

static double get_u01(void *param, void *state)
{
    double u = uint64_to_udouble(get_bits64(param, state));
    return u;
}

static void *init_state()
{
    MLFib17_5_State *obj = (MLFib17_5_State *) malloc(sizeof(MLFib17_5_State));
    // pcg_rxs_m_xs64 for initialization
    uint32_t seed = prng_seed32();
    uint64_t state = seed;
    printf("SEED: %X\n", seed);
    for (size_t k = 1; k <= LFIB_A; k++) {    
        obj->U[k] = pcg_bits64(&state) | 0x1;
    }
    obj->i = LFIB_A; obj->j = LFIB_B;
    return (void *) obj;
}

static void delete_state(void *param, void *state)
{
    (void) param;
    free(state);
}

int EXPORT gen_initlib(uint64_t seed, void *data)
{
    (void) seed;
    (void) data;
    return 1;
}

int EXPORT gen_closelib()
{
    return 1;
}

int EXPORT gen_getinfo(GenInfoC *gi)
{
    static const char name[] = "MLFib17_5";
    gi->name = name;
    gi->init_state = init_state;
    gi->delete_state = delete_state;
    gi->get_u01 = get_u01;
    gi->get_bits32 = get_bits32;
    return 1;
}
