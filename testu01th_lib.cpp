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
#include "bigcrush.h"
#include "crush.h"
#include "smallcrush.h"
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

bool load_module(GenCModule &mod, const char *libname)
{

/*
    ext_library prng_lib = dlopen(argv[2], RTLD_LAZY);
    if (!ext_library) {
        fprintf(stderr, "dlopen() error: %s\n", dlerror());
        return 1;
    };

    dlclose(prng_lib);
*/

    return false;
}


/////////////////////////////////////
///// End of UNIX-specific code /////
/////////////////////////////////////
#endif


////////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[]) 
{
    if (argc != 3) {
        std::cout << "Usage: test_lib battery generator_lib" << std::endl;
        std::cout << "  battery: battery name; supported batteries are:" << std::endl;
        std::cout << "    - SmallCrush - parallel version of TestU01 SmallCrush" << std::endl;
        std::cout << "    - Crush      - parallel version of TestU01 Crush" << std::endl;
        std::cout << "    - BigCrush   - parallel version of TestU01 BigCrush" << std::endl;
        std::cout << "    - SmallCrush_ser - classic (serial) version of SmallCrush" << std::endl;
        std::cout << "    - Crush_ser      - classic (serial) version of Crush" << std::endl;
        std::cout << "    - BigCrush_ser   - classic (serial) version of BigCrush" << std::endl;
        std::cout << "  generator: PRNG name (name of dynamic library)\n" << std::endl;
        return 0;
    }

    GenCModule mod;
    GenInfoC geninfo;

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
    if (battery == "SmallCrush") {
        mt_bat_SmallCrush(create_gen);
    } else if (battery == "Crush") {
        mt_bat_Crush(create_gen);
    } else if (battery == "BigCrush") {
        mt_bat_BigCrush(create_gen);
    } else if (battery == "SmallCrush_ser") {
        auto objptr = create_gen();
        bbattery_SmallCrush(objptr->GetPtr());
    } else if (battery == "Crush_ser") {
        auto objptr = create_gen();
        bbattery_Crush(objptr->GetPtr());
    } else if (battery == "BigCrush_ser") {
        auto objptr = create_gen();
        bbattery_BigCrush(objptr->GetPtr());
    } else {
        std::cerr << "Unknown battery " << battery << std::endl;
    }

    mod.gen_closelib();
    return 0;
}
