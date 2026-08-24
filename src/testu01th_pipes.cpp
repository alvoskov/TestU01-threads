/**
 * @file testu01th_pipes.cpp
 * @brief Reads pseudorandom numbers from stdin as uint32_t in binary form
 * and sends it to one-threaded version of TestU01.
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
#include "testu01_threads.h"
#include <stdio.h>
#include <stdint.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>

using namespace testu01_threads;

/**
 * @brief This pseudorandom number generator obtains numbers
 * from stdin (as uint32_t in binary format).
 */
class Stdin32Collector : public UniformGenerator
{
protected:
    static constexpr size_t buffer_size{2048};
    uint32_t buffer[buffer_size];
    size_t pos;

public:
    Stdin32Collector() : UniformGenerator("stdin32")
    {
        set_bin_stdin();
        pos = buffer_size;
    }

    void FillBuffer()
    {
        const auto nvals = std::fread(buffer, sizeof(uint32_t), buffer_size, stdin);
        if (nvals != buffer_size) {
            std::cerr << "Stdin32Collector::FillBuffer: cannot read from stdin; "
                      << nvals << " values were received" << std::endl;
            exit(EXIT_FAILURE);
        }
        pos = 0;
    }

    double GetU01() override
    {
        return static_cast<double>(GetBits32()) / static_cast<double>(1ULL << 32);
    }

    uint32_t GetBits32() override
    {
        if (pos == buffer_size) {
            FillBuffer();
        }
        return buffer[pos++];
    }
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
};

/**
 * @brief Allows to share the same sample from stdin between different
 * threads. So different tests can process one sample from stdin simultaneously.
 */
class Stdin32ParallelCollector
{
    static constexpr size_t buffer_size{2048};
    std::vector<uint32_t> buffer;
    size_t pos;
    size_t ngenerators_all{0}; ///< Total number of allocated generators
    size_t ngenerators_waiting{0}; ///< Number of generator that are waiting for the buffer refresh
    std::mutex buffer_mutex; ///< Controls buffers/counters
    std::condition_variable buffer_cv;

public:
    Stdin32ParallelCollector();
    void FillBuffer();
    GenFactoryFunc CreateGenFactoryFunc();
};


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

uint32_t Stdin32ParallelGenerator::GetBits32()
{
    if (pos == buffer.size()) {
        parent.FillBuffer();
        pos = 0;
    }
    return buffer[pos++];
}



/////////////////////////////////////////////////////////
///// Stdin32ParallelCollector class implementation /////
/////////////////////////////////////////////////////////

Stdin32ParallelCollector::Stdin32ParallelCollector()
: buffer(buffer_size), pos(buffer_size)
{
    set_bin_stdin();
    pos = buffer_size;
}

void Stdin32ParallelCollector::FillBuffer()
{
    std::unique_lock<std::mutex> lock(buffer_mutex);
    ngenerators_waiting--;
    if (ngenerators_waiting == 0) {
        const auto nvals = std::fread(buffer.data(), sizeof(uint32_t), buffer_size, stdin);
        if (nvals != buffer_size) {
            std::cerr << "Stdin32Collector::FillBuffer: cannot read from stdin; "
                      << nvals << " values were received" << std::endl;
            exit(EXIT_FAILURE);
        }
        ngenerators_waiting = ngenerators_all;
        pos = 0;
        buffer_cv.notify_all();
    } else {
        buffer_cv.wait(lock, [this]() { 
            return ngenerators_waiting == 0;
        });
    }
}


GenFactoryFunc Stdin32ParallelCollector::CreateGenFactoryFunc()
{
    return [&] () {
        std::lock_guard<std::mutex> lock(buffer_mutex);    
        auto gen = std::make_shared<Stdin32ParallelGenerator>(*this, buffer);
        ngenerators_all++;
        ngenerators_waiting++;
        return gen;
    };
}


//////////////////////





//        SmallCrushBattery bat(create_gen);
//        auto results = bat.RunTest(testid, &seq, opts->nthreads);



void print_help()
{
    static const std::string helptext{
    "Runs TestU01 batteries from binary data from stdin. Data are\n"
    "processed as unsigned 32-bit integers. All batteries are working\n"
    "in one-threaded mode. Multi-threaded mode is available only for\n"
    "C modules and testu01th_run\n\n"
    "Usage: test01th_pipes battery\n"
    "  battery: SmallCrush, Crush, BigCrush, pseudoDIEHARD\n\n"
    "Example:\n"
    "  testu01th_demo.exe stdout KISS93 | testu01th_pipes.exe SmallCrush\n\n"
    };
    std::cout << helptext << std::endl;
}

/**
 * @brief Program entry point.
 */
int main(int argc, char* argv[]) 
{
    Stdin32Collector stdin_prng;
    if (argc < 2) {
        print_help();
        return 0;
    }
    std::string battery = argv[1];
    if (battery == "SmallCrush") {
        original::battery::SmallCrush(stdin_prng);
    } else if (battery == "Crush") {
        original::battery::Crush(stdin_prng);
    } else if (battery == "BigCrush") {
        original::battery::BigCrush(stdin_prng);
    } else if (battery == "pseudoDIEHARD") {
        original::battery::pseudoDIEHARD(stdin_prng);
    } else {
        std::cerr << "Unknown battery " << battery << std::endl;
        return 1;
    }
    return 0;
}
