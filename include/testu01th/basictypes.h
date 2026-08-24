/**
 * @file basictypes.h
 * @brief Some basic types used by different parts of TestU01-threads.
 * Contains some forward declarations of classes/structs.
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
#ifndef __TESTU01_BASICTYPES_H
#define __TESTU01_BASICTYPES_H
#include <functional>
#include <limits>

namespace testu01_threads {

constexpr unsigned int NTHREADS_DEFAULT{std::numeric_limits<unsigned int>::max()};

class TestDescr;
class BatteryIO;
class Unif01GenWrapper;

/**
 * @brief Callback function that runs the test and saves its
 * result in BatteryIO class using the test description from
 * TestDescr.
 */
using TestCbFunc = std::function<void(const TestDescr&, BatteryIO&)>;

} // namespace testu01_threads

#endif // __TESTU01_BASICTYPES_H
