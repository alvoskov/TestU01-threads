#include <random>

#include "testu01_mt_cintf.h"

PRNG_CMODULE_PROLOG


static long unsigned int get_bits32(void *param, void *state)
{
    std::mt19937 *obj = (std::mt19937 *) state;
    (void) param;
    return (*obj)();
}


static double get_u01(void *param, void *state)
{
    static constexpr double INV32 = 1.0 / (static_cast<uint64_t>(1) << 32);
    (void) param;
    std::mt19937 *obj = (std::mt19937 *) state;
    return (*obj)() * INV32;
}


static void *init_state()
{
    std::mt19937 *obj = new std::mt19937();
    return (void *) obj;
}


static void delete_state(void *param, void *state)
{
    std::mt19937 *obj = (std::mt19937 *) state;
    (void) param;
    delete obj;
}


int EXPORT gen_getinfo(GenInfoC *gi)
{
    static const char name[] = "mt19937";
    gi->name = name;
    gi->init_state = init_state;
    gi->delete_state = delete_state;
    gi->get_u01 = get_u01;
    gi->get_bits32 = get_bits32;
    return 1;
}
