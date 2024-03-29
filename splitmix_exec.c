#include "testu01_mt_cintf.h"
#include "splitmix_gen.h"

int main()
{
    GenInfoC gi;
    splitmix_get_geninfo(&gi);
    set_generator(&gi);
    run_smallcrush();
    return 0;
}
