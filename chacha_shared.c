#include <stdio.h>
#include <stdlib.h>
#include "chacha_shared.h"

static CallerAPI intf;

static inline uint32_t rotl(uint32_t x, uint32_t r)
{
    return (x << r) | (x >> (32 - r));
}

static inline void qround(uint32_t *x, size_t ai, size_t bi, size_t ci, size_t di)
{
    x[ai] += x[bi]; x[di] ^= x[ai]; x[di] = rotl(x[di], 16);
    x[ci] += x[di]; x[bi] ^= x[ci]; x[bi] = rotl(x[bi], 12);
    x[ai] += x[bi]; x[di] ^= x[ai]; x[di] = rotl(x[di], 8);
    x[ci] += x[di]; x[bi] ^= x[ci]; x[bi] = rotl(x[bi], 7);
}

static void ChaCha_inc_counter(ChaChaState *obj)
{
    obj->x[12]++;
    if (obj->x[12] == 0) obj->x[13]++;
    if (obj->x[13] == 0) obj->x[14]++;
    if (obj->x[14] == 0) obj->x[15]++;
}



/**
 * @details
 * Rounds:
 *
 * | x . . . |    | . x . . |    | . . x . |    | . . . x |
 * | x . . . | => | . x . . | => | . . x . | => | . . . x |
 * | x . . . |    | . x . . |    | . . x . |    | . . . x |
 * | x . . . |    | . x . . |    | . . x . |    | . . . x |
 * 
 * | x . . . |    | . x . . |    | . . x . |    | . . . x |
 * | . x . . | => | . . x . | => | . . . x | => | x . . . |
 * | . . x . |    | . . . x |    | x . . . |    | . x . . |
 * | . . . x |    | x . . . |    | . x . . |    | . . x . |
 */
void EXPORT ChaCha_block(ChaChaState *obj)
{
    for (size_t k = 0; k < 16; k++) {
        obj->out[k] = obj->x[k];
    }

    for (size_t k = 0; k < obj->ncycles; k++) {
        qround(obj->out, 0, 4, 8,12);
        qround(obj->out, 1, 5, 9,13);
        qround(obj->out, 2, 6,10,14);
        qround(obj->out, 3, 7,11,15);
        qround(obj->out, 0, 5,10,15);
        qround(obj->out, 1, 6,11,12);
        qround(obj->out, 2, 7, 8,13);
        qround(obj->out, 3, 4, 9,14);
    }

    for (size_t i = 0; i < 16; i++)
        obj->out[i] += obj->x[i];
}

void EXPORT ChaCha_init(ChaChaState *obj, size_t nrounds, const uint32_t *seed)
{
    /* Constants: the upper row of the matrix */
    obj->x[0] = 0x61707865; obj->x[1] = 0x3320646e;
    obj->x[2] = 0x79622d32; obj->x[3] = 0x6b206574;
    /* Rows 1-2: seed (key) */
    for (size_t i = 0; i < 8; i++) {
        obj->x[i + 4] = seed[i];
    }
    /* Row 3: counter and nonce */
    obj->x[12] = 1; obj->x[13] = 0; obj->x[14] = 0; obj->x[15] = 0;
    /* Number of rounds => Number of cycles */
    obj->ncycles = nrounds / 2;
    /* Output state */
    for (size_t i = 0; i < 16; i++) {
        obj->out[i] = 0;
    }
    /* Output counter */
    obj->pos = 16;
}



static long unsigned int get_bits32(void *param, void *state)
{
    (void) param;
    ChaChaState *obj = (ChaChaState *) state;
    if (obj->pos >= 16) {
        ChaCha_inc_counter(obj);
        ChaCha_block(obj);
        obj->pos = 0;
    }
    return obj->out[obj->pos++];
}

static double get_u01(void *param, void *state)
{
    static const double INV32 = 2.3283064365386963E-10;
    return get_bits32(param, state) * INV32;
}


static void *init_state()
{
    ChaChaState *obj = (ChaChaState *) malloc(sizeof(ChaChaState));
    uint32_t seeds[8];
    for (size_t i = 0; i < 4; i++) {
        uint64_t s = intf.get_seed64();
        seeds[2*i] = s & 0xFFFFFFF;
        seeds[2*i + 1] = s >> 32;
    }

    ChaCha_init(obj, 8, seeds);
    return (void *) obj;
}

static void delete_state(void *param, void *state)
{
    (void) param;
    free(state);
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
    static const char name[] = "ChaCha8";
    gi->name = name;
    gi->init_state = init_state;
    gi->delete_state = delete_state;
    gi->get_u01 = get_u01;
    gi->get_bits32 = get_bits32;
    return 1;
}
