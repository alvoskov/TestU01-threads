/**
 * @file testu01th_lib.cpp
 * @brief Loads a pseudorandom number generator from an external dynamic
 * library with a simple C-style interface. Simplicity of the interface
 * allows to link PRNG containing modules without C++ compiler and without
 * usage of header files of the TestU01 library.
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

#include "testu01_mt.h"
#include "batteries.h"
#include "entropy.h"
#include "generators.h"

#include <stdlib.h>
#include <stdio.h>

#include <string>
#include <system_error>
#include <map>
#include <cstring>
#include <chrono>
#include <functional>
#include <memory>
#include <vector>
#include <thread>
#include <mutex>
#include <algorithm>
#include <iostream>
#include <chrono>
#include <thread>

//////////////////////////
///// Some constants /////
//////////////////////////

constexpr size_t ELEMENTS_PER_BLOCK = 1000;



////////////////////////////////////////////
///// Platform-dependent include files /////
////////////////////////////////////////////
#include <immintrin.h>
#include <x86intrin.h>

#ifdef USE_LOADLIBRARY
//////////////////////////////////////////
///// Begin of WINDOWS-specific code /////
//////////////////////////////////////////
#include <windows.h>

class HModuleWrapper
{
    HMODULE hDll;

public:
    HModuleWrapper() : hDll(0) {}
    HModuleWrapper(HMODULE h) : hDll(h) {}
    ~HModuleWrapper()
    {
        if (hDll != 0)
            FreeLibrary(hDll);
    }
};

bool load_module(GenCModule &mod, const char *libname)
{
    HMODULE hDll = LoadLibraryA(libname);
    if (hDll == 0 || hDll == INVALID_HANDLE_VALUE) {
        int errcode = (int) GetLastError();
        fprintf(stderr, "Cannot load the '%s' module; error code: %d\n",
            libname, errcode);
        return false;
    }

    static HModuleWrapper dll_wrapper(hDll);

    mod.gen_initlib = reinterpret_cast<GenInitLibFunc>((void *) GetProcAddress(hDll, "gen_initlib"));
    if (mod.gen_initlib == nullptr) {
        fprintf(stderr, "Cannot find the 'gen_initlib' function\n");
        return false;
    }
    mod.gen_closelib = reinterpret_cast<GenCloseLibFunc>((void *) GetProcAddress(hDll, "gen_closelib"));
    if (mod.gen_closelib == nullptr) {
        fprintf(stderr, "Cannot find the 'gen_closelib' function\n");
        return false;
    }
    mod.gen_getinfo = reinterpret_cast<GenGetInfoFunc>((void *) GetProcAddress(hDll, "gen_getinfo"));
    if (mod.gen_getinfo == nullptr) {
        fprintf(stderr, "Cannot find the 'gen_getinfo' function\n");
        return false;
    }
    return true;
}

////////////////////////////////////////
///// End of WINDOWS-specific code /////
////////////////////////////////////////
#else
///////////////////////////////////////
///// Begin of UNIX-specific code /////
///////////////////////////////////////

#include <dlfcn.h>

class ExtLibraryWrapper
{
    void *lib;

public:
    ExtLibraryWrapper() : lib(nullptr) {}
    ExtLibraryWrapper(void *h) : lib(h) {}
    ~ExtLibraryWrapper()
    {
        if (lib != nullptr)
            dlclose(lib);
    }
};


bool load_module(GenCModule &mod, const char *libname)
{
    void *lib = dlopen(libname, RTLD_LAZY);
    if (lib == nullptr) {
        fprintf(stderr, "dlopen() error: %s\n", dlerror());
        return false;
    };

    static ExtLibraryWrapper lib_wrapper(lib);

    mod.gen_initlib = reinterpret_cast<GenInitLibFunc>(dlsym(lib, "gen_initlib"));
    if (mod.gen_initlib == nullptr) {
        fprintf(stderr, "Cannot find the 'gen_initlib' function\n");
        return false;
    }
    mod.gen_closelib = reinterpret_cast<GenCloseLibFunc>(dlsym(lib, "gen_closelib"));
    if (mod.gen_initlib == nullptr) {
        fprintf(stderr, "Cannot find the 'gen_closelib' function\n");
        return false;
    }
    mod.gen_getinfo = reinterpret_cast<GenGetInfoFunc>(dlsym(lib, "gen_getinfo"));
    if (mod.gen_initlib == nullptr) {
        fprintf(stderr, "Cannot find the 'gen_getinfo' function\n");
        return false;
    }

    return true;
}


/////////////////////////////////////
///// End of UNIX-specific code /////
/////////////////////////////////////
#endif



////////////////////////////////////////////////////////////////////
///// An implementation of "dummy" PRNG that always returns 0. /////
///// It is used only for performance measurements!            /////
////////////////////////////////////////////////////////////////////

static GenCModule dummy_cmodule;

static int dummy_initlib(CallerAPI *intf)
{
    (void) intf;
    return 1;
}

static int dummy_closelib(void)
{
    return 1;
}



static double dummy_get_u01(void *param, void *state)
{
    (void) param;
    (void) state;
    return 0.0;
}


static long unsigned int dummy_get_bits32(void *param, void *state)
{
    (void) param;
    (void) state;
    return 0;
}

static uint64_t dummy_get_bits64(void *param, void *state)
{
    (void) param;
    (void) state;
    return 0;
}

static void dummy_get_array32(void *param, void *state, uint32_t *out, size_t len)
{
    (void) param;
    (void) state;
    (void) out;
    (void) len;
}

static void dummy_get_array64(void *param, void *state, uint64_t *out, size_t len)
{
    (void) param;
    (void) state;
    (void) out;
    (void) len;
}



static void *dummy_init_state() { return nullptr; }

static void dummy_delete_state(void *param, void *state)
{
    (void) param;
    free(state);
}


static int dummy_gen_getinfo(GenInfoC *gi)
{
    static const char name[] = "Dummy";
    static GenInfoC dummy_info;
    dummy_info.name = name;
    dummy_info.init_state = dummy_init_state;
    dummy_info.delete_state = dummy_delete_state;
    dummy_info.get_u01 = dummy_get_u01;
    dummy_info.get_bits32 = dummy_get_bits32;
    dummy_info.get_bits64 = dummy_get_bits64;
    dummy_info.get_array32 = dummy_get_array32;
    dummy_info.get_array64 = dummy_get_array64;
    dummy_info.run_self_test = nullptr;
    *gi = dummy_info;
    return 1;
}



static void init_dummy_cmodule()
{
    dummy_cmodule.gen_initlib = dummy_initlib;
    dummy_cmodule.gen_closelib = dummy_closelib;
    dummy_cmodule.gen_getinfo = dummy_gen_getinfo;
}


/*
uint64_t seed64_rdtsc()
{
    uint64_t s = 0;
    char buf[16];
    uint32_t lcg = (uint32_t) time(NULL);
    lcg ^= (uint32_t) __rdtsc() & 0xFFFFFFFF;
    for (size_t i = 0; i < 8; i++) {
        uint64_t tic = __rdtsc();
        for (size_t j = 0; j < 16; j++) {
            for (size_t k = 0; k < 16; k++) {
                lcg = (69069 * lcg + 1);
                buf[k] = ((lcg >> 16) % 26) + 'A';
            }
            buf[15] = 0;
            getenv(buf);
            //printf("%s\n", buf);
        }
        //uint32_t ms = 1000;// + (lcg >> 30);
        //std::this_thread::sleep_for(std::chrono::microseconds(ms));
        uint64_t toc = __rdtsc();
        s |= ((toc - tic) & 0xFF) << (i * 8);
    }
    s ^= __rdtsc();
    return s;
}
*/


