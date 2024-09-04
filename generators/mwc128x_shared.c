/**
 * @file mwc128x_shared.c
 * @brief MWC128X - 128-bit PRNG based on MWC method.
 * @details Multiply-with-carry PRNG with a simple output function x ^ c.
 * Has period about 2^127. Passes SmallCrush, Crush and BigCrush tests.
 * It is a slight modification of MWC128 suggested by Sebastiano Vigna.
 *
 * Without XORing this PRNG may fail specially developed 3D birthday
 * spacing tests (noticed by S.Vigna). The output XOR is different from
 * used by Vigna and similar to used in MWC64X suggested by David B. Thomas.
 *
 * MWC_A1 multiplier was found by S. Vigna.
 *
 * References:
 * 1. David B. Thomas. The MWC64X Random Number Generator.
 *    https://cas.ee.ic.ac.uk/people/dt10/research/rngs-gpu-mwc64x.html
 * 2. G. Marsaglia "Multiply-With-Carry (MWC) generators" (from DIEHARD
 *    CD-ROM) https://www.grc.com/otg/Marsaglia_MWC_Generators.pdf
 * 3. Sebastiano Vigna. MWC128. https://prng.di.unimi.it/MWC128.c
 */
#include "testu01_mt_cintf.h"

PRNG_CMODULE_PROLOG

/**
 * @brief MWC128X state. Cannot be initialized to (0, 0) or to
 * (2^64 - 1, 2^64 - 1). Default initialization is (seed, 1) as suggested
 * by S. Vigna.
 */
typedef struct {
    uint64_t x;
    uint64_t c;
    union {
        uint64_t u64val; ///< To be splitted to two 32-bit values.
        uint32_t u32val[2]; ///< 32-bit values.
    } buf32; ///< Internal buffer for 32-bit outputs.
    int pos32; ///< Output position for 32-bit output.
} MWC128State;

/**
 * @brief MWC128X PRNG implementation.
 * @details A slight modification of PRNG by S.Vigna: output function
 * is x ^ c instead of (x << 32) ^ x. The multiplier is suggested
 * by Vigna.
 */
static inline uint64_t get_bits64_raw(void *param, void *state)
{
    static const uint64_t MWC_A1 = 0xffebb71d94fcdaf9;
    MWC128State *obj = state;
    const __uint128_t t = MWC_A1 * (__uint128_t) obj->x + obj->c;
    (void) param;
    obj->x = t;
    obj->c = t >> 64;
    return obj->x ^ obj->c;
}

static uint64_t get_bits64(void *param, void *state)
{
    return get_bits64_raw(param, state);
}

/**
 * @brief Decomposes each 64-bit value into a pair of 32-bit
 * values. Needed for TestU01: its batteries should have
 * an access to all bits.
 */
static long unsigned int get_bits32(void *param, void *state)
{
    MWC128State *obj = state;
    if (obj->pos32 == 2) {
        obj->buf32.u64val = get_bits64_raw(param, state);
        obj->pos32 = 0;
    }
    return obj->buf32.u32val[obj->pos32++];
}


static double get_u01(void *param, void *state)
{
    return uint64_to_udouble(get_bits64_raw(param, state));
}


static void *init_state()
{
    MWC128State *obj = intf.malloc(sizeof(MWC128State));
    obj->x = intf.get_seed64();
    obj->c = 1;
    obj->pos32 = 2;
    return (void *) obj;
}


static void get_array64(void *param, void *state, uint64_t *out, size_t len)
{
    for (size_t i = 0; i < len; i++) {
        out[i] = get_bits64_raw(param, state);
    }
}


static void delete_state(void *param, void *state)
{
    (void) param;
    intf.free(state);
}


int EXPORT gen_getinfo(GenInfoC *gi)
{
    static const char name[] = "MWC128X";
    gi->name = name;
    gi->init_state = init_state;
    gi->delete_state = delete_state;
    gi->get_u01 = get_u01;
    gi->get_bits32 = get_bits32;
    gi->get_bits64 = get_bits64;
    gi->get_array64 = get_array64;
    return 1;
}
