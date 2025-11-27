/**
 * @file testu01_plugin.h
 * @brief Wrappers for pseudorandom number generators from SmokeRand test suite.
 * source code.
 *
 * @copyright
 * (c) 2024-2025 Alexey L. Voskov, Lomonosov Moscow State University.
 * alvoskov@gmail.com
 *
 * All rights reserved.
 *
 * This software is provided under the Apache 2 License.
 *
 * In scientific publications which used this software, a reference to it
 * would be appreciated.
 */
#include "testu01th/testu01_plugin.h"
using namespace testu01_threads;

///////////////////////////////////////////////////////
///// UniformGeneratorPlugin class implementation /////
///////////////////////////////////////////////////////

UniformGeneratorPlugin::UniformGeneratorPlugin(const GeneratorInfo* gi, const CallerAPI* intf)
: UniformGenerator(gi->name)
{
    obj.gi = gi;
    obj.state = gi->create(gi, intf);
    obj.intf = intf;
    if (obj.state == NULL) {
        fprintf(stderr,
            "Cannot create an example of generator '%s' with parameter '%s'\n",
            gi->name, intf->get_param());
        exit(EXIT_FAILURE);
    }
}


double UniformGeneratorPlugin::GetU01()
{
    static constexpr double INV32 = 1.0 / (static_cast<uint64_t>(1) << 32);
    const uint64_t u = obj.gi->get_bits(obj.state);
    return static_cast<double>(u >> (obj.gi->nbits - 32)) * INV32;
}

uint32_t UniformGeneratorPlugin::GetBits32()
{
    const uint64_t u = obj.gi->get_bits(obj.state);
    return static_cast<uint32_t>(u >> (obj.gi->nbits - 32));
}

UniformGeneratorPlugin::~UniformGeneratorPlugin()
{
    obj.gi->free(obj.state, obj.gi, obj.intf);
}
