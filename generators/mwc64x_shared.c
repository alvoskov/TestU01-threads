/**
 * @file mwc64x_shared.c
 * @brief MWC64X - 64-bit PRNG based on MWC method.
 * @details Multiply-with-carry PRNG with a simple output function x ^ c.
 * Has a period about 2^63. Passes SmallCrush, Crush and BigCrush tests.
 *
 * This PRNG is was proposed by David B. Thomas. MWC itself is invented
 * by G. Marsaglia. The A0 multiplier was changed by A.L. Voskov using
 * spectral test from TAOCP and SmallCrush for the version without XORing.
 *
 * Without XORing this PRNG fails "birthdayspacing t=3 (N12)" test from
 * Crush battery. The similar problem was noticed by S.Vigna in MWC128,
 * and he proposed a similar (but not the same) solution with XORing.
 *
 * References:
 * 1. David B. Thomas. The MWC64X Random Number Generator.
 *    https://cas.ee.ic.ac.uk/people/dt10/research/rngs-gpu-mwc64x.html
 * 2. G. Marsaglia "Multiply-With-Carry (MWC) generators" (from DIEHARD
 *    CD-ROM) https://www.grc.com/otg/Marsaglia_MWC_Generators.pdf
 * 3. https://github.com/lpareja99/spectral-test-knuth
 * 4. Sebastiano Vigna. MWC128. https://prng.di.unimi.it/MWC128.c
 */
#include "testu01_mt_cintf.h"

PRNG_CMODULE_PROLOG

/**
 * @brief MWC64X state.
 */
typedef struct {
    uint64_t data;
} MWC64XState;

static inline uint32_t get_bits32_raw(void *param, void *state)
{
    const uint64_t A0 = 0xff676488; // 2^32 - 10001272
    MWC64XState *obj = state;
    (void) param;
    uint32_t c = obj->data >> 32;
    uint32_t x = obj->data & 0xFFFFFFFF;
    obj->data = A0 * x + c;
    return x ^ c; // XORing masks issue with BirthdaySpacing (t=3, Crush N12)
}

static void *init_state()
{
    MWC64XState *obj = intf.malloc(sizeof(MWC64XState));
    do {
        obj->data = intf.get_seed64();
    } while (obj->data == 0 || obj->data == 0xFFFFFFFFFFFFFFFF);
    return (void *) obj;
}

MAKE_UINT32_PRNG("MWC64X", NULL)
