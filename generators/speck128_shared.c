/**
 * @file speck128_shared.c
 * @brief 
 *
 * References:
 * 1. https://ia.cr/2013/404
 * 2. https://nsacyber.github.io/simon-speck/implementations/ImplementationGuide1.1.pdf
 * 3. https://mcnp.lanl.gov/pdf_files/TechReport_2023_LANL_LA-UR-23-25111Rev.1_Josey.pdf
 *
 * Rounds:
 * - 8 rounds: passes SmallCrush, fails PractRand at 8 GiB
 * - 9 rounds: passes Crush, fails PractRand at ???
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



static inline uint64_t rotl(uint64_t x, uint64_t r)
{
    return (x << r) | (x >> (64 - r));
}

static inline uint64_t rotr(uint64_t x, uint64_t l)
{
    return (x << (64 - l)) | (x >> l);
}

/**
 * @brief Speck128 state.
 */
typedef struct {
    uint64_t ctr[2]; ///< Counter
    uint64_t out[2]; ///< Output buffer
    uint64_t keys[32]; ///< Round keys
    unsigned int pos;
    Interleaved32Buffer i32buf;
} Speck128State;

#define R(x,y,k) (x=rotr(x,8), x+=y, x^=k, y=rotl(y,3), y^=x)


static void Speck128State_init(Speck128State *obj, const uint64_t *key)
{
    obj->ctr[0] = 0;
    obj->ctr[1] = 0;
    if (key == NULL) {
        obj->keys[0] = intf.get_seed64();
        obj->keys[1] = intf.get_seed64();
    } else {
        obj->keys[0] = key[0];
        obj->keys[1] = key[1];
    }
    uint64_t a = obj->keys[0], b = obj->keys[1];
    for (size_t i = 0; i < 31; i++) {
        //intf.printf("%llX\n", obj->keys[i]);
        R(b, a, i);
        obj->keys[i + 1] = a;
    }    
    obj->pos = 2;
    Interleaved32Buffer_init(&obj->i32buf);
}

static inline void Speck128State_block(Speck128State *obj)
{
    obj->out[0] = obj->ctr[0];
    obj->out[1] = obj->ctr[1];
    for (size_t i = 0; i < 32; i++) {
        R(obj->out[1], obj->out[0], obj->keys[i]);
    }
}

static void *init_state(void)
{
    Speck128State *obj = intf.malloc(sizeof(Speck128State));
    Speck128State_init(obj, NULL);
    return (void *) obj;
}


/**
 * @brief Speck128/128 implementation.
 */
static inline uint64_t get_bits64_raw(void *param, void *state)
{
    Speck128State *obj = state;
    (void) param;
    if (obj->pos == 2) {
        Speck128State_block(obj);
        if (++obj->ctr[0] == 0) obj->ctr[1]++;
        obj->pos = 0;
    }
    return obj->out[obj->pos++];
}




int run_self_test(void)
{
    const uint64_t key[] = {0x0706050403020100, 0x0f0e0d0c0b0a0908};
    const uint64_t ctr[] = {0x7469206564616d20, 0x6c61766975716520};
    const uint64_t out[] = {0x7860fedf5c570d18, 0xa65d985179783265};
    Speck128State *obj = intf.malloc(sizeof(Speck128State));
    Speck128State_init(obj, key);
    obj->ctr[0] = ctr[0]; obj->ctr[1] = ctr[1];
    Speck128State_block(obj);
    intf.printf("Output:    0x%16llX 0x%16llX\n", obj->out[0], obj->out[1]);
    intf.printf("Reference: 0x%16llX 0x%16llX\n", out[0], out[1]);
    intf.free(obj);
    return obj->out[0] == out[0] && obj->out[1] == out[1];
    return 1;
}


MAKE_UINT64_INTERLEAVED32_PRNG("Speck128", Speck128State, run_self_test)
