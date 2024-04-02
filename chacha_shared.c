/**
 * @brief KISS93 pseudorandom number generator. It passes SmallCrush
 * but fails the LinearComp (r = 29) test in the Crush battery.
 * https://datatracker.ietf.org/doc/html/rfc7539
 */
#include "testu01_mt_cintf.h"
#include <stdio.h>
#include <stdlib.h>

uint32_t global_seed;

typedef struct {
    uint32_t x[16]; /**< Working state */
    uint32_t out[16]; /**< Output state */
    size_t pos;
} ChaChaState;



static inline uint32_t rotl(uint32_t x, uint32_t r)
{
    return (x << r) | (x >> (32 - r));
}

static inline void qround(uint32_t *x, size_t ai, size_t bi, size_t ci, size_t di)
{
    x[ai] += x[bi]; x[di] ^= x[ai]; x[di] = rotl(x[di], 16);
    x[ci] += x[di]; x[bi] ^= x[ci]; x[di] = rotl(x[di], 12);
    x[ai] += x[bi]; x[di] ^= x[ai]; x[di] = rotl(x[di], 8);
    x[ci] += x[di]; x[bi] ^= x[ci]; x[di] = rotl(x[di], 7);
}

static void ChaCha20_inc_counter(ChaChaState *obj)
{
    obj->x[12]++;
    if (obj->x[12] == 0) obj->x[13]++;
    if (obj->x[13] == 0) obj->x[14]++;
    if (obj->x[14] == 0) obj->x[15]++;
}




void EXPORT ChaCha20_block(ChaChaState *obj)
{
    ChaCha20_inc_counter(obj);
    for (size_t k = 0; k < 16; k++) {
        obj->out[k] = obj->x[k];
    }

    for (size_t k = 0; k < 4; k++) {
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



static long unsigned int get_bits32(void *param, void *state)
{
    (void) param;
    ChaChaState *obj = (ChaChaState *) state;
    if (obj->pos >= 16) {
        ChaCha20_block(obj);
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
    /* Constants: the upper row of the matrix */
    obj->x[0] = 0x61707865; obj->x[1] = 0x3320646e;
    obj->x[2] = 0x79622d32; obj->x[3] = 0x6b206574;
    /* Rows 1-2: seed (key); low-grade LCG is suitable here */
    uint32_t lcg = global_seed;
    for (size_t k = 4; k <= 11; k++) {
        lcg = (69069 * lcg + 1);
        obj->x[k] = lcg;
    }
    /* Row 3: counter and nonce */
    obj->x[12] = 1; obj->x[13] = 0; obj->x[14] = 0; obj->x[15] = 0;
    /* Output state */
    for (size_t k = 0; k < 16; k++) {
        obj->out[k] = 0;
    }
    /* Output counter */
    obj->pos = 16;
    return (void *) obj;
}

static void delete_state(void *param, void *state)
{
    (void) param;
    free(state);
}

int EXPORT gen_initlib(uint64_t seed, void *data)
{
    global_seed = seed;
    (void) data;
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
