/**
 * @file lfib4_shared.c 
 * @brief An implementation of LFIB4 generator by G.Marsaglia.
 * Fails some statistical tests due to low quality of lower bits. 
 * @details References:
 *
 * 1. http://www.cse.yorku.ca/~oz/marsaglia-rng.html
 */
#include "testu01th/cinterface.h"

PRNG_CMODULE_PROLOG

typedef struct {
    uint32_t t[256];
    uint8_t c;
} LFib4State;

static inline unsigned long get_bits32_raw(void *param, void *state)
{
    LFib4State *obj = state;
    uint32_t *t = obj->t;
    (void) param;
    obj->c++;
    uint8_t c1 = obj->c + 58, c2 = obj->c + 119, c3 = obj->c + 178;
    t[obj->c] += t[c1] + t[c2] + t[c3];
    return t[obj->c];
}

/**
 * @param jcong0 Marsaglia's default is 12345
 */
static void LFib4State_init(LFib4State *obj, uint32_t jcong0)
{
    uint32_t z = 12345, w = 65435, xs = 34221, jcong = jcong0;
    // Use Marsaglia's original procedure for LFIB4 initailization
    //uint64_t state = intf->get_seed64();
    for (int i = 0; i < 256; i++) {
        // KISS iteration
        z = 36969 * (z & 65535) + (z >> 16);
        w = 18000 * (w & 65535) + (w >> 16);
        jcong = 69069 * jcong + 1234567;
        xs ^= (xs << 17);
        xs ^= (xs >> 13);
        xs ^= (xs << 5);
        uint32_t mwc = (z << 16) + w;
        uint32_t kiss = (mwc ^ jcong) + xs;
        // Write to LFIB4 table
        obj->t[i] = kiss;
    }
    obj->c = 0;
}

static void *init_state()
{
    LFib4State *obj = intf.malloc(sizeof(LFib4State));
    LFib4State_init(obj, 12345);
    return (void *) obj;
}

static int run_self_test()
{
    uint32_t x, x_ref = 1064612766;
    LFib4State *obj = intf.malloc(sizeof(LFib4State));
    LFib4State_init(obj, 12345); 
    for (unsigned long i = 0; i < 1000000; i++) {
        x = get_bits32_raw(NULL, obj);
    }
    intf.printf("x = %22u; x_ref = %22u\n", x, x_ref);
    intf.free(obj);
    return x == x_ref;
}



MAKE_UINT32_PRNG("LFib4", run_self_test)
