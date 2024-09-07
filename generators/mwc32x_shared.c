/**
 * @file mwc32x_shared.c
 * @brief MWC32X - 32-bit PRNG based on MWC method.
 * @details Multiply-with-carry PRNG with a simple output function x ^ c.
 * Has a period about 2^30. Generates 16-bit numbers that are concatenated
 * to 32-bit numbers. Passes SmallCrush but not Crush or BigCrush.
 * Passes PractRand on 64MiB of data.
 *
 * This PRNG is a truncated version of MWC64X proposed by David B. Thomas.
 * MWC itself is invented by G. Marsaglia.
 *
 * References:
 * 1. David B. Thomas. The MWC64X Random Number Generator.
 *    https://cas.ee.ic.ac.uk/people/dt10/research/rngs-gpu-mwc64x.html
 * 2. G. Marsaglia "Multiply-With-Carry (MWC) generators" (from DIEHARD
 *    CD-ROM) https://www.grc.com/otg/Marsaglia_MWC_Generators.pdf
 * 3. https://github.com/lpareja99/spectral-test-knuth
 */
#include "testu01_mt_cintf.h"

PRNG_CMODULE_PROLOG

/**
 * @brief MWC32X state. Cannot be initialized to 0 or 2^32 - 1.
 */
typedef struct {
    uint32_t data;
} MWC32XState;

/**
 * @brief MWC32X algorithm implementation.
 * @details The multiplier were selected by using spectral tests
 * from TAOCP and SmallCrush test.
 */
static inline uint32_t get_bits16(void *param, void *state)
{
    const uint16_t A0 = 63885; // Selected from Knuth spectral test
    MWC32XState *obj = state;
    (void) param;
    uint16_t c = obj->data >> 16;
    uint16_t x = obj->data & 0xFFFF;
    obj->data = A0 * x + c;
    return x ^ c;
}

/**
 * @brief Concatenates two 16-bit numbers to the 32-bit value.
 */
static inline uint32_t get_bits32_raw(void *param, void *state)
{
    uint32_t hi = get_bits16(param, state);
    uint32_t lo = get_bits16(param, state);    
    return (hi << 16) | lo;
}


static void *init_state()
{
    MWC32XState *obj = intf.malloc(sizeof(MWC32XState));
    do {
        obj->data = intf.get_seed64();
    } while (obj->data == 0 || obj->data == 0xFFFFFFFF);
    return (void *) obj;
}

MAKE_UINT32_PRNG("MWC32X", NULL)
