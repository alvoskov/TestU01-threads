/**
 * @file swb_shared.c
 * @brief Subtract with borrow PRNG.
 *
 * 1. https://doi.org/10.18637/jss.v007.i03
 * 2. https://doi.org/10.1103/PhysRevLett.69.3382
 * 3. https://doi.org/10.1016/0010-4655(90)90033-W
 * 4. https://doi.org/10.1214/aoap/1177005878
 *
 * @copyright (c) 2024 Alexey L. Voskov, Lomonosov Moscow State University.
 * alvoskov@gmail.com
 *
 * This software is licensed under the MIT license.
 */
#include "testu01th/cinterface.h"

PRNG_CMODULE_PROLOG

#define SWB_A 920
#define SWB_B 856

/**
 * @brief 32-bit SWB state.
 */
typedef struct {    
    uint32_t x[SWB_A + 1];
    uint32_t c;
    int i;
    int j;
} SwbState;


static inline unsigned long get_bits32_raw(void *param, void *state)
{
    SwbState *obj = state;
    static const int64_t b = 1ll << 32;
    uint32_t x;
    (void) param;
    int64_t xj = obj->x[obj->j], xi = obj->x[obj->i];
    int64_t t = xj - xi - (int64_t) obj->c;
    if (t >= 0) {
        x = (uint32_t) t;
        obj->c = 0;
    } else {
        x = (uint32_t) (t + b);
        obj->c = 1;
    }
    obj->x[obj->i] = x;
    if (--obj->i == 0) obj->i = SWB_A;
	if (--obj->j == 0) obj->j = SWB_A;
    return x;
}


static void *init_state()
{
    SwbState *obj = intf.malloc(sizeof(SwbState));

    // pcg_rxs_m_xs64 for initialization
    uint64_t state = intf.get_seed64();
    for (int i = 0; i <= SWB_A; i++) {
        obj->x[i] = pcg_bits64(&state);
    }
    // Ensure state validity
    obj->c = 1;
    obj->x[1] |= 1;
    obj->x[2] = (obj->x[2] >> 1) << 1;
    obj->i = SWB_A; obj->j = SWB_B;
    return (void *) obj;
}


MAKE_UINT32_PRNG("SWB", NULL)
