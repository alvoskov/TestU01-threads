/**
 * @brief Test for ChaCha pseudorandom number generator. 
 * @details If compares the ChaCha20 PRNG output with reference values
 * from the RFC 7359 specification.
 *
 * References:
 * 1. RFC 7539. ChaCha20 and Poly1305 for IETF Protocols
 *    https://datatracker.ietf.org/doc/html/rfc7539
 *
 * @copyright (c) 2024 Alexey L. Voskov, Lomonosov Moscow State University.
 * alvoskov@gmail.com
 *
 * All rights reserved.
 *
 * This software is provided under the Apache 2 License.
 *
 * In scientific publications which used this software, a reference to it
 * would be appreciated.
 */
#include "chacha_avx_shared.h"
#include <stdio.h>

/**
 * @brief Print the 4x8 matrix of uint32_t from the ChaCha PRNG state.
 * @param x Pointer to the matrix (C-style)
 */
static void print_mat32(uint32_t *x)
{
    for (size_t i = 0; i < 32; i++) {
        printf("%10.8X ", x[i]);
        if ((i + 1) % 8 == 0)
            printf("\n");
    }
}


/**
 * @brief Print the 4x4 matrix of uint32_t from the ChaCha PRNG state.
 * @param x Pointer to the matrix (C-style)
 */
void print_mat16(uint32_t *x)
{
    for (size_t i = 0; i < 16; i++) {
        printf("%10.8X ", x[i]);
        if ((i + 1) % 4 == 0)
            printf("\n");
    }
}


/**
 * @brief Program entry point.
 */
int main()
{
    /* Input values */
    uint32_t x_init[] = {
        0x03020100,  0x07060504,  0x0b0a0908,  0x0f0e0d0c,
        0x13121110,  0x17161514,  0x1b1a1918,  0x1f1e1d1c,
        0x00000001,  0x09000000,  0x4a000000,  0x00000000
    };

    /* Refernce values from RFC 7359 */
    uint32_t out_final[] = {
       0xe4e7f110,  0x15593bd1,  0x1fdd0f50,  0xc47120a3,
       0xc7f4d1c7,  0x0368c033,  0x9aaa2204,  0x4e6cd4c3,
       0x466482d2,  0x09aa9f07,  0x05d7c214,  0xa2028bd9,
       0xd19c12b5,  0xb94e16de,  0xe883d0cb,  0x4e3c50a2
    };

    size_t mat32_map[] = {
         0, 1, 2, 3,  0, 1, 2, 3,
         4, 5, 6, 7,  4, 5, 6, 7,
         8, 9,10,11,  8, 9,10,11,
        12,13,14,15, 12,13,14,15
    };

    ChaChaState obj;

    ChaCha_init(&obj, 20, x_init);
    memcpy(obj.x + 8, x_init, 4 * sizeof(uint32_t)); // Row 2
    memcpy(obj.x + 12, x_init, 4 * sizeof(uint32_t));

    memcpy(obj.x + 16, x_init + 4, 4 * sizeof(uint32_t)); // Row 3
    memcpy(obj.x + 20, x_init + 4, 4 * sizeof(uint32_t));

    memcpy(obj.x + 24, x_init + 8, 4 * sizeof(uint32_t)); // Row 4
    memcpy(obj.x + 28, x_init + 8, 4 * sizeof(uint32_t));
    printf("Input:\n"); print_mat32(obj.x);
    ChaCha_block(&obj);
    printf("Output (real):\n"); print_mat32(obj.out);
    printf("Output (reference):\n"); print_mat16(out_final);
    for (size_t i = 0; i < 32; i++) {
        if (out_final[mat32_map[i]] != obj.out[i]) {
            printf("TEST FAILED!\n");
            return 1;
        }        
    }
    printf("Success.\n");
    return 0;
}
