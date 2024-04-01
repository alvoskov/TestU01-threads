#include "testu01_mt_cintf.h"
#include <stdio.h>
#include <stdlib.h>

#define LFIB_A 17
#define LFIB_B 5

static const double c = 5566755282872655.0 / 9007199254740992.0; /**< shift */

static inline double amb_mod_r(double a, double b)
{
    static const double r = 9007199254740881.0 / 9007199254740992.0; /**< base (prime) */
    double x = a - b;
    return (x >= 0.0) ? x : (x + r);
}

typedef struct {
    double z;
    double w;
    double w2;
    double U[LFIB_A]; /**< Buffer for pseudorandom number */
    size_t pos;
    
} LFibFloat;


static void fill_u01(void *param, void *state)
{
    LFibFloat *obj = (LFibFloat *) state;
    (void) param;

    printf("===>\n");
    for (size_t i = 0; i < LFIB_A; i++) {
        printf("%g ", obj->U[i]);
    }
    printf("\n");


    /* Phase 1: calculate elements that depend on the previous
       content of the buffer */
    for (size_t i = LFIB_A - 1; i >= LFIB_A - LFIB_B; i--) {
        obj->U[i] -= obj->U[i - (LFIB_A - LFIB_B)];
    }    
    for (size_t i = LFIB_A - 1; i >= LFIB_A - LFIB_B; i--) {
        if (obj->U[i] < 0.0) obj->U[i] += 1.0;
        printf("(%d) ", i);
    }
    /* Phase 2: calculate elements that depend on the elements
       calculate at Phase 1 */
    for (size_t i = LFIB_A - LFIB_B, j = LFIB_A - 1; i-- != 0; j--) {
        obj->U[i] -= obj->U[j];
        if (i == 0) printf("====>%d", j);
    }
    for (size_t i = 0; i < LFIB_A - LFIB_B; i++) {
        if (obj->U[i] < 0.0) obj->U[i] += 1.0;
        printf("<%d> ", i);
    }


    for (size_t i = 0; i < LFIB_A; i++) {
        printf("%g ", obj->U[i]);
    }
    printf("\n\n");
}


static double get_u01(void *param, void *state)
{
    LFibFloat *obj = (LFibFloat *) state;
    (void) param;

    /* Subtractive Lagged Fibbonaci part */
    double x = obj->U[obj->pos++];
    if (obj->pos == LFIB_A) {
        fill_u01(param, state);
        obj->pos = 0;
    }
    return x;
    
/*
    double x = obj->U[obj->i] - obj->U[obj->j];
    if (x < 0.0) x += 1.0;
    obj->U[obj->i] = x;
    if(--obj->i == 0) obj->i = LFIB_A;
	if(--obj->j == 0) obj->j = LFIB_A;
*/
    /* Nested Weyl sequence part */
/*
    obj->z = amb_mod_r(obj->z, c);
    obj->w = amb_mod_r(obj->w, obj->z);
    obj->w2 = amb_mod_r(obj->w2, obj->w);
*/
    /* Combine two generators */
/*
    x -= obj->w2;
    return (x < 0.0) ? (x + 1.0) : x;
*/
}

static long unsigned int get_bits32(void *param, void *state)
{
    const double m_2_pow_32 = 4294967296.0;
    return get_u01(param, state) * m_2_pow_32;
}

static void *init_state()
{
    LFibFloat *obj = (LFibFloat *) malloc(sizeof(LFibFloat));
    uint32_t seed = prng_seed32();
    obj->z = (double) seed / UINT_MAX;
    obj->w = obj->z;
    obj->w2 = obj->z;
    double w2 = obj->z, w3 = obj->z;
    for (size_t k = 0; k < LFIB_A; k++) {
        obj->z = amb_mod_r(obj->z, c);
        obj->w = amb_mod_r(obj->w, obj->z);
        w2 = amb_mod_r(w2, obj->w);
        w3 = amb_mod_r(w3, w2);
        obj->U[k] = w3;
    }
    obj->pos = LFIB_A - 1;
    return (void *) obj;
}

static void delete_state(void *param, void *state)
{
    (void) param;
    free(state);
}

int EXPORT gen_initlib()
{
    return 1;
}

int EXPORT gen_closelib()
{
    return 1;
}

int EXPORT gen_getinfo(GenInfoC *gi)
{
    static const char name[] = "LFib_float";
    gi->name = name;
    gi->init_state = init_state;
    gi->delete_state = delete_state;
    gi->get_u01 = get_u01;
    gi->get_bits32 = get_bits32;
    return 1;
}
