/**
 * @file testu01th_pipes.cpp
 * @brief Reads pseudorandom numbers from stdin as uint32_t in binary form
 * and sends it to one-threaded version of TestU01.
 *
 * @copyright (c) 2024 Alexey L. Voskov, Lomonosov Moscow State University.
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

using namespace testu01_threads;

/**
 * @brief This pseudorandom number generator obtains numbers
 * from stdin (as uint32_t in binary format).
 */
class Stdin32Collector : public UniformGenerator
{
    static constexpr size_t buffer_size = 2048;
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
        fread(buffer, sizeof(uint32_t), buffer_size, stdin);
        pos = 0;
    }

    double GetU01() override
    {
        return (double) GetBits32() / ((double) UINT_MAX + 1.0);
    }

    uint32_t GetBits32() override
    {
        if (pos == buffer_size) {
            FillBuffer();
        }
        return buffer[pos++];
    }
};



void print_help()
{
    const std::string helptext(
    "Runs TestU01 batteries from binary data from stdin. Data are\n"
    "processed as unsigned 32-bit integers. All batteries are working\n"
    "in one-threaded mode. Multi-threaded mode is available only for\n"
    "C modules and testu01th_run\n\n"
    "Usage: test01th_pipes battery\n"
    "  battery: SmallCrush, Crush, BigCrush, pseudoDIEHARD\n\n"
    "Example:\n"
    "  testu01th_run.exe stdout32 gen.dll | testu01th_pipes.exe SmallCrush\n\n"
    );
    std::cout << helptext << std::endl;
}

/**
 * @brief Program entry point.
 */
int main(int argc, char *argv[]) 
{
    Stdin32Collector stdin_prng;
    if (argc < 2) {
        print_help();
        return 0;
    }
    std::string battery = argv[1];
    if (battery == "SmallCrush") {
        bbattery_SmallCrush(stdin_prng.GetPtr());
    } else if (battery == "Crush") {
        bbattery_Crush(stdin_prng.GetPtr());
    } else if (battery == "BigCrush") {
        bbattery_BigCrush(stdin_prng.GetPtr());
    } else if (battery == "pseudoDIEHARD") {
        bbattery_pseudoDIEHARD(stdin_prng.GetPtr());
    } else {
        std::cerr << "Unknown battery " << battery << std::endl;
        return 1;
    }
    return 0;
}
