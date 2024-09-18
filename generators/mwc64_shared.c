/**
 * @file mwc64_shared.c
 * @brief MWC64 - 64-bit MWC PRNG
 * @details Multiply-with-carry PRNG with a period about 2^63.
 * Passes SmallCrush, Crush and BigCrush tests.
 *
 * MWC itself is invented by G. Marsaglia. The A0 multiplier was selected by
 * A.L. Voskov using spectral test from TAOCP and SmallCrush.
 * 
 * Passes SmallCrush but fails Crush:
 *
 *
 *            Test                          p-value
 *      ----------------------------------------------
 *      12  BirthdaySpacings, t = 3          eps
 *      19  ClosePairs mNP2S, t = 3        2.0e-39
 *      71  LinearComp, r = 0               0.9995
 *      ----------------------------------------------
 *      All other tests were passed
 *
 * It also fails BigCrush
 *
 *            Test                          p-value
 *      ----------------------------------------------
 *      14  BirthdaySpacings, t = 3          eps
 *      22  ClosePairs mNP2S, t = 3          eps
 *      22  ClosePairs mNP1, t = 3         5.4e-91
 *      22  ClosePairs mNP, t = 3          5.6e-65
 *      ----------------------------------------------
 *      All other tests were passed
 *
 *
 * References:
 * 1. G. Marsaglia "Multiply-With-Carry (MWC) generators" (from DIEHARD
 *    CD-ROM) https://www.grc.com/otg/Marsaglia_MWC_Generators.pdf
 * 2. https://github.com/lpareja99/spectral-test-knuth
 *
 * @copyright (c) 2024 Alexey L. Voskov, Lomonosov Moscow State University.
 * alvoskov@gmail.com
 *
 * All rights reserved.
 *
 * This software is provided under the Apache 2 License.
 */
#include "testu01th/cinterface.h"

PRNG_CMODULE_PROLOG

/**
 * @brief MWC64 state.
 */
typedef struct {
    uint64_t data;
} MWC64State;

static inline uint32_t get_bits32_raw(void *param, void *state)
{
    const uint64_t A0 = 0xff676488; // 2^32 - 10001272
    MWC64State *obj = state;
    (void) param;
    uint32_t c = obj->data >> 32;
    uint32_t x = obj->data & 0xFFFFFFFF;
    obj->data = A0 * x + c;
    return x;
}

static void *init_state(void)
{
    MWC64State *obj = intf.malloc(sizeof(MWC64State));
    // Seeding: prevent (0,0) and (?,0xFFFF)
    do {
        obj->data = intf.get_seed64() << 1;
    } while (obj->data == 0);
    return (void *) obj;
}

MAKE_UINT32_PRNG("MWC64", NULL)
