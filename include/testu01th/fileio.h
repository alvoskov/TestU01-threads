/**
 * @file fileio.h
 * @brief Wrappers for pseudorandom sequences from stdin.
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
#ifndef __TESTU01_FILEIO_H
#define __TESTU01_FILEIO_H
#include "testu01_threads.h"
#include <cstdint>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>

namespace testu01_threads {


/**
 * @brief This pseudorandom number generator obtains numbers
 * from stdin (as uint32_t in binary format).
 */
class Stdin32Collector : public UniformGenerator
{
protected:
    static constexpr size_t buffer_size{2048};
    std::uint32_t buffer[buffer_size];
    size_t pos;

public:
    Stdin32Collector();
    void FillBuffer();
    double GetU01() override;
    std::uint32_t GetBits32() override;
};


class Stdin32ParallelCollector;

/**
 * @brief Essentially an output port for Stdin32ParallelCollector
 */
class Stdin32ParallelGenerator : public UniformGenerator
{
    Stdin32ParallelCollector& parent; ///< Parent collector (interface to stdin)
    const std::vector<uint32_t>& buffer; ///< Buffer from stdin
    size_t pos; ///< Current position in the buffer

public:
    Stdin32ParallelGenerator(Stdin32ParallelCollector& parent_,
        const std::vector<uint32_t>& buffer_);
    double GetU01() override;
    uint32_t GetBits32() override;
    void Invalidate() override;
};

/**
 * @brief Allows to share the same sample from stdin between different
 * threads. So different tests can process one sample from stdin simultaneously.
 */
class Stdin32ParallelCollector
{
    static constexpr size_t buffer_size{1UL << 16};
    std::vector<uint32_t> buffer;
    size_t pos;
    size_t ngenerators_all{0}; ///< Total number of allocated generators
    size_t ngenerators_waiting{0}; ///< Number of generator that are waiting for the buffer refresh
    size_t generation{0}; ///< Barrier generation
    std::mutex genfactory_mutex; ///< Controls ngenerators_all and ngenerators_waiting counters.
    std::mutex buffer_mutex; ///< Controls buffers/counters
    std::condition_variable buffer_cv;

public:
    Stdin32ParallelCollector();
    void FillBuffer(bool last_fill = false);
    GenFactoryFunc CreateGenFactoryFunc();
};

} // namespace testu01_threads

#endif
