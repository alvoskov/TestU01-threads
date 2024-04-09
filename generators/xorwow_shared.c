/**
 * @file xorwow_shared.c
 * @brief xorwow pseudorandom number generator.
 * @details Fails the next test from BigCrush:
 * - 81  LinearComp, r = 29             1 - eps1
 * References:
 * - https://doi.org/10.18637/jss.v008.i14
 * - https://docs.nvidia.com/cuda/curand/testing.html
 *
 */
#include "testu01_mt_cintf.h"

/////////////////////////////////////////////////
///// Entry point for -nostdlib compilation /////
/////////////////////////////////////////////////
SHARED_ENTRYPOINT_CODE

static CallerAPI intf;

typedef struct {
    uint32_t x; /**< Xorshift register */
    uint32_t y; /**< Xorshift register */
    uint32_t z; /**< Xorshift register */
    uint32_t w; /**< Xorshift register */
    uint32_t v; /**< Xorshift register */
    uint32_t d; /**< "Weyl sequence" counter */
} XorWowState;

static long unsigned int get_bits32(void *param, void *state)
{
    const uint32_t d_inc = 362437;
    XorWowState *obj = (XorWowState *) state;
    (void) param;
    uint32_t t = (obj->x ^ (obj->x >> 2));
    obj->x = obj->y;
    obj->y = obj->z;
    obj->z = obj->w;
    obj->w = obj->v;
    obj->v = (obj->v ^ (obj->v << 4)) ^ (t ^ (t << 1));
    return (obj->d += d_inc) + obj->v;
}

static double get_u01(void *param, void *state)
{
    static const double INV32 = 2.3283064365386963E-10;
    return get_bits32(param, state) * INV32;
}

static void *init_state()
{
    XorWowState *obj = (XorWowState *) intf.malloc(sizeof(XorWowState));
    uint64_t s1 = intf.get_seed64();
    uint64_t s2 = intf.get_seed64();
    uint64_t s3 = intf.get_seed64();
    obj->x = (uint32_t) s1;
    obj->y = (uint32_t) (s1 >> 32);
    obj->z = (uint32_t) s2;
    obj->w = (uint32_t) (s2 >> 32);
    obj->v = (uint32_t) s3;
    obj->d = (uint32_t) (s3 >> 32);
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
    static const char name[] = "XorWow";
    gi->name = name;
    gi->init_state = init_state;
    gi->delete_state = delete_state;
    gi->get_u01 = get_u01;
    gi->get_bits32 = get_bits32;
    return 1;
}
