#ifndef __TESTU01_MT_C_H
#define __TESTU01_MT_C_H
#include "testu01_mt.h"

namespace testu01_threads {

/**
 * @brief A variant of UniformGenerator that is designed as an interface
 * for C program.
 * @details The next functions should be supplied by the C module:
 *
 * - `double get_u01(void *param, void *state)` - that returns uniformly
 *    distributed pseudorandom numbers from the [0;1) interval.
 * - `unsigned long get_bits32(void *param, void *state)` - that returns
 *    uniformly distributed 32-bit unsigned pseudorandom numbers.
 * - `void gen_delete(void *param, void *state)` - destroys the generator.
 *
 * The module also can supply optional functions that are useful
 * for PractRand and for performance measurements:
 *
 * - `uint64_t get_bits64(void *param, void *state)` - returns uniformly
 *    distributed 64-bit unsigned pseudorandom numbers.
 * - `void get_array32(void *param, void *state, uint32_t *out, size_t len)`
 *   Returns array of 32-bit unsigned integer pseudorandom numbers.
 * - `void get_array64(void *param, void *state, uint64_t *out, size_t len)`
 *   Returns array of 64-bit unsigned integer pseudorandom numbers.
 */
class UniformGeneratorC : public UniformGenerator
{
    static void WrExternGen(void *junk2) { (void) junk2; }
    std::string name;
    const GenInfoC gen_module;
    UniformGeneratorC(const UniformGeneratorC &obj) = delete;
    UniformGeneratorC &operator=(const UniformGeneratorC &obj) = delete;
    
public:
    UniformGeneratorC(const GenInfoC *gi)
    : UniformGenerator(""), gen_module(*gi)
    {
        this->name = std::string(gi->name);
        gen.state = gi->init_state();
        gen.param = nullptr;
        gen.Write = WrExternGen;
        gen.GetU01 = gi->get_u01;
        gen.GetBits = gi->get_bits32;
        gen.name = const_cast<char *>(name.c_str());
    }

    unif01_Gen *GetPtr() const { return const_cast<unif01_Gen *>(&gen); }
    const std::string &GetName() { return name; }
    double GetU01() override { return gen.GetU01(gen.param, gen.state); }
    uint32_t GetBits32() override { return gen.GetBits(gen.param, gen.state); }
    uint64_t GetBits64() override
    {
        return gen_module.get_bits64(gen.param, gen.state);
    }
    void GetArray32(uint32_t *out, size_t len) override
    {
        return gen_module.get_array32(gen.param, gen.state, out, len);
    }
    void GetArray64(uint64_t *out, size_t len) override
    {
        return gen_module.get_array64(gen.param, gen.state, out, len);
    }
    uint32_t GetSum32(size_t len) override
    {
        return gen_module.get_sum32(gen.param, gen.state, len);
    }
    uint64_t GetSum64(size_t len) override
    {
        return gen_module.get_sum64(gen.param, gen.state, len);
    }
    virtual ~UniformGeneratorC()
    {
        gen_module.delete_state(gen.param, gen.state);
    }
};
}

#endif
