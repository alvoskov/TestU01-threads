/**
 * @file mwc40xxa8.c
 * @brief MWC40XXA8.
 * @details Multiply-with-carry PRNG.
 *
 * \f[
 * x_{n} = ax_{n - 4} + c \mod 2^32
 * \f]
 * 
 *
 * References:
 * 1. https://tom-kaitchuck.medium.com/designing-a-new-prng-1c4ffd27124d
 * 2. G. Marsaglia "Multiply-With-Carry (MWC) generators" (from DIEHARD
 *    CD-ROM) https://www.grc.com/otg/Marsaglia_MWC_Generators.pdf
 * 3. Sebastiano Vigna. MWC128. https://prng.di.unimi.it/MWC128.c
 *
 * @copyright
 * (c) 2024-2025 Alexey L. Voskov, Lomonosov Moscow State University.
 * alvoskov@gmail.com
 *
 * This software is licensed under the MIT license.
 */
#include "testu01th/cinterface.h"

PRNG_CMODULE_PROLOG

/**
 * @brief MWC40XXA8 state. Cannot be initialized to (0, 0, 0, 0) or to
 * (2^64 - 1, 2^32 - 1, 2^32 - 1, 2^64 - 1). Default initialization
 * is (seed, seed, seed, 1) as suggested by S. Vigna.
 */
typedef struct {
    uint8_t x[4];
    uint8_t c;
} Mwc40xxa8State;

static inline unsigned long get_bits32_raw(void *param, void *state)
{
    static const uint8_t MWC_A1 = 227;
    Mwc40xxa8State *obj = state;
    uint32_t ans = 0;
    for (int i = 0; i < 4; i++) {
        uint16_t t = MWC_A1 * (uint64_t) obj->x[3];
        uint8_t ans8 = (obj->x[2] ^ obj->x[1]) + (obj->x[0] ^ (t >> 8));
        t += obj->c;
        obj->x[3] = obj->x[2];
        obj->x[2] = obj->x[1];
        obj->x[1] = obj->x[0];
        obj->x[0] = (uint8_t) t;
        obj->c = t >> 8;
        ans = (ans << 8) | ans8;
    }
    (void) param;
    return ans;
}

static void Mwc40xxa8State_init(Mwc40xxa8State *obj, uint32_t seed)
{
    obj->x[0] = seed & 0xFF;
    obj->x[1] = (seed >> 8) & 0xFF;
    obj->x[2] = (seed >> 16) & 0xFF;
    obj->x[3] = (seed >> 24) & 0xFF;
    obj->c = 1;
    for (int i = 0; i < 6; i++) {
        (void) get_bits32_raw(NULL, obj);
    }
}

static void *init_state(void)
{
    Mwc40xxa8State *obj = intf.malloc(sizeof(Mwc40xxa8State));
    Mwc40xxa8State_init(obj, intf.get_seed64());
    return (void *) obj;
}


MAKE_UINT32_PRNG("Mwc40xxa8", NULL)
