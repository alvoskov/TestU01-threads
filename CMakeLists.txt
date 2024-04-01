cmake_minimum_required (VERSION 3.5)
project (TestU01-threads)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Compiler settings
if (NOT MSVC)
    set(PROF_FLAGS "")
    #set(PROF_FLAGS "-g -pg")
    if (WIN32)
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${PROF_FLAGS} -std=gnu++14 -O3 -march=haswell -Wall -Werror -Wextra -Wno-attributes -static-libstdc++ -static-libgcc")
    else()
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${PROF_FLAGS} -std=gnu++14 -O3 -march=haswell -Wall -Werror -Wextra -Wno-attributes")
    endif()
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
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Check if math library is required to link
# It is a workaround for different behaviour of GCC and MSVC
# GCC in GNU/Linux cannot work without -lm key, MSVC
# cannot work WITH it, MinGW tolerates both variants
find_library(M_LIB m)

# Source files names
add_library(testu01threads testu01_mt.h testu01_mt_cintf.h batteries.h bigcrush.h crush.h smallcrush.h pdiehard.h generators.h
    testu01_mt.cpp testu01_mt_cintf.cpp bigcrush.cpp crush.cpp smallcrush.cpp pdiehard.cpp generators.cpp)
target_include_directories(testu01threads PRIVATE ${TESTU01_INCLUDE})
target_link_directories(testu01threads PRIVATE ${TESTU01_LINK})

# Executables that call TestU01-threads library
add_executable(testu01th_demo testu01th_demo.cpp)
add_executable(testu01th_lib testu01th_lib.cpp)
add_executable(splitmix_exec splitmix_exec.c splitmix_gen.c splitmix_gen.h)

# Examples of plugins
add_library(splitmix_shared SHARED splitmix_shared.c splitmix_gen.c splitmix_gen.h testu01_mt_cintf.h)
add_library(kiss93_shared SHARED kiss93_shared.c testu01_mt_cintf.h)
add_library(mlfib17_5_shared SHARED mlfib17_5_shared.c testu01_mt_cintf.h)
add_library(alfib_shared SHARED alfib_shared.c testu01_mt_cintf.h)
add_library(lfib_float_shared SHARED lfib_float_shared.c testu01_mt_cintf.h)
add_library(lfib_vfloat_shared SHARED lfib_vfloat_shared.c testu01_mt_cintf.h)


foreach (program testu01th_demo testu01th_lib splitmix_exec)
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
