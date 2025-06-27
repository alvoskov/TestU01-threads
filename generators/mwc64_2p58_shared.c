/**
 * @file mwc64_2p58.c
 * @brief 64-bit multiply-with-carry PRNG taken from KISS64 generator
 * by George Marsaglia.
 * @details It is developed by George Marsaglia. References:
 *
 * - https://groups.google.com/g/comp.lang.fortran/c/qFv18ql_WlU
 * - https://www.thecodingforums.com/threads/64-bit-kiss-rngs.673657/
 * - https://ssau.ru/pagefiles/sbornik_pit_2021.pdf
 *
 * @copyright (c) 2025 Alexey L. Voskov, Lomonosov Moscow State University.
 * alvoskov@gmail.com
 *
 * This software is licensed under the MIT license.
 *
 * KISSS64 algorithm was developed by George Marsaglia.
 */
#include "testu01th/cinterface.h"

PRNG_CMODULE_PROLOG

typedef struct {
    uint64_t x;  ///< MWC state 1
    uint64_t c;  ///< MWC state 2
} Mwc64State;


static inline unsigned long get_bits32_raw(void *param, void *state)
{
    Mwc64State *obj = state;
    uint64_t t = (obj->x << 58) + obj->c;
    (void) param;
    obj->c = obj->x >> 6;
    obj->x += t;
    obj->c += (obj->x < t);
    return obj->x >> 32;
}


static void *init_state()
{
    //const uint64_t mask58 = 0x3FFFFFFFFFFFFFFULL;
    Mwc64State *obj = intf.malloc(sizeof(Mwc64State));
    do { obj->x = intf.get_seed64(); } while (obj->x == 0);
    obj->c = 0;
    return (void *) obj;
}

MAKE_UINT32_PRNG("Mwc64_2p58", NULL)
