/**
 * @file testu01_plugin.h
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
#ifndef __TESTU01_PLUGIN_H
#define __TESTU01_PLUGIN_H

#include "testu01_mt.h"
#include "smokerand/plugindefs.h"

namespace testu01_threads {

/**
 * @brief Wraps PRNGs from SmokeRand plugins to make them accessible by the
 * TestU01-threads library. Automatically takes higher bits from 64-bit
 * generators outputs.
 */
class UniformGeneratorPlugin : public UniformGenerator
{
    GeneratorState obj;
    static std::string GetGeneratorName(const GeneratorInfo* gi);

public:
    UniformGeneratorPlugin(const GeneratorInfo* gi, const CallerAPI* intf);
    ~UniformGeneratorPlugin() override;

    virtual double GetU01() override;
    virtual std::uint32_t GetBits32() override;
};


} // namespace testu01_threads

#endif // __TESTU01_PLUGIN_H
