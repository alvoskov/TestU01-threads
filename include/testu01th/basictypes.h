#ifndef __TESTU01_BASICTYPES_H
#define __TESTU01_BASICTYPES_H
#include <functional>
#include <limits>

namespace testu01_threads {

static constexpr unsigned int NTHREADS_DEFAULT{std::numeric_limits<unsigned int>::max()};

class TestDescr;
class BatteryIO;
class Unif01GenWrapper;

/**
 * @brief Callback function that runs the test and saves its
 * result in BatteryIO class using the test description from
 * TestDescr.
 */
typedef std::function<void(const TestDescr&, BatteryIO&)> TestCbFunc;

} // namespace testu01_threads

#endif // __TESTU01_BASICTYPES_H
