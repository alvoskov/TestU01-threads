#include "testu01_mt_cintf.h"
#include "testu01_mt.h"
#include "smallcrush.h"
#include "crush.h"
#include "bigcrush.h"

using namespace testu01_threads;

/**
 * @brief Initialize the structure with information about PRNG
 * with empty values (pointers to empty strings and nullptrs)
 */
void GenInfoC_init(GenInfoC *obj)
{
    obj->name = "";
    obj->options = "";
    obj->init_state = nullptr;
    obj->delete_state = nullptr;
    obj->get_u01 = nullptr;
    obj->get_bits32 = nullptr;
    obj->get_bits64 = nullptr;
    obj->get_array32 = nullptr;
    obj->get_array64 = nullptr;
    obj->get_sum32 = nullptr;
    obj->get_sum64 = nullptr;
    obj->run_self_test = nullptr;
}


/*
void set_generator(const GenInfoC *gi)
{
    geninfo = *gi;
}
*/

/**
 * @brief Runs multithreaded SmallCrush for the given PRNG.
 * @param gi Pseudorandom number generator for testing.
 */
int run_smallcrush(const GenInfoC *gi)
{
    SmallCrushBattery bat([gi] () -> std::shared_ptr<UniformGenerator> {
        return std::shared_ptr<UniformGenerator>(new UniformGeneratorC(gi));
    });
    bat.Run();
    return 1;
}

/**
 * @brief Runs multithreaded Crush for the given PRNG.
 * @param gi Pseudorandom number generator for testing.
 */
int run_crush(const GenInfoC *gi)
{
    CrushBattery bat([gi] () -> std::shared_ptr<UniformGenerator> {
        return std::shared_ptr<UniformGenerator>(new UniformGeneratorC(gi));
    });
    bat.Run();
    return 1;
}

/**
 * @brief Runs multithreaded BigCrush for the given PRNG.
 * @param gi Pseudorandom number generator for testing.
 */
int run_bigcrush(const GenInfoC *gi)
{
    BigCrushBattery bat([gi] () -> std::shared_ptr<UniformGenerator> {
        return std::shared_ptr<UniformGenerator>(new UniformGeneratorC(gi));
    });
    bat.Run();
    return 1;
}
