#include "testu01th/speedtest.h"
#include <iostream>
#include <cmath>

using namespace testu01_threads;

static GenCModule dummy_cmodule = dummy_init_cmodule();

static size_t run_u01_block(std::shared_ptr<UniformGenerator> &objptr, size_t niter)
{
    double sum = 0.0;
    auto *gen = objptr->GetPtr();
    for (size_t k = 0; k < niter; k++) {
        sum += gen->GetU01(nullptr, gen->state);
    }
    return static_cast<size_t>(sum);
}

static size_t run_uint32_block(std::shared_ptr<UniformGenerator> &objptr, size_t niter)
{
    long unsigned int sum = 0;
    auto *gen = objptr->GetPtr();
    for (size_t k = 0; k < niter; k++) {
        sum += gen->GetBits(nullptr, gen->state);
    }
    return static_cast<size_t>(sum);
}

static size_t run_uint64_block(std::shared_ptr<UniformGenerator> &objptr, size_t niter)
{
    uint64_t sum = 0;
    for (size_t k = 0; k < niter; k++) {
        sum += objptr->GetBits64();
    }
    return static_cast<size_t>(sum);
}


static size_t run_array32_block(std::shared_ptr<UniformGenerator> &objptr, size_t niter)
{
    uint32_t sum = 0;
    std::vector<uint32_t> buf(ELEMENTS_PER_BLOCK);
    for (size_t i = 0; i < niter; i++) {
        objptr->GetArray32(buf.data(), ELEMENTS_PER_BLOCK);
        sum += buf[0];
    }
    return static_cast<size_t>(sum);
}


static size_t run_array64_block(std::shared_ptr<UniformGenerator> &objptr, size_t niter)
{
    uint64_t sum = 0;
    std::vector<uint64_t> buf(ELEMENTS_PER_BLOCK);
    for (size_t i = 0; i < niter; i++) {
        objptr->GetArray64(buf.data(), ELEMENTS_PER_BLOCK);
        sum += buf[0];
    }
    return static_cast<size_t>(sum);
}

static size_t run_sum32_block(std::shared_ptr<UniformGenerator> &objptr, size_t niter)
{
    uint32_t sum = 0;
    for (size_t i = 0; i < niter; i++) {
        sum += objptr->GetSum32(ELEMENTS_PER_BLOCK);
    }
    return static_cast<size_t>(sum);
}

static size_t run_sum64_block(std::shared_ptr<UniformGenerator> &objptr, size_t niter)
{
    uint64_t sum = 0;
    for (size_t i = 0; i < niter; i++) {
        sum += objptr->GetSum64(ELEMENTS_PER_BLOCK);
    }
    return static_cast<size_t>(sum);
}



/**
 * @brief Keeps PRNG speed measurements results.
 */
class SpeedResults
{
public:
    double ns_per_call; ///< Nanoseconds per call
    double ticks_per_call; ///< Processor ticks per call

    SpeedResults() : ns_per_call(0.0), ticks_per_call(0) {}
};

/**
 * @brief PRNG speed measurement for uint32 output
 */
static SpeedResults measure_speed(GenFactoryFunc create_gen,
    size_t (*run_block_func)(std::shared_ptr<UniformGenerator> &objptr, size_t niter))
{
    auto objptr = create_gen();
    SpeedResults results;
    for (size_t niter = 2, ms_total = 0; ms_total < 500; niter <<= 1) {
        auto tic = std::chrono::high_resolution_clock::now();
        uint64_t tic_proc = Entropy::CpuClock();
        run_block_func(objptr, niter);
        uint64_t toc_proc = Entropy::CpuClock();
        auto toc = std::chrono::high_resolution_clock::now();
        ms_total = std::chrono::duration_cast<std::chrono::milliseconds>(toc - tic).count();
        results.ns_per_call = static_cast<double>(ms_total) / niter * 1e6;
        results.ticks_per_call = static_cast<double>(toc_proc - tic_proc) / niter;
    }
    return results;
}


