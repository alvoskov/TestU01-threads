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
    UniformGeneratorPlugin(const GeneratorInfo *gi, const CallerAPI *intf);
    ~UniformGeneratorPlugin() override;

    virtual double GetU01() override;
    virtual uint32_t GetBits32() override;
};


} // namespace testu01_threads

#endif
