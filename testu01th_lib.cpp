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
#include <map>
#include <cstring>
#include <functional>
#include <memory>
#include <vector>
#include <thread>
#include <mutex>
#include <algorithm>
#include <iostream>


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
        fprintf(stderr, "Cannot load the '%s' module", libname);
        return false;
    }

    static HModuleWrapper dll_wrapper(hDll);

    mod.gen_initlib = reinterpret_cast<GenInitLibFunc>((void *) GetProcAddress(hDll, "gen_initlib"));
    if (mod.gen_initlib == nullptr) {
        fprintf(stderr, "Cannot find the 'gen_initlib' function");
        return false;
    }
    mod.gen_closelib = reinterpret_cast<GenCloseLibFunc>((void *) GetProcAddress(hDll, "gen_closelib"));
    if (mod.gen_initlib == nullptr) {
        fprintf(stderr, "Cannot find the 'gen_closelib' function");
        return false;
    }
    mod.gen_getinfo = reinterpret_cast<GenGetInfoFunc>((void *) GetProcAddress(hDll, "gen_getinfo"));
    if (mod.gen_initlib == nullptr) {
        fprintf(stderr, "Cannot find the 'gen_getinfo' function");
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
        fprintf(stderr, "Cannot find the 'gen_initlib' function");
        return false;
    }
    mod.gen_closelib = reinterpret_cast<GenCloseLibFunc>(dlsym(lib, "gen_closelib"));
    if (mod.gen_initlib == nullptr) {
        fprintf(stderr, "Cannot find the 'gen_closelib' function");
        return false;
    }
    mod.gen_getinfo = reinterpret_cast<GenGetInfoFunc>(dlsym(lib, "gen_getinfo"));
    if (mod.gen_initlib == nullptr) {
        fprintf(stderr, "Cannot find the 'gen_getinfo' function");
        return false;
    }

    return true;
}


/////////////////////////////////////
///// End of UNIX-specific code /////
/////////////////////////////////////
#endif


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
        std::cout << "  generator: PRNG name (name of dynamic library). Should export the functions:" << std::endl;
        std::cout << "    - int gen_initlib()" << std::endl;
        std::cout << "    - int gen_getinfo(GenInfoC *gi)" << std::endl;
        std::cout << "    - int gen_closelib()" << std::endl;
        std::cout << "  test_id:   Optional argument with specific test ID" << std::endl << std::endl;
        return 0;
    }

    GenCModule mod;
    GenInfoC geninfo;
    int test_id = -1;

    if (!load_module(mod, argv[2])) {
        std::cerr << "Cannot load the module" << std::endl;
        return 1;
    }
    mod.gen_initlib();
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
    } else {
        std::cerr << "Unknown battery " << battery << std::endl;
    }

    mod.gen_closelib();
    return 0;
}
