/**
 * @file swbw_shared.c
 * @brief Subtract with borrow PRNG combined with "Weyl sequence"
 * Passes SmallCrush, Crush and BigCrush.
 *
 * 1. https://doi.org/10.1103/PhysRevLett.69.3382
 * 2. https://doi.org/10.1016/0010-4655(90)90033-W
 * 3. https://doi.org/10.1214/aoap/1177005878
 *
 * @copyright (c) 2024 Alexey L. Voskov, Lomonosov Moscow State University.
 * alvoskov@gmail.com
 *
 * This software is licensed under the MIT license.
 */
#include "testu01th/cinterface.h"

PRNG_CMODULE_PROLOG

#define SWB_A 43
#define SWB_B 22

/**
 * @brief 32-bit LCG state.
 */
typedef struct {    
    uint32_t x[SWB_A + 1];
    uint32_t c;
    uint32_t w;
    int i;
    int j;
} SwbWState;


static inline uint32_t get_bits32_raw(void *param, void *state)
{
    SwbWState *obj = state;
    uint32_t x;
    int64_t xj = obj->x[obj->j], xi = obj->x[obj->i];
    int64_t t = xj - xi - (int64_t) obj->c;
    (void) param;
    if (t >= 0) {
        x = (uint32_t) t;
        obj->c = 0;
    } else {
        x = (uint32_t) (t + 0xFFFFFFFB);
        obj->c = 1;
    }
    obj->x[obj->i] = x;
    obj->w -= 362436069;
    if (--obj->i == 0) obj->i = SWB_A;
	if (--obj->j == 0) obj->j = SWB_A;
    return x - obj->w;
}


static void *init_state(void)
{
    SwbWState *obj = intf.malloc(sizeof(SwbWState));
    obj->w = intf.get_seed64();
    for (size_t i = 1; i <= SWB_A; i++) {
        obj->x[i] = intf.get_seed64();
    }    
    obj->c = 1;
    obj->x[1] |= 1;
    obj->x[2] = (obj->x[2] >> 1) << 1;
    obj->i = SWB_A; obj->j = SWB_B;
    return (void *) obj;
}


MAKE_UINT32_PRNG("SWBW", NULL)
