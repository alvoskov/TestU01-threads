cmake_minimum_required (VERSION 3.5)
project (TestU01-threads VERSION 0.1 LANGUAGES CXX C)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Compiler settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 99)
if (NOT MSVC)
    add_compile_options(-O2 -Wall -Werror -Wextra -Wconversion -Wshadow -Wno-attributes -march=native)
    #add_compile_options(-g -pg)
    if (WIN32)
        add_compile_options(-static-libstdc++ -static-libgcc)
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
set(CMAKE_VERBOSE_MAKEFILE FALSE)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Check if math library is required to link
# It is a workaround for different behaviour of GCC and MSVC
# GCC in GNU/Linux cannot work without -lm key, MSVC
# cannot work WITH it, MinGW tolerates both variants
find_library(M_LIB m)

# Source files names
add_library(testu01threads STATIC
    include/testu01th/basictypes.h
    include/testu01th/batteries.h
    include/testu01th/bigcrush.h          src/bigcrush.cpp
    include/testu01th/crush.h             src/crush.cpp
    include/testu01th/generators.h        src/generators.cpp 
    include/testu01th/smallcrush.h        src/smallcrush.cpp
    include/testu01th/pdiehard.h          src/pdiehard.cpp 
    include/testu01th/testu01_callbacks.h src/testu01_callbacks.cpp
    include/testu01th/testu01_mt.h        src/testu01_mt.cpp
    include/testu01th/testu01_plugin.h    src/testu01_plugin.cpp)
set_property(TARGET testu01threads PROPERTY POSITION_INDEPENDENT_CODE ON)

# testu01threads core library
target_include_directories(testu01threads PRIVATE ${TESTU01_INCLUDE} include)
target_link_directories(testu01threads PUBLIC ${TESTU01_LINK})
target_link_libraries(testu01threads PRIVATE ${TESTU01_LIBS} $<$<PLATFORM_ID:Windows>:ws2_32>)

# Shared library: plugin for SmokeRand test suite
add_library(testu01th_sr_ext SHARED src/testu01_sr_ext.cpp)
target_include_directories(testu01th_sr_ext PRIVATE include)
target_link_libraries(testu01th_sr_ext testu01threads)

# Link executables
foreach (program testu01th_demo testu01th_pipes example)
    add_executable(${program} src/${program}.cpp)
    target_include_directories(${program} PRIVATE include)
    target_link_libraries(${program} testu01threads)
endforeach (program)
