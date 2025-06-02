/**
 * @file ziff98.c 
 * @brief An implementation of Ziff98 LFSR.
 * @details Has much better quality than R250 or R1279 but still fails the
 * linear complexity test for all bits.
 *
 * Uses the next recurrent relationship:
 *
 * \f[
 * x_{n} = x_{n - 471} \oplus x_{n - 1586} \oplus x_{n - 6988} \oplus x_{n - 9689)
 * \f]
 *
 * where \f$ x_i \f$ are 32-bit (or n-bit) words. It was proven by the PRNG
 * author [1] that it is equivalent to the 2-tap PRNG with decimation. This
 * technique was used in RANLUX for SWB generator and in TAOCP2 for LFIB
 * generator.
 *
 * References:
 *
 * 1. Ziff R.M. Four-tap shift-register-sequence random-number generators
 *    // Computers in Physics. 1998. V. 12. N 4. P.385-392.
 *    https://doi.org/10.1063/1.168692
 *
 * @copyright
 * (c) 2025 Alexey L. Voskov, Lomonosov Moscow State University.
 * alvoskov@gmail.com
 */
#include "testu01th/cinterface.h"

PRNG_CMODULE_PROLOG

enum {
    LF8_BUFSIZE = 64,
    LF8_MASK = 0x3F
};

typedef struct {
    uint8_t x[LF8_BUFSIZE];
    uint8_t w;
    uint8_t pos;
} Alfib8State;

static inline uint64_t get_bits8(Alfib8State *obj)
{
    uint8_t *x = obj->x;
    obj->pos++;
    obj->w += 151;
    uint8_t u = x[(obj->pos - 55) & LF8_MASK] + x[(obj->pos - 24) & LF8_MASK];
    x[obj->pos & LF8_MASK] = u;
    u = (u ^ (u >> 5)) + (obj->w ^ (obj->w >> 5));
    u = u + (u << 1) + (u << 3) + (u << 6);
    return u;
}


static inline unsigned long get_bits32_raw(void *param, void *state)
{
    union {
        uint8_t  u8[4];
        uint32_t u32;
    } out;
    for (int i = 0; i < 4; i++) {
        out.u8[i] = get_bits8(state);
    }
    (void) param;
    return out.u32;
}

static void Alfib8State_init(Alfib8State *obj, uint32_t seed)
{
    uint8_t x = seed & 0xFF;
    uint8_t a = (seed >> 8) & 0xFF;
    uint8_t b = (seed >> 16) & 0xFF;
    uint8_t c = (seed >> 24) & 0xFF;
    for (int i = 0; i < 32; i++) {
        a ^= c ^ (x += 151);
        b += a;
        c = (c + ((b << 7) | (b >> 1))) ^ a;
    }
    for (int i = 0; i < LF8_BUFSIZE; i++) {
        a ^= c ^ (x += 151);
        b += a;
        c = (c + ((b << 7) | (b >> 1))) ^ a;
        obj->x[i] = c ^ b;
    }
    obj->w = 0;
    obj->pos = 0;
}

static void *init_state(void)
{
    Alfib8State *obj = intf.malloc(sizeof(Alfib8State));
    Alfib8State_init(obj, intf.get_seed64());
    return obj;
}

MAKE_UINT32_PRNG("Alfib8", NULL)
