#include "testu01_mt_cintf.h"
#include <limits.h>

#define LFIB_A 98
#define LFIB_B 33

/////////////////////////////////////////////////
///// Entry point for -nostdlib compilation /////
/////////////////////////////////////////////////
SHARED_ENTRYPOINT_CODE


static CallerAPI intf;

static const double c = 5566755282872655.0 / 9007199254740992.0; /**< shift */
//static const double c = 362436069876.0 / 9007199254740992.0; /**< original shift */


static inline double amb_mod_r(double a, double b)
{
    static const double r = 9007199254740881.0 / 9007199254740992.0; /**< base (prime) */
    double x = a - b;
    return (x >= 0.0) ? x : (x + r);
}

typedef struct {
    double U[LFIB_A + 1];
    double z;
    int i;
    int j;
} LFibFloat;

static double get_u01(void *param, void *state)
{
    LFibFloat *obj = (LFibFloat *) state;
    (void) param;
    /* Subtractive Lagged Fibbonaci part */
    double x = obj->U[obj->i] - obj->U[obj->j];
    if (x < 0.0) x += 1.0;
    obj->U[obj->i] = x;
    if(--obj->i == 0) obj->i = LFIB_A;
	if(--obj->j == 0) obj->j = LFIB_A;
    /* Nested Weyl sequence part */
    obj->z = amb_mod_r(obj->z, c);
    /* Combine two generators */
    x -= obj->z;
    return (x < 0.0) ? (x + 1.0) : x;
}

static long unsigned int get_bits32(void *param, void *state)
{
    const double m_2_pow_32 = 4294967296.0;
    return get_u01(param, state) * m_2_pow_32;
}

static void *init_state()
{
    LFibFloat *obj = (LFibFloat *) intf.malloc(sizeof(LFibFloat));
    uint64_t seed = intf.get_seed64();
    obj->z = uint64_to_udouble(seed);
    double w[5];
    for (size_t i = 0; i < 5; i++)
        w[i] = obj->z;
    for (size_t i = 1; i <= LFIB_A; i++) {
        obj->z = amb_mod_r(obj->z, c);
        w[0] = amb_mod_r(w[0], obj->z);
        for (size_t j = 1; j < 5; j++) 
            w[j] = amb_mod_r(w[j], w[j - 1]);
        obj->U[i] = w[4];
    }

/*
    uint64_t seed = intf.get_seed64();
    uint32_t x = seed & 0xFFFFFFFF;
    uint32_t y = seed >> 32;
    for (size_t i = 1; i < LFIB_A; i++) {
        double s = 0.0, t = 0.5;
        for (size_t j = 1; j < 54; j++) {
            x = (6969*x) % 65543;
            y = (8888*y) % 65579; // Original work: y = (8888*x) % 65579;
            if (((x ^ y) & 32) > 0)
                s += t;
            t *= 0.5;
        }
        obj->U[i] = s;
    }
*/


    obj->i = LFIB_A; obj->j = LFIB_B;
    return (void *) obj;
}

static void delete_state(void *param, void *state)
{
    (void) param;
    intf.free(state);
}

int EXPORT gen_initlib(CallerAPI *intf_)
{
    intf = *intf_;
    return 1;
}

int EXPORT gen_closelib()
{
    return 1;
}

int EXPORT gen_getinfo(GenInfoC *gi)
{
    static const char name[] = "LFib_RANMAR64";
    gi->name = name;
    gi->init_state = init_state;
    gi->delete_state = delete_state;
    gi->get_u01 = get_u01;
    gi->get_bits32 = get_bits32;
    return 1;
}
