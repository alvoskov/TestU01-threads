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
#include <cstdint>

namespace testu01_threads {

constexpr unsigned int NTHREADS_DEFAULT{std::numeric_limits<unsigned int>::max()};

class TestDescr;
class BatteryIO;
class Unif01GenWrapper;

/**
 * @brief Conversion of unsigned (pseudorandom) 64-bit integer
 * to the double that belongs to the [0;1) interval.
 */
static inline double uint64_to_udouble(std::uint64_t val)
{
    union {
        std::uint64_t i;
        double f;
    } x;
    x.i = val;
    x.i = (x.i >> 12) | 0x3ff0000000000000;
    x.f -= 1.0;
    return x.f;
}


/**
 * @brief Conversion of unsigned (pseudorandom) 32-bit integer
 * to the double that belongs to the [0;1) interval.
 */
static inline double uint32_to_udouble(std::uint32_t val)
{
    constexpr double xdbl_norm = 1.0 / static_cast<double>(1ULL << 32);
    return static_cast<double>(val) * xdbl_norm;
}


/**
 * @brief Callback function that runs the test and saves its
 * result in BatteryIO class using the test description from
 * TestDescr.
 */
using TestCbFunc = std::function<void(const TestDescr&, BatteryIO&)>;

} // namespace testu01_threads

#endif // __TESTU01_BASICTYPES_H
