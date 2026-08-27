/**
 * @file testu01_plugin.cpp
 * @brief Wrappers for pseudorandom number generators from SmokeRand test suite.
 * source code.
 *
 * @copyright
 * (c) 2024-2026 Alexey L. Voskov, Lomonosov Moscow State University.
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


std::string UniformGeneratorPlugin::GetGeneratorName(const GeneratorInfo* gi)
{
    std::string name;
    if (gi->parent != nullptr) {
        name = std::string(gi->name) + ":" + gi->parent->name;
    } else {
        name = std::string(gi->name);
    }
    for (size_t i = 0; i < name.size(); i++) {
        if (name[i] == ':') {  // TestU01 uses ':' as a separator
            name[i] = '_';
        }
    }
    return name;
}


UniformGeneratorPlugin::UniformGeneratorPlugin(const GeneratorInfo* gi, const CallerAPI* intf)
: UniformGenerator{GetGeneratorName(gi)}
{
    obj.gi = gi;
    obj.state = gi->create(gi, intf);
    obj.intf = intf;
    if (obj.state == nullptr) {
        fprintf(stderr,
            "Cannot create an example of generator '%s' with parameter '%s'\n",
            gi->name, intf->get_param());
        exit(EXIT_FAILURE);
    }
}


double UniformGeneratorPlugin::GetU01()
{
    static constexpr double INV32{1.0 / (static_cast<std::uint64_t>(1) << 32)};
    const std::uint64_t u{obj.gi->get_bits(obj.state)};
    return static_cast<double>(u >> (obj.gi->nbits - 32)) * INV32;
}


std::uint32_t UniformGeneratorPlugin::GetBits32()
{
    const std::uint64_t u{obj.gi->get_bits(obj.state)};
    return static_cast<std::uint32_t>(u >> (obj.gi->nbits - 32));
}


UniformGeneratorPlugin::~UniformGeneratorPlugin()
{
    obj.gi->free(obj.state, obj.gi, obj.intf);
}
