#include "testu01_mt_cintf.h"
#include "testu01_mt.h"
#include "smallcrush.h"
#include "crush.h"
#include "bigcrush.h"

static GenInfoC geninfo;

void GenInfoC_init(GenInfoC *obj)
{
    obj->name = "";
    obj->init_state = nullptr;
    obj->delete_state = nullptr;
    obj->get_u01 = nullptr;
    obj->get_bits32 = nullptr;
    obj->get_bits64 = nullptr;
}


void set_generator(const GenInfoC *gi)
{
    geninfo = *gi;
}

int run_smallcrush()
{
    SmallCrushBattery bat([] () -> std::shared_ptr<UniformGenerator> {
        return std::shared_ptr<UniformGenerator>(new UniformGeneratorC(&geninfo));
    });
    bat.Run();
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
