/**
 * @file sqxor_shared.c
 * @brief PRNG inspired by the Von Neumann middle squares method and
 * its modification by B.Widynski. Requires 128-bit integers.
 *
 */
#include "testu01_mt_cintf.h"

/////////////////////////////////////////////////
///// Entry point for -nostdlib compilation /////
/////////////////////////////////////////////////
SHARED_ENTRYPOINT_CODE

static CallerAPI intf;

typedef struct {
    uint64_t w; /**< "Weyl sequence" counter state */
    union {
        uint32_t dw_ary[2];
        uint64_t qw_val;
    } buf_32b; /**< Needed for conversion of 64-bit output to 32-bit */
    size_t pos_32b; /**< Position for 32-bit output */
} SqXorState;


static uint64_t get_bits64(void *param, void *state)
{
    const uint64_t s = UINT64_C(0x9E3779B97F4A7C15);
    SqXorState *obj = (SqXorState *) state;
    (void) param;
    uint64_t ww = obj->w += s; // or ++obj->w ^ s;
    // Round 1
    __int128 sq = ((__int128) ww) * ww; // |32bit|32bit||32bit|32bit||
    uint64_t x = (sq >> 64) ^ sq; // Middle squares (64 bits) + XORing
    // Round 2
    sq = ((__int128) x) * ww;
    x = (sq >> 64) ^ sq; // Middle squares (64 bits) + XORing
    // Return the result
    return x;
}

static long unsigned int get_bits32(void *param, void *state)
{
    SqXorState *obj = (SqXorState *) state;
    if (obj->pos_32b >= 2) {
        obj->buf_32b.qw_val = get_bits64(param, state);
        obj->pos_32b = 0;
    }
    return obj->buf_32b.dw_ary[obj->pos_32b++];
}

static double get_u01(void *param, void *state)
{
    return uint64_to_udouble(get_bits64(param, state));
}

static void *init_state()
{
    SqXorState *obj = (SqXorState *) intf.malloc(sizeof(SqXorState));
    obj->w = intf.get_seed64();
    obj->pos_32b = 2;
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
    gi->init_state = init_state;
    gi->delete_state = delete_state;
    gi->get_u01 = get_u01;
    gi->get_bits32 = get_bits32;
    gi->get_bits64 = get_bits64;
    return 1;
}
