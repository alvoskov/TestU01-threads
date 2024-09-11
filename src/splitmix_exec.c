#include "testu01th/cinterface.h"
#include "testu01th/splitmix_gen.h"

int main()
{
    GenInfoC gi;
    splitmix_get_geninfo(&gi);
    run_smallcrush(&gi);
    return 0;
}
