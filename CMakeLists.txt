cmake_minimum_required (VERSION 3.5)
project (TestU01MT)
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
set (HEADERS testu01_mt.h bigcrush.cpp crush.cpp smallcrush.cpp generators.cpp)
set (SOURCES weyl_test.cpp testu01_mt.cpp bigcrush.h crush.h smallcrush.h generators.h)


add_executable(weyl_test ${HEADERS} ${SOURCES})
if(M_LIB)
    target_link_libraries (weyl_test)
endif()

target_include_directories(weyl_test PRIVATE C:/C_PROG/testu01/include D:/C_PROG/testu01/include)
target_link_directories(weyl_test PRIVATE C:/C_PROG/testu01 D:/C_PROG/testu01)
target_link_libraries(weyl_test testu01)
if (WIN32)
    target_link_libraries(weyl_test ws2_32)
endif()

