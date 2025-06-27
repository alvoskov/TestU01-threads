cmake_minimum_required (VERSION 3.5)
project (TestU01-threads)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Compiler settings
if (NOT MSVC)
    set(PROF_FLAGS "")
    set(CMODULE_LINK_FLAGS "-nostdlib")
    set(C_CXX_FLAGS "-O2 -Wall -Werror -Wextra -Wno-attributes -march=native")
    #set(PROF_FLAGS "-g -pg")
    if (WIN32)
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${PROF_FLAGS} -std=gnu++14 ${C_CXX_FLAGS} -static-libstdc++ -static-libgcc")
        set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${PROF_FLAGS} -std=c99  -ffreestanding ${C_CXX_FLAGS}")
    else()
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${PROF_FLAGS} -std=gnu++14 ${C_CXX_FLAGS}")
        set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${PROF_FLAGS} -std=c99  -ffreestanding ${C_CXX_FLAGS}")
    endif()
else()
    set(CMODULE_LINK_FLAGS " ")
endif()

if (WIN32)
    set(TESTU01_INCLUDE C:/C_PROG/testu01/include D:/C_PROG/testu01/include)
    set(TESTU01_LINK C:/C_PROG/testu01 D:/C_PROG/testu01)
    set(TESTU01_LIBS testu01)
else()
    # Designed for Ubuntu. Install the next packages before work:
    # libtestu01-0:amd64 (contains the compiled TestU01 library)
    # libtestu01-0-dev-common (contains the TestU01 header files)
    set(TESTU01_INCLUDE /usr/include/testu01)
    set(TESTU01_LINK /usr/lib/x86_64-linux-gnu)
    set(TESTU01_LIBS testu01 testu01probdist testu01mylib)
endif()

# Some CMake settings
set(CMAKE_VERBOSE_MAKEFILE TRUE)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Check if math library is required to link
# It is a workaround for different behaviour of GCC and MSVC
# GCC in GNU/Linux cannot work without -lm key, MSVC
# cannot work WITH it, MinGW tolerates both variants
find_library(M_LIB m)

# Source files names
add_library(testu01threads
    include/testu01th/batteries.h
    include/testu01th/bigcrush.h      src/bigcrush.cpp
    include/testu01th/crush.h         src/crush.cpp
    include/testu01th/dummy_module.h  src/dummy_module.c
    include/testu01th/entropy.h       src/entropy.cpp
    include/testu01th/generators.h    src/generators.cpp 
    include/testu01th/smallcrush.h    src/smallcrush.cpp
    include/testu01th/speedtest.h     src/speedtest.cpp
    include/testu01th/pdiehard.h      src/pdiehard.cpp 
    include/testu01th/testu01_mt.h    src/testu01_mt.cpp
    include/testu01th/cinterface.h    src/cinterface.cpp)
target_include_directories(testu01threads PRIVATE ${TESTU01_INCLUDE} include)
target_link_directories(testu01threads PRIVATE ${TESTU01_LINK})

# Executables that call TestU01-threads library
add_executable(testu01th_run src/testu01th_run.cpp)
target_include_directories(testu01th_run PRIVATE include)
add_executable(testu01th_demo src/testu01th_demo.cpp)
target_include_directories(testu01th_demo PRIVATE include)
add_executable(testu01th_pipes src/testu01th_pipes.cpp)
target_include_directories(testu01th_pipes PRIVATE include)

# SplitMix generator in two variants: that calls TestU01-threads
# and that is called by TestU01-threads as an external module
add_executable(splitmix_exec
    src/splitmix_exec.c src/splitmix_gen.c include/testu01th/splitmix_gen.h)
target_include_directories(splitmix_exec PRIVATE include)
add_library(splitmix_shared SHARED generators/splitmix_shared.c
    src/splitmix_gen.c include/testu01th/splitmix_gen.h include/testu01th/cinterface.h)

