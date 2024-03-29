#include "testu01_mt_cintf.h"
#include "testu01_mt.h"
#include "smallcrush.h"
#include "crush.h"
#include "bigcrush.h"

static GenInfoC geninfo;

void set_generator(const GenInfoC *gi)
{
    geninfo = *gi;
}

int run_smallcrush()
{
    auto create_gen = [] () -> std::shared_ptr<UniformGenerator> {
        return std::shared_ptr<UniformGenerator>(new UniformGeneratorC(&geninfo));
    };
    mt_bat_SmallCrush(create_gen);
    return 1;
}

int run_crush()
{
    return 1;
}

int run_bigcrush()
{
    return 1;
}
