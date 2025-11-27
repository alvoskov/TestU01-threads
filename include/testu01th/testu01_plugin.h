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
#ifndef __TESTU01_PLUGIN_H
#define __TESTU01_PLUGIN_H

#include "testu01_mt.h"

extern "C" {
#include "smokerand/apidefs.h"
#include "smokerand/core.h"
}


namespace testu01_threads {

class UniformGeneratorPlugin : public UniformGenerator
{
    GeneratorState obj;

public:
    UniformGeneratorPlugin(const GeneratorInfo* gi, const CallerAPI* intf);
    ~UniformGeneratorPlugin() override;

    virtual double GetU01() override;
    virtual uint32_t GetBits32() override;
};


} // namespace testu01_threads

#endif
