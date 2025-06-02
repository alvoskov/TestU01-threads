/**
 * @file mularx64_u32.c
 * @brief A simple counter-based generator that passes `full` battery and
 * 64-bit birthday paradox test.
 *
 * References:
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
 * @brief SplitMix PRNG state.
 */
typedef struct {
    union {
        uint32_t u32[2];
        uint64_t u64;
    } ctr;
    union {
        uint32_t u32[2];
        uint64_t u64;
    } out;
    int pos;
} Mularx64x32State;


static inline void mulbox64(uint32_t *v, int i, int j, uint32_t a, int r1, int r2)
{
    uint64_t mul = ((uint64_t) a) * (v[i] ^ v[j]);
    v[i] = (uint32_t) mul;
    v[j] ^= mul >> 32;
    //v[j] = v[j] + rotl32(v[i], r1);
    //v[i] = v[i] + rotl32(v[j], r2);

    // Behaves slightly better in PractRand 0.94 (doesn't fail the gap test)
    // but requires re-optimization of constants.
    v[i] = v[i] + rotl32(v[j], r1);
    v[j] = v[j] + rotl32(v[i], r2);    
}

static inline unsigned long get_bits32_raw(void *param, void *state)
{
    Mularx64x32State *obj = state;
    (void) param;
    if (obj->pos == 2) {
        obj->pos = 0;
        obj->ctr.u64++;
        obj->out.u64 = obj->ctr.u64;

        mulbox64(obj->out.u32, 0, 1, 0xD7474D0B, 30, 6);
        mulbox64(obj->out.u32, 0, 1, 0xE293A7BD, 26, 23);
    }

    return obj->out.u32[obj->pos++];
}


static void *init_state()
{
    Mularx64x32State *obj = intf.malloc(sizeof(Mularx64x32State));
    obj->ctr.u64 = intf.get_seed64();
    obj->out.u64 = 0;
    obj->pos = 2;
    return obj;
}

MAKE_UINT32_PRNG("Mularx64_u32", NULL)