/**
 * @brief Seeds generation for PRNGs
 */
Entropy entropy;


/**
 * @brief Obtain hardware generated seed (random number)
 * from the RDSEED instruction.
 */
static uint64_t seed64()
{
/*
    long long unsigned int rd;
    while (!_rdseed64_step(&rd)) {}
    return static_cast<uint64_t>(rd);
*/
    return entropy.Seed64();
}

void print_help()
{
    std::cout << "Runs test batteries from TestU01 library in serial or parallel mode." << std::endl;
    std::cout << "The parallel mode allows to use all cores of CPU for computations and" << std::endl;
    std::cout << "used its own dispatcher. The serial version runs in one-threaded mode" << std::endl;
    std::cout << "and just runs the batteries from TestU01 without modification." << std::endl << std::endl;
    std::cout << "Usage: test01th_lib battery generator_lib [test_id] [gen_options]" << std::endl;
    std::cout << "  battery: battery name; supported batteries are:" << std::endl;
    std::cout << "    Parallel versions of batteries:" << std::endl;
    std::cout << "    - SmallCrush, Crush, BigCrush, pseudoDIEHARD" << std::endl;
    std::cout << "    Serial versions of batteries:" << std::endl;
    std::cout << "    - SmallCrush_ser, Crush_ser, BigCrush_ser, pseudoDIEHARD_ser" << std::endl;
    std::cout << "    Output for practrand library:" << std::endl;
    std::cout << "    - practrand32, practrand64, practrand32v, practrand64v" << std::endl;
    std::cout << "      Use as testu01th_lib practrand32 generator_lib | RNG_test stdin32 -multithreaded" << std::endl;
    std::cout << "             testu01th_lib practrand64 generator_lib | RNG_test stdin64 -multithreaded" << std::endl;
    std::cout << "      'v' means vectorized versions of generators" << std::endl;
    std::cout << "    Special measurements for the supplied PRNG:" << std::endl;
    std::cout << "    - speed - measures performance" << std::endl;
    std::cout << "    - selftest - runs the internal self-test" << std::endl;
    std::cout << "  generator: PRNG name (name of dynamic library). Should export the functions:" << std::endl;
    std::cout << "    - int gen_initlib()" << std::endl;
    std::cout << "    - int gen_getinfo(GenInfoC *gi)" << std::endl;
    std::cout << "    - int gen_closelib()" << std::endl;
    std::cout << "  test_id:   Optional argument with specific test ID" << std::endl;
    std::cout << "  gen_options: Optional argument with generator options" << std::endl << std::endl;
}

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
    uint64_t sum = 0;
    std::vector<uint32_t> buf(ELEMENTS_PER_BLOCK);
    for (size_t k = 0; k < niter; k++) {
        objptr->GetArray32(buf.data(), ELEMENTS_PER_BLOCK);
        sum += buf[0];
    }
    return static_cast<size_t>(sum);
}


