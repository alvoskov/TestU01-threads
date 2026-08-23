/**
 * @file example.cpp
 * @brief A very simple "Hello, World" example of TestU01-threads usage
 * for custom pseudorandom generators testing.
 * source code.
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
#include <iostream>
#include <cstdint>

using namespace testu01_threads;

class Xorshift64Generator : public UniformGenerator
{
    uint64_t x;

public:
    Xorshift64Generator(uint64_t seed = 1234)
    : UniformGenerator{"xorshift64"}, x{(seed == 0) ? 1234 : seed}
    {
    }

    inline uint64_t GetBits64()
    {
        x ^= x >> 12;
        x ^= x << 25;
        x ^= x >> 27;
        return x;
    }

    double GetU01() override { return uint64_to_udouble(GetBits64()); }
    uint32_t GetBits32() override { return static_cast<uint32_t>(GetBits64() >> 32); }
};


int main()
{
    SmallCrushBattery bat([] () -> std::shared_ptr<UniformGenerator> {
        return std::make_shared<Xorshift64Generator>();
    });
    const auto res = bat.Run();
    std::cout << res << std::endl;
    return 0;
}
