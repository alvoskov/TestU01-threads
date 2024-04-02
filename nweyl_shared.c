#include "testu01_mt_cintf.h"
#include <stdio.h>
#include <stdlib.h>

#define NROUNDS 10

static uint32_t global_seed;
static const double c = 5566755282872655.0 / 9007199254740992.0; /**< shift */

static inline double amb_mod_r(double a, double b)
{
    static const double r = 9007199254740881.0 / 9007199254740992.0; /**< base (prime) */
    double x = a - b;
    return (x >= 0.0) ? x : (x + r);
}

typedef struct {
    double w[NROUNDS];
} NWeyl;


static double get_u01(void *param, void *state)
{
    NWeyl *obj = (NWeyl *) state;
    (void) param;
    obj->w[0] = amb_mod_r(obj->w[0], c);
    for (size_t k = 1; k < NROUNDS; k++) {
        obj->w[k] = amb_mod_r(obj->w[k], obj->w[k - 1]);
    }
    return obj->w[NROUNDS - 1];
}

static long unsigned int get_bits32(void *param, void *state)
{
    const double m_2_pow_32 = 4294967296.0;
    return get_u01(param, state) * m_2_pow_32;
}

static void *init_state()
{
    NWeyl *obj = (NWeyl *) malloc(sizeof(NWeyl));
    for (size_t k = 0; k < NROUNDS; k++) {
        obj->w[k] = (double) global_seed / UINT_MAX;
    }
    return (void *) obj;
}

static void delete_state(void *param, void *state)
{
    (void) param;
    free(state);
}

int EXPORT gen_initlib(uint64_t seed, void *data)
{
    global_seed = seed;
    (void) data;
    return 1;
}

int EXPORT gen_closelib()
{
    return 1;
}

int EXPORT gen_getinfo(GenInfoC *gi)
{
    static const char name[] = "NWeyl";
    gi->name = name;
    gi->init_state = init_state;
    gi->delete_state = delete_state;
    gi->get_u01 = get_u01;
    gi->get_bits32 = get_bits32;
    return 1;
}
