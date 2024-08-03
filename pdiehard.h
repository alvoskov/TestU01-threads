#ifndef __PDIEHARD_H
#define __PDIEHARD_H
#include "testu01_mt.h"

/**
 * @brief As close as possible to the DIEHARD test suite.
 */
class PseudoDiehardBattery : public TestsBattery
{
public:
    PseudoDiehardBattery(GenFactoryFunc genf);
};


#endif
