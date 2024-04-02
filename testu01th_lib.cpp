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

// Platform-dependent include files
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
    if (mod.gen_initlib == nullptr) {
        fprintf(stderr, "Cannot find the 'gen_closelib' function\n");
        return false;
    }
    mod.gen_getinfo = reinterpret_cast<GenGetInfoFunc>((void *) GetProcAddress(hDll, "gen_getinfo"));
    if (mod.gen_initlib == nullptr) {
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
    *gi = dummy_info;
    return 1;
}



static void init_dummy_cmodule()
{
    dummy_cmodule.gen_initlib = dummy_initlib;
    dummy_cmodule.gen_closelib = dummy_closelib;
    dummy_cmodule.gen_getinfo = dummy_gen_getinfo;
}


/**
 * @brief PRNG speed measurement.
 */
static double measure_speed(GenFactoryFunc create_gen)
{
    auto objptr = create_gen();
    double ns_per_call = 0.0;
    for (size_t niter = 65536, ms_total = 0; ms_total < 500; niter <<= 1) {
        auto tic = std::chrono::high_resolution_clock::now();
        double sum = 0.0;
        auto *gen = objptr->GetPtr();
        for (size_t k = 0; k < niter; k++) {
            sum += gen->GetU01(nullptr, gen->state);
        }
        auto toc = std::chrono::high_resolution_clock::now();
        ms_total = std::chrono::duration_cast<std::chrono::milliseconds>(toc - tic).count();
        ns_per_call = static_cast<double>(ms_total) / niter * 1e6;
    }
    return ns_per_call;
}

/*
static uint64_t seed64_rdtsc()
{
    uint64_t s = 0;
    uint32_t lcg = (uint32_t) time(NULL);
    for (size_t i = 0; i < 4; i++) {
        uint64_t tic = __rdtsc();
        lcg = (69069 * lcg + 1);
        uint32_t ms = 2 + (lcg >> 30);
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
        uint64_t toc = __rdtsc();
        s |= (toc - tic) << (i * 16);
    }   
    return s;
}
*/

static uint64_t seed64()
{
    // Rdseed instruction
    uint64_t s;
    while (!_rdseed64_step(&s)) {}
//    printf("%llX\n", seed64_rdtsc());
    return s;
}

/**
 * @brief Program entry point.
 */
int main(int argc, char *argv[]) 
{
    if (argc < 3) {
        std::cout << "Runs test batteries from TestU01 library in serial or parallel mode." << std::endl;
        std::cout << "The parallel mode allows to use all cores of CPU for computations and" << std::endl;
        std::cout << "used its own dispatcher. The serial version runs in one-threaded mode" << std::endl;
        std::cout << "and just runs the batteries from TestU01 without modification." << std::endl << std::endl;
        std::cout << "Usage: test01th_lib battery generator_lib [test_id]" << std::endl;
        std::cout << "  battery: battery name; supported batteries are:" << std::endl;
        std::cout << "    Parallel versions of batteries:" << std::endl;
        std::cout << "    - SmallCrush, Crush, BigCrush, pseudoDIEHARD" << std::endl;
        std::cout << "    Serial versions of batteries:" << std::endl;
        std::cout << "    - SmallCrush_ser, Crush_ser, BigCrush_ser, pseudoDIEHARD_ser" << std::endl;
        std::cout << "    Special measurements:" << std::endl;
        std::cout << "    - speed - measures performance of the supplied PRNG" << std::endl;
        std::cout << "  generator: PRNG name (name of dynamic library). Should export the functions:" << std::endl;
        std::cout << "    - int gen_initlib()" << std::endl;
        std::cout << "    - int gen_getinfo(GenInfoC *gi)" << std::endl;
        std::cout << "    - int gen_closelib()" << std::endl;
        std::cout << "  test_id:   Optional argument with specific test ID" << std::endl << std::endl;
        return 0;
    }

    CallerAPI intf;
    intf.get_seed64 = seed64;



    GenCModule mod;
    GenInfoC geninfo;
    int test_id = -1;

    if (!load_module(mod, argv[2])) {
        std::cerr << "Cannot load the module" << std::endl;
        return 1;
    }
    mod.gen_initlib(&intf);
    mod.gen_getinfo(&geninfo);

    auto create_gen = [&geninfo] () -> std::shared_ptr<UniformGenerator> {
        return std::shared_ptr<UniformGenerator>(new UniformGeneratorC(&geninfo));
    };

    std::string battery = argv[1];
    if (argc >= 4) {
        test_id = std::stoi(argv[3]);
        if (test_id == 0) {
            std::cerr << "Invalid test number " << argv[3] << std::endl;
            return 1;
        }
    }

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
    } else if (battery == "speed") {
        GenInfoC dummy_gen;
        init_dummy_cmodule();
        dummy_cmodule.gen_getinfo(&dummy_gen);
        auto create_dummy_gen = [&dummy_gen] () -> std::shared_ptr<UniformGenerator> {
            return std::shared_ptr<UniformGenerator>(new UniformGeneratorC(&dummy_gen));
        };
        double ns_per_call = measure_speed(create_gen);
        double ns_dummy_per_call = measure_speed(create_dummy_gen);
        double ns_per_call_corr = ns_per_call - ns_dummy_per_call;
        std::cout << "Generator name: " << geninfo.name << std::endl;
        std::cout << "Nanoseconds per call:" << std::endl;
        std::cout << "  Raw result:             " << ns_per_call << std::endl;        
        std::cout << "  For empty 'dummy' PRNG: " << ns_dummy_per_call << std::endl;
        std::cout << "  Corrected result:       " << ns_per_call_corr << std::endl;
    } else {
        std::cerr << "Unknown battery " << battery << std::endl;
    }

    mod.gen_closelib();
    return 0;
}
