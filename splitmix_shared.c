#include "testu01_mt_cintf.h"
#include "splitmix_gen.h"
#include <stdio.h>

static GenInfoC gi;

int EXPORT gen_initlib()
{
    printf("===== gen_initlib =====\n");
    splitmix_get_geninfo(&gi);
    return 1;
}

int EXPORT gen_closelib()
{
    printf("===== gen_closelib =====\n");
    return 1;
}

int EXPORT gen_getinfo(GenInfoC *gi_out)
{
    printf("===== gen_getinfo =====\n");
    *gi_out = gi;
    return 1;
}