static void test_speed(GenFactoryFunc create_gen,
    const GenInfoC &geninfo,
    size_t (*run_block_func)(std::shared_ptr<UniformGenerator> &objptr, size_t niter),
    size_t nbits = 32)
{
    GenInfoC dummy_gen;
    dummy_cmodule.gen_getinfo(&dummy_gen);
    auto create_dummy_gen = [&dummy_gen] () -> std::shared_ptr<UniformGenerator> {
        return std::shared_ptr<UniformGenerator>(new UniformGeneratorC(&dummy_gen));
    };
    auto speed_full = measure_speed(create_gen, run_block_func);
    auto speed_dummy = measure_speed(create_dummy_gen, run_block_func);
    double ns_per_call_corr = speed_full.ns_per_call - speed_dummy.ns_per_call;
    double ticks_per_call_corr = speed_full.ticks_per_call - speed_dummy.ticks_per_call;
    double cpb_corr = ticks_per_call_corr / (nbits / 8);
    double gb_per_sec = (double) nbits / 8.0 / (1.0e-9 * ns_per_call_corr) / pow(2.0, 30.0);

    std::cout << "Generator name: " << geninfo.name << std::endl;
    std::cout << "Nanoseconds per call:" << std::endl;
    std::cout << "  Raw result:                " << speed_full.ns_per_call << std::endl;        
    std::cout << "  For empty 'dummy' PRNG:    " << speed_dummy.ns_per_call << std::endl;
    std::cout << "  Corrected result:          " << ns_per_call_corr << std::endl;
    std::cout << "  Corrected result (GB/sec): " << gb_per_sec << std::endl;
    std::cout << "CPU ticks per call:" << std::endl;
    std::cout << "  Raw result:                " << speed_full.ticks_per_call << std::endl;        
    std::cout << "  For empty 'dummy' PRNG:    " << speed_dummy.ticks_per_call << std::endl;
    std::cout << "  Corrected result:          " << ticks_per_call_corr << std::endl;
    std::cout << "  Corrected result (cpB):    " << cpb_corr << std::endl;
}


void testu01_threads::test_battery_speed(const GenFactoryFunc &create_gen, const GenInfoC &geninfo)
{
    // Part 1. Scalar tests
    std::cout << "----- Speed test for double generation -----" << std::endl;
    test_speed(create_gen, geninfo, run_u01_block);
    std::cout << std::endl;
    std::cout << "----- Speed test for uint32 generation -----" << std::endl;
    test_speed(create_gen, geninfo, run_uint32_block);
    std::cout << std::endl;
    if (geninfo.get_bits64 != nullptr) {
        std::cout << "----- Speed test for uint64 generation -----" << std::endl;
        test_speed(create_gen, geninfo, run_uint64_block, 64);
        std::cout << std::endl;
    } else {
        std::cout << "----- uint64 generator is not implemented -----" << std::endl;
    }
    // Part 2. Vectorized tests
    if (geninfo.get_array32 != nullptr) {
        std::cout << "----- Speed test for array of uint32 generation -----" << std::endl;
        test_speed(create_gen, geninfo, run_array32_block, ELEMENTS_PER_BLOCK * 32);
    } else {
        std::cout << "----- Array of uint32 generator is not implemented -----" << std::endl;
    }

    if (geninfo.get_array64 != nullptr) {
        std::cout << "----- Speed test for array of uint64 generation -----" << std::endl;
        test_speed(create_gen, geninfo, run_array64_block, ELEMENTS_PER_BLOCK * 64);
        std::cout << std::endl;
    } else {
        std::cout << "----- Array of uint64 generator is not implemented -----" << std::endl;
    }

    // Part 3. Inlining tests
    if (geninfo.get_sum32 != nullptr) {
        std::cout << "----- Speed test for sum of uint32 generation -----" << std::endl;
        test_speed(create_gen, geninfo, run_sum32_block, ELEMENTS_PER_BLOCK * 32);
        std::cout << std::endl;
    } else {
        std::cout << "----- Sum of uint32 generator is not implemented -----" << std::endl;
    }

    if (geninfo.get_sum64 != nullptr) {
        std::cout << "----- Speed test for sum of uint64 generation -----" << std::endl;
        test_speed(create_gen, geninfo, run_sum64_block, ELEMENTS_PER_BLOCK * 64);
        std::cout << std::endl;
    } else {
        std::cout << "----- Sum of uint64 generator is not implemented -----" << std::endl;
    }
}
