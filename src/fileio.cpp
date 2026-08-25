/**
 * @file fileio.cpp
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
#include "testu01th/fileio.h"
#include <iostream>

namespace testu01_threads {

/////////////////////////////////////////////////
///// Stdin32Collector class implementation /////
/////////////////////////////////////////////////

Stdin32Collector::Stdin32Collector()
: UniformGenerator("stdin32")
{
    set_bin_stdin();
    pos = buffer_size;
}

void Stdin32Collector::FillBuffer()
{
    const auto nvals = std::fread(buffer, sizeof(std::uint32_t), buffer_size, stdin);
    if (nvals != buffer_size) {
        std::cerr << "Stdin32Collector::FillBuffer: cannot read from stdin; "
                  << nvals << " values were received" << std::endl;
        exit(EXIT_FAILURE);
    }
    pos = 0;
}

double Stdin32Collector::GetU01()
{
    return static_cast<double>(GetBits32()) / static_cast<double>(1ULL << 32);
}


std::uint32_t Stdin32Collector::GetBits32()
{
    if (pos == buffer_size) {
        FillBuffer();
    }
    return buffer[pos++];
}


/////////////////////////////////////////////////////////
///// Stdin32ParallelGenerator class implementation /////
/////////////////////////////////////////////////////////

Stdin32ParallelGenerator::Stdin32ParallelGenerator(
    Stdin32ParallelCollector& parent_,
    const std::vector<uint32_t>& buffer_)
    : UniformGenerator("stdin32par"),
      parent(parent_), buffer(buffer_), pos(buffer_.size())
{
}


double Stdin32ParallelGenerator::GetU01()
{
    return static_cast<double>(GetBits32()) / static_cast<double>(1ULL << 32);
}

std::uint32_t Stdin32ParallelGenerator::GetBits32()
{
    if (pos == buffer.size()) {
        parent.FillBuffer();
        pos = 0;
    }
    return buffer[pos++];
}

void Stdin32ParallelGenerator::Invalidate()
{
    parent.FillBuffer(true);
}



/////////////////////////////////////////////////////////
///// Stdin32ParallelCollector class implementation /////
/////////////////////////////////////////////////////////

Stdin32ParallelCollector::Stdin32ParallelCollector()
: buffer(buffer_size), pos(buffer_size)
{
    set_bin_stdin();
}

void Stdin32ParallelCollector::FillBuffer(bool last_fill)
{
    std::unique_lock<std::mutex> lock(buffer_mutex);
    const size_t gen = generation;
    if (last_fill) {
        if (ngenerators_all > 0) {
            ngenerators_all--;
        }
    }
    if (ngenerators_waiting == 0 || --ngenerators_waiting == 0) {
        size_t nvals{0};
        for (size_t i = 0; i < buffer_size >> 10; i++) {
            nvals += std::fread(buffer.data() + (i << 10), sizeof(std::uint32_t), 1UL << 10, stdin);
        }
        if (nvals != buffer_size) {
            std::cerr << "Stdin32Collector::FillBuffer: cannot read from stdin; "
                      << nvals << " values were received" << std::endl;
            exit(EXIT_FAILURE);
        }
        generation++;
        ngenerators_waiting = ngenerators_all;
        buffer_cv.notify_all();
    } else {
        buffer_cv.wait(lock, [this, gen]() { 
            return gen != generation;
        });
    }
}


GenFactoryFunc Stdin32ParallelCollector::CreateGenFactoryFunc()
{
    auto f = [&] () {
        auto gen = std::make_shared<Stdin32ParallelGenerator>(*this, buffer);
        ngenerators_all++;
        ngenerators_waiting++;
        return gen;
    };
    std::this_thread::sleep_for(std::chrono::seconds(1));
    return f;
}

} // namespace testu01_threads
