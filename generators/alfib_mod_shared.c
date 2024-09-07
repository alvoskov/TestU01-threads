/**
 * @file alfib_mod_shared.c
 * @brief A shared library that implements the modified additive
 * Lagged Fibbonaci generator \f$ LFib(2^{64}, 55, 24, +) \f$.
 * @details It uses the next recurrent formula:
 * \f[
 * X_{n} = X_{n - 55} + X_{n - 24}
 * \f]
 * and returns either higher 32 bits (as unsigned integer) or higher
 * 52 bits (as double). The initial values in the ring buffer are filled
 * by the 64-bit PCG generator.
 *
 * It passes SmallCrush, Crush and BigCrush batteries.
 */
#include "testu01_mt_cintf.h"

/////////////////////////////////////////////////
///// Entry point for -nostdlib compilation /////
/////////////////////////////////////////////////
SHARED_ENTRYPOINT_CODE

#define LFIB_A 607
#define LFIB_B 273

typedef struct {
    uint64_t U[LFIB_A + 1]; /**< Ring buffer (only values 1..17 are used) */
    uint64_t w; /**< "Weyl sequence */
    int i;
    int j;
} ALFib_State;

static CallerAPI intf;

static uint64_t get_bits64(void *param, void *state)
{
    ALFib_State *obj = (ALFib_State *) state;
    (void) param;
    uint64_t x = obj->U[obj->i] + obj->U[obj->j];
    obj->U[obj->i] = x;
    obj->w += 0x9E3779B97F4A7C15;//UINT64_C(0xd1342543de82ef95)*obj->w + 1;
    if(--obj->i == 0) obj->i = LFIB_A;
	if(--obj->j == 0) obj->j = LFIB_A;
    return x ^ obj->w;
}


static long unsigned int get_bits32(void *param, void *state)
{
    return get_bits64(param, state) >> 32;
}

static double get_u01(void *param, void *state)
{
    double u = uint64_to_udouble(get_bits64(param, state));
    return u;
}

static void *init_state()
{
    ALFib_State *obj = (ALFib_State *) intf.malloc(sizeof(ALFib_State));
    // pcg_rxs_m_xs64 for initialization
    uint64_t state = intf.get_seed64();
    for (size_t k = 1; k <= LFIB_A; k++) {    
        obj->U[k] = pcg_bits64(&state);
    }
    obj->i = LFIB_A; obj->j = LFIB_B;
    obj->w = pcg_bits64(&state);
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

int EXPORT gen_closelib(void)
{
    return 1;
}

int EXPORT gen_getinfo(GenInfoC *gi)
{
    static const char name[] = "ALFib_mod";
    gi->name = name;
    gi->init_state = init_state;
    gi->delete_state = delete_state;
    gi->get_u01 = get_u01;
    gi->get_bits32 = get_bits32;
    gi->get_bits64 = get_bits64;
    return 1;
}
