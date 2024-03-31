#include "testu01_mt_cintf.h"
#include "splitmix_gen.h"
#include <stdio.h>

static GenInfoC gi;

int EXPORT gen_initlib(uint64_t seed, void *data)
{
    (void) seed;
    (void) data;
    splitmix_get_geninfo(&gi);
    return 1;
}

int EXPORT gen_closelib()
{
    return 1;
}

int EXPORT gen_getinfo(GenInfoC *gi_out)
{
    *gi_out = gi;
    return 1;
}
