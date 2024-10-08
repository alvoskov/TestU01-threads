#include "testu01th/cinterface.h"
#include "testu01th/splitmix_gen.h"
#include <stdio.h>

static GenInfoC gi;

int EXPORT gen_initlib(CallerAPI *intf)
{
    (void) intf;
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