static size_t run_array64_block(std::shared_ptr<UniformGenerator> &objptr, size_t niter)
{
    uint64_t sum = 0;
    std::vector<uint64_t> buf(ELEMENTS_PER_BLOCK);
    for (size_t k = 0; k < niter; k++) {
        objptr->GetArray64(buf.data(), ELEMENTS_PER_BLOCK);
        sum += buf[0];
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
    for (size_t niter = 65536, ms_total = 0; ms_total < 500; niter <<= 1) {
        auto tic = std::chrono::high_resolution_clock::now();
        uint64_t tic_proc = __rdtsc();
        run_block_func(objptr, niter);
        uint64_t toc_proc = __rdtsc();
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
    init_dummy_cmodule();
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


CallerAPI get_caller_api()
{
    CallerAPI intf;
    intf.get_seed64 = seed64;
    intf.malloc = malloc;
    intf.free = free;
    intf.printf = printf;
    intf.strcmp = strcmp;
    return intf;
}


static int run_self_test(const GenInfoC &geninfo)
{
    std::cout << "----- Internal self-test -----" << std::endl;
    if (geninfo.run_self_test == nullptr) {
        std::cout << "Internal self-test is not implemented" << std::endl;
        return 1;
    } else {
        int ans = geninfo.run_self_test();
        if (ans) {
            std::cout << "Internal self-test: PASSED" << std::endl;
            return 0;
        } else {
            std::cout << "Internal self-test: NOT PASSED" << std::endl;
            return 1;
        }
    }
}


void test_battery_speed(const GenFactoryFunc &create_gen, const GenInfoC &geninfo)
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
        std::cout << "----- Array of uint32 generatior is not implemented -----" << std::endl;
    }

    if (geninfo.get_array64 != nullptr) {
        std::cout << "----- Speed test for array of uint64 generation -----" << std::endl;
        test_speed(create_gen, geninfo, run_array64_block, ELEMENTS_PER_BLOCK * 64);
        std::cout << std::endl;
    } else {
        std::cout << "----- Array of uint64 generatior is not implemented -----" << std::endl;
    }
}


std::string get_gen_options(int argc, char *argv[])
{
    std::string gen_options;
    if (argc >= 5) {
        gen_options = std::string(argv[4]);
    }
    return gen_options;
}

/**
 * @brief Returns test ID (from SmallCrush, Crush or BigCrush battery)
 * selected by the user.
 * @return -1 -- no test selected, 0 -- invalid test ID, >0 - test ID.
 */
int get_test_id(int argc, char *argv[])
{
    int test_id = -1;
    if (argc >= 4) {
        test_id = std::stoi(argv[3]);
        if (test_id == 0) {
            std::cerr << "Invalid test number " << argv[3] << std::endl;
            return 0;
        }
    }
    return test_id;
}

/**
 * @brief Program entry point.
 */
int main(int argc, char *argv[]) 
{
    // Get command line arguments
    if (argc < 3) {
        print_help();
        //std::cout << entropy.XxteaTest() << std::endl;
        //std::cout << entropy.Seed64() << std::endl;
        //std::cout << entropy.Seed64() << std::endl;
        return 0;
    }
    std::string battery = argv[1];
    const char *module_name = argv[2];
    int test_id = get_test_id(argc, argv);
    std::string gen_options = get_gen_options(argc, argv);
    if (test_id == 0) {
        return 0;
    }

    GenCModule mod;
    if (!load_module(mod, module_name)) {
        std::cerr << "Cannot load the module" << std::endl;
        return 1;
    }

    CallerAPI intf = get_caller_api();
    GenInfoC geninfo;
    GenInfoC_init(&geninfo);
    geninfo.options = gen_options.c_str();
    mod.gen_initlib(&intf);
    if (!mod.gen_getinfo(&geninfo)) {
        std::cerr << "Error: PRNG `gen_getinfo` function failed" << std::endl;
        return 1;
    }

    auto create_gen = [&geninfo] () -> std::shared_ptr<UniformGenerator> {
        return std::shared_ptr<UniformGenerator>(new UniformGeneratorC(&geninfo));
    };


    if (battery == "SmallCrush") {
        SmallCrushBattery bat(create_gen);
        bat.RunTest(test_id);
    } else if (battery == "Crush") {
        CrushBattery bat(create_gen);
        bat.RunTest(test_id);
    } else if (battery == "BigCrush") {
        BigCrushBattery bat(create_gen);
        bat.RunTest(test_id);
    } else if (battery == "pseudoDIEHARD") {
        PseudoDiehardBattery bat(create_gen);
        bat.RunTest(test_id);
    } else if (battery == "SmallCrush_ser") {
        auto objptr = create_gen();
        bbattery_SmallCrush(objptr->GetPtr());
    } else if (battery == "Crush_ser") {
        auto objptr = create_gen();
        bbattery_Crush(objptr->GetPtr());
    } else if (battery == "BigCrush_ser") {
        auto objptr = create_gen();
        bbattery_BigCrush(objptr->GetPtr());
    } else if (battery == "pseudoDIEHARD_ser") {
        auto objptr = create_gen();
        bbattery_pseudoDIEHARD(objptr->GetPtr());
    } else if (battery == "practrand32") {
        prng_bits32_to_file(create_gen());
    } else if (battery == "practrand32v") {
        if (geninfo.get_array32 == nullptr) {
            std::cerr << "This PRNG doesn't support vectorized 32-bit mode" << std::endl;
            return 1;
        }
        prng_array32_to_file(create_gen());
    } else if (battery == "practrand64") {
        if (geninfo.get_bits64 == nullptr) {
            std::cerr << "This PRNG doesn't support 64-bit mode" << std::endl;
            return 1;
        }
        prng_bits64_to_file(create_gen());
    } else if (battery == "practrand64v") {
        if (geninfo.get_array64 == nullptr) {
            std::cerr << "This PRNG doesn't support vectorized 64-bit mode" << std::endl;
            return 1;
        }
        prng_array64_to_file(create_gen());
    } else if (battery == "speed") {
        test_battery_speed(create_gen, geninfo);
    } else if (battery == "selftest") {
        return run_self_test(geninfo);
    } else {
        std::cerr << "Unknown battery " << battery << std::endl;
    }

    mod.gen_closelib();
    return 0;
}
