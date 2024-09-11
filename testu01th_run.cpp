/**
 * @file testu01th_run.cpp
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
#include "dummy_module.h"
#include "speedtest.h"

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
#include <fstream>
#include <chrono>
#include <thread>

using namespace testu01_threads;

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

/**
 * @brief Seeds generation for PRNGs
 */
Entropy entropy;


/**
 * @brief Obtain hardware generated seed (random number)
 * from the RDSEED instruction. Used during configuration of C modules
 * interfaces.
 */
static uint64_t seed64()
{
    return entropy.Seed64();
}


void print_help()
{
    static const std::string helptext(
    "Runs test batteries from TestU01 library in serial or parallel mode.\n"
    "The parallel mode allows to use all cores of CPU for computations and\n"
    "used its own dispatcher. The serial version runs in one-threaded mode\n"
    "and just runs the batteries from TestU01 without modification.\n\n"
    "Usage: test01th_lib battery generator_lib [test_id] [gen_options]\n"
    "  battery: battery name; supported batteries are:\n"
    "    Parallel versions of batteries:\n"
    "    - SmallCrush, Crush, BigCrush, pseudoDIEHARD\n"
    "    Serial versions of batteries:\n"
    "    - SmallCrush_ser, Crush_ser, BigCrush_ser, pseudoDIEHARD_ser\n"
    "    Output to stdout that is useful for practrand library:\n"
    "    - stdout32, stdout64, stdout32v, stdout64v;\n"
    "      'v' means vectorized versions of generators\n"
    "    Special measurements for the supplied PRNG:\n"
    "    - speed - measures performance\n"
    "    - selftest - runs the internal self-test\n"
    "  generator_lib: name of dynamic library with PRNG that export the functions:\n"
    "    - int gen_initlib()\n"
    "    - int gen_getinfo(GenInfoC *gi)\n"
    "    - int gen_closelib()\n"
    "  test_id:   Optional argument with specific test ID\n"
    "  gen_options: Optional argument with generator options\n\n"
    "Examples:\n"
    "  testu01th_lib SmallCrush lcg64_shared.dll\n"
    "  testu01th_lib stdout32 lcg64_shared.dll | RNG_test stdin32 -multithreaded");

    std::cout << helptext << std::endl << std::endl;
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
 * @brief Save the full protocol to the file
 */
void SaveProtocol(const BatteryResults &results, Entropy &entropy)
{
    std::ofstream outfile;
    char buf[256];
    size_t nseeds = entropy.GetNSeeds();
    size_t nthreads = results.pvalues.size();
    size_t seeds_per_thread = nseeds / nthreads;
    outfile.open("report.txt");
    outfile << results.ToString() << std::endl;
    outfile << "========= Seeds allocator report =========" << std::endl;
    outfile << "  Number of threads: " << nthreads << std::endl;
    outfile << "  Seeds generated:   " << nseeds << std::endl;
    outfile << "  Seeds per thread:  " << nseeds / nthreads << std::endl;
    outfile << "  Seeds outside threads: " <<
        nseeds - nthreads * seeds_per_thread << std::endl << std::endl;
    outfile << "===== List of seeeds =====" << std::endl;
    snprintf(buf, 256, "  %3s %3s %25s   %16s\n",
        "TH", "#", "DEC", "HEX");
    outfile << std::string(buf);
    for (size_t i = 0, pos = 0; i < nthreads; i++) {
        for (size_t j = 0; j < seeds_per_thread; j++, pos++) {
            uint64_t seed = entropy.seeds_log[pos];
            snprintf(buf, 256, "  %3d %3d %25llu 0x%16.16llX\n",
                (int) i, (int) j,
                (unsigned long long) seed, (unsigned long long) seed);
            outfile << std::string(buf);
        }
    }
    outfile << std::endl;
}


void RunBattery(TestsBattery &bat, int test_id, Entropy &entropy)
{
    auto results = bat.RunTest(test_id);
    std::cout << results.report;
    SaveProtocol(results, entropy);
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
    // Run the selected battery
    if (battery == "SmallCrush") {
        SmallCrushBattery bat(create_gen);
        RunBattery(bat, test_id, entropy);
    } else if (battery == "Crush") {
        CrushBattery bat(create_gen);
        RunBattery(bat, test_id, entropy);
    } else if (battery == "BigCrush") {
        BigCrushBattery bat(create_gen);
        RunBattery(bat, test_id, entropy);
    } else if (battery == "pseudoDIEHARD") {
        PseudoDiehardBattery bat(create_gen);
        RunBattery(bat, test_id, entropy);
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
    } else if (battery == "stdout32") {
        prng_bits32_to_file(create_gen());
    } else if (battery == "stdout32v") {
        if (geninfo.get_array32 == nullptr) {
            std::cerr << "This PRNG doesn't support vectorized 32-bit mode" << std::endl;
            return 1;
        }
        prng_array32_to_file(create_gen());
    } else if (battery == "stdout64") {
        if (geninfo.get_bits64 == nullptr) {
            std::cerr << "This PRNG doesn't support 64-bit mode" << std::endl;
            return 1;
        }
        prng_bits64_to_file(create_gen());
    } else if (battery == "stdout64v") {
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
