/**
 * @file msws_shared.c
 * @brief Implementatio of "Middle-Square Weyl Sequence PRNG" 
 * @details Passes SmallCrush, Crush, ??? and ???.
 * References:
 * 1. Bernard Widynski Middle-Square Weyl Sequence RNG
 *    https://arxiv.org/abs/1704.00358
 */
#include "testu01_mt_cintf.h"

PRNG_CMODULE_PROLOG

/**
 * @brief Middle-square Weyl sequence PRNG state.
 */
typedef struct {
    uint64_t x; ///< Buffer for output function/
    uint64_t w; ///< "Weyl sequence" counter.
} MswsState;


static inline unsigned long get_bits32_raw(void *param, void *state)
{
    static const uint64_t s = 0xb5ad4eceda1ce2a9;
    MswsState *obj = state;
    (void) param;
    obj->x *= obj->x;
    obj->x += (obj->w += s);
    return obj->x = (obj->x >> 32) | (obj->x << 32);
}


static void *init_state()
{
    MswsState *obj = intf.malloc(sizeof(MswsState));
    obj->x = intf.get_seed64();
    obj->w = intf.get_seed64();
    return (void *) obj;
}


MAKE_UINT32_PRNG("Msws", NULL)
