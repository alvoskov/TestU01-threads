/**
 * @file cmwc4827.c
 * @brief A complementary multiply-with-carry generator MWC4827 by G.Marsaglia.
 * @details It is a typical MWC generator with a large 256 lag designed for
 * 32-bit computers like 80386. It seems that there were several versions with
 * several different multipliers, we use one from [1-3].
 *
 * References:
 *
 * 1. https://www.thecodingforums.com/threads/the-cmwc4827-rng-an-improvement-on-mwc4691.736178/
 *
 * @copyright The CMWC4827 algorithm was developed by G. Marsaglia.
 *
 * Adaptation for SmokeRand:
 *
 * (c) 2025 Alexey L. Voskov, Lomonosov Moscow State University.
 * alvoskov@gmail.com
 */
#include "testu01th/cinterface.h"

PRNG_CMODULE_PROLOG

/**
 * @brief CMWC4827 PRNG state.
 */
typedef struct {
    uint32_t x[4827]; ///< Generated values
    uint32_t c; ///< Carry
    int pos; ///< Current position in the buffer
} Cmwc4827State;


static void Cmwc4827State_init(Cmwc4827State *obj, uint32_t xcng, uint32_t xs)
{
    for (int i = 0; i < 4827; i++) {
        xcng = 69069u * xcng + 13579u;
        xs ^= (xs << 13);
        xs ^= (xs >> 17);
        xs ^= (xs << 5);
        obj->x[i] = xcng + xs;
    }
    obj->c = 0;
    obj->pos = 4827;
    obj->c = 1271;
}

static inline unsigned long get_bits32_raw(void *param, void *state)
{
    Cmwc4827State *obj = state;
    obj->pos = (obj->pos < 4826) ? obj->pos + 1 : 0;
    uint32_t x = obj->x[obj->pos];
    uint32_t t = (x << 12) + obj->c;
    obj->c = (x >> 20) - (t < x);
    (void) param;
    return obj->x[obj->pos] = ~(t - x);
}

static void *init_state()
{
    Cmwc4827State *obj = intf.malloc(sizeof(Cmwc4827State));
    uint64_t s = intf.get_seed64();
    Cmwc4827State_init(obj, s >> 32, (s & 0xFFFFFFFF) | 0x1);
    return obj;
}


static int run_self_test()
{
    uint32_t x, x_ref = 1346668762;
    Cmwc4827State *obj = intf.malloc(sizeof(Cmwc4827State));
    Cmwc4827State_init(obj, 123456789, 362436069); 
    for (unsigned long i = 0; i < 1000000000; i++) {
        x = (uint32_t) get_bits32_raw(NULL, obj);
    }
    intf.printf("x = %22u; x_ref = %22u\n", x, x_ref);
    intf.free(obj);
    return x == x_ref;
}


#if 0
int main(void)
{unsigned long int i,x,cng=123456789,xs=362436069;
/* First seed Q[] with CNG+XS: */
for(i=0;i<4827;i++) Q=CNG+XS;
/* Then generate 10^9 CMWC4827()s */
for(i=0;i<1000000000;i++) x=CMWC4827();
printf("Does x=1346668762?\n x=%lu\n",x);
return 0;
}

#endif


MAKE_UINT32_PRNG("Cmwc8222", run_self_test)
