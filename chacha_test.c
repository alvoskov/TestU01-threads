#include "chacha_shared.h"
#include <stdio.h>

void print_mat16(uint32_t *x)
{
    for (size_t i = 0; i < 16; i++) {
        printf("%10.8X ", x[i]);
        if ((i + 1) % 4 == 0)
            printf("\n");
    }
}

int main()
{
    uint32_t x_init[] = {
        0x03020100,  0x07060504,  0x0b0a0908,  0x0f0e0d0c,
        0x13121110,  0x17161514,  0x1b1a1918,  0x1f1e1d1c,
        0x00000001,  0x09000000,  0x4a000000,  0x00000000
    };

    uint32_t out_final[] = {
       0xe4e7f110,  0x15593bd1,  0x1fdd0f50,  0xc47120a3,
       0xc7f4d1c7,  0x0368c033,  0x9aaa2204,  0x4e6cd4c3,
       0x466482d2,  0x09aa9f07,  0x05d7c214,  0xa2028bd9,
       0xd19c12b5,  0xb94e16de,  0xe883d0cb,  0x4e3c50a2
    };

    ChaChaState obj;

    ChaCha_init(&obj, 20, x_init);
    for (size_t i = 0; i < 12; i++) {
        obj.x[i + 4] = x_init[i];
    }
    printf("Input:\n");
    print_mat16(obj.x);
    ChaCha_block(&obj);
    printf("Output (real):\n");
    print_mat16(obj.out);
    printf("Output (reference):\n");
    print_mat16(out_final);

    for (size_t i = 0; i < 16; i++) {
        if (out_final[i] != obj.out[i]) {
            printf("TEST FAILED!\n");
            return 1;
        }        
    }
    printf("Success.\n");
    return 0;
}