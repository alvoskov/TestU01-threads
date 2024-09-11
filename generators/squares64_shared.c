/**
 * @file squares_shared.c
 * @brief "Squares" counter-based PRNG by B.Widynski
 * @details Reference:
 *
 * - Bernard Widynski. Squares: A Fast Counter-Based RNG
 *   arXiv:2004.06278
 */
#include "testu01_mt_cintf.h"

PRNG_CMODULE_PROLOG

typedef struct {
    uint64_t ctr;
    Interleaved32Buffer i32buf;
} Squares64State;


#define ROR32(x) ( ((x) >> 32) | ((x) << 32) )

static inline uint64_t get_bits64_raw(void *param, void *state)
{    
    const uint64_t key = 0x9E3779B97F4A7C15;
    uint64_t t, x, y, z;
    Squares64State *obj = state;
    (void) param;
    y = x = obj->ctr++ * key; z = y + key;
    x = ROR32(x*x + y); // Round 1
    x = ROR32(x*x + z); // Round 2
    x = ROR32(x*x + y); // Round 3
    t = x = x*x + z; x = ROR32(x); // Round 4
    return t ^ ((x*x + y) >> 32); // Round 5
}

static void *init_state()
{
    Squares64State *obj = intf.malloc(sizeof(Squares64State));
    obj->ctr = intf.get_seed64();
    Interleaved32Buffer_init(&obj->i32buf);
    return (void *) obj;
}

MAKE_UINT64_INTERLEAVED32_PRNG("Squares64", Squares64State, NULL)
