cmake_minimum_required (VERSION 3.5)
project (TestU01-threads)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Compiler settings
if (NOT MSVC)
    set(PROF_FLAGS "")
    set(CMODULE_LINK_FLAGS "-nostdlib")
    set(C_CXX_FLAGS "-O2 -Wall -Werror -Wextra -Wconversion -Wshadow -Wno-attributes -march=native")
    set(CMAKE_CXX_STANDARD 14)
    set(CMAKE_C_STANDARD 99)
    #set(PROF_FLAGS "-g -pg")
    if (WIN32)
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${PROF_FLAGS} ${C_CXX_FLAGS} -static-libstdc++ -static-libgcc")
        set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${PROF_FLAGS} -ffreestanding ${C_CXX_FLAGS}")
    else()
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${PROF_FLAGS} ${C_CXX_FLAGS}")
        set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${PROF_FLAGS} -ffreestanding ${C_CXX_FLAGS}")
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
    include/testu01th/bigcrush.h       src/bigcrush.cpp
    include/testu01th/crush.h          src/crush.cpp
    include/testu01th/generators.h     src/generators.cpp 
    include/testu01th/smallcrush.h     src/smallcrush.cpp
    include/testu01th/pdiehard.h       src/pdiehard.cpp 
    include/testu01th/testu01_mt.h     src/testu01_mt.cpp
    include/testu01th/testu01_plugin.h src/testu01_plugin.cpp)

# testu01threads core library
target_include_directories(testu01threads PRIVATE ${TESTU01_INCLUDE} include)
target_link_directories(testu01threads PRIVATE ${TESTU01_LINK})
target_link_libraries(${program} testu01threads)

# Shared library: plugin for SmokeRand test suite
add_library(testu01th_sr_ext SHARED src/testu01_sr_ext.cpp)
target_include_directories(testu01th_sr_ext PRIVATE ${TESTU01_INCLUDE} include)
target_link_directories(testu01th_sr_ext PRIVATE ${TESTU01_LINK})
target_link_libraries(testu01th_sr_ext ${TESTU01_LIBS} testu01threads)
if (WIN32)
    target_link_libraries(testu01th_sr_ext ws2_32)
endif()

# Executables that call TestU01-threads library
add_executable(testu01th_demo src/testu01th_demo.cpp)
target_include_directories(testu01th_demo PRIVATE include)
add_executable(testu01th_pipes src/testu01th_pipes.cpp)
target_include_directories(testu01th_pipes PRIVATE include)

# Link executables
foreach (program testu01th_demo testu01th_pipes)
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
