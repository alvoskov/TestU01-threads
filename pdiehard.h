#ifndef __PDIEHARD_H
#define __PDIEHARD_H
#include "testu01_mt.h"

namespace testu01_threads {

/**
 * @brief As close as possible to the DIEHARD test suite.
 */
class PseudoDiehardBattery : public TestsBattery
{
public:
    PseudoDiehardBattery(GenFactoryFunc genf);
};

} // namespace testu01_threads

#endif