# Examples of plugins, i.e. modules with PRNGs
foreach(prng_name alfib_lux alfib_mod alfib alfib8 ara32
    biski16
    biski16_alt
    biski32
    biski32_alt
    chacha chacha_avx coveyou64 cmwc4096 cmwc4827 cswb4288_64 cswb4288 flea32x1
    hicg64 hc256
    isaac64
    jkiss jkiss32
    kiss64 kiss93 kiss99 kiss4691
    kiss11_32
    lcg32prime lcg64 lcg64prime lcg64prime_il lcg128 lcg69069 lfib_ranmar lfib4
    lfsr113 lfsr258 lrnd64_255 lrnd64_1023
    melg607 melg19937 melg44497
    minstd mixmax mrg32k3a msws mularx64_u32 mulberry32 mwc32x
    mwc32xxa8 mwc40xxa8 mwc48xxa16
    mwc64_2p58
    mwc64 mwc64x mwc128 mwc128x mwc128xxa32 mwc1616 mwc1616x mwc4691
    mwc8222
    mlfib17_5
    pcg32 pcg64 philox philox32 r1279 randu ranrot32 ranval rc4 rc4ok rrmxmx # philox2x32
    sapparot sapparot2
    seed64 sezgin63 sfc64 shr3 speck128 speck128_avx splitmix32 splitmix32cbc
    stormdrop stormdrop_old
    swblux swbw swblarge
    squares64 sqxor sqxor32 superduper64 superduper96 superduper73 taus88 tinymt32 threefry v3b wich1982 wich2006 wyrand
    ultra
    xabc8 xabc16 xabc32 xkiss8_awc xkiss16_awc xkiss16sh_awc
    xkiss32_awc xkiss32sh_awc
    xoroshiro32pp xoroshiro64pp xoroshiro64st
    xorshift64st xoroshiro128 xoroshiro128p xoroshiro128stst xoroshiro1024st
    xorshift128 xorshift128p xorshift128rp xoshiro128p xoshiro128pp xorwow xsh ziff98)
    add_library(${prng_name}_shared SHARED generators/${prng_name}_shared.c include/testu01th/cinterface.h)
    set_target_properties(${prng_name}_shared PROPERTIES LINK_FLAGS ${CMODULE_LINK_FLAGS})
    set_target_properties(${prng_name}_shared PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/generators)
    set_target_properties(${prng_name}_shared PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/generators)
    target_include_directories(${prng_name}_shared PRIVATE include)
endforeach()

# Module for dummy PRNG
add_library(dummy_shared SHARED generators/dummy_shared.c src/dummy_module.c include/testu01th/cinterface.h)
set_target_properties(dummy_shared PROPERTIES LINK_FLAGS ${CMODULE_LINK_FLAGS})

# Module for RANLUX++ PRNG
add_library(ranluxpp_shared SHARED generators/ranluxpp_shared.c
    generators/ranluxpp_mulmod.h generators/ranluxpp_helpers.h include/testu01th/cinterface.h)
set_target_properties(ranluxpp_shared PROPERTIES LINK_FLAGS ${CMODULE_LINK_FLAGS})

# Module and test for MT19937
add_library(mt19937_shared SHARED generators/mt19937_shared.cpp include/testu01th/cinterface.h)


foreach(prng_name dummy ranluxpp mt19937 splitmix)
    set_target_properties(${prng_name}_shared PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/generators)
    set_target_properties(${prng_name}_shared PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/generators)
    target_include_directories(${prng_name}_shared PRIVATE include)
endforeach()



# Link executables
foreach (program testu01th_demo testu01th_run testu01th_pipes splitmix_exec)
    if(M_LIB)
        target_link_libraries (${program})
    endif()
    target_link_libraries(${program} testu01threads)
    target_include_directories(${program} PRIVATE ${TESTU01_INCLUDE})
    target_link_directories(${program} PRIVATE ${TESTU01_LINK})
    target_link_libraries(${program} ${TESTU01_LIBS})
    if (WIN32)
        target_link_libraries(${program} ws2_32)
    endif()
endforeach (program)
