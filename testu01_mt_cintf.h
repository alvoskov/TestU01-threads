/**
 * @file testu01th_lib.h
 * @brief Simple C-style interface that enables an interaction between
 * TestU01-threads library and C programs.
 * @details Two styles of interactions are supported:
 *
 * - Calling SmallCrush, Crush and BigCrush test batteries from C program.
 * - Calling PRNG from shared library written in pure C.
 * 
 * This file is designed for both C and C++ programs. Please, don't add
 * anything C++-specific here!
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

#ifndef __TESTU01_MT_CINTF
#define __TESTU01_MT_CINTF
#include <stdint.h>

typedef double (*GetU01CallbackC)(void *param, void *state);
typedef long unsigned int (*GetBits32CallbackC)(void *param, void *state);
typedef void *(*InitStateCallbackC)();
typedef void (*DeleteStateCallbackC)(void *param, void *state);

/**
 * @brief Keeps the information for initialization and destruction
 * of PRNG objects. Can be considered as UniformGenerator class equivalent
 * for pure C.
 */
typedef struct {
    const char *name;
    InitStateCallbackC init_state;
    DeleteStateCallbackC delete_state;
    GetU01CallbackC get_u01;
    GetBits32CallbackC get_bits32;
} GenInfoC;

#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64) || defined(__MINGW32__) || defined(__MINGW64__)
#define EXPORT __declspec( dllexport )
#define USE_LOADLIBRARY
#else
#define EXPORT
#endif


typedef int (*GenInitLibFunc)();
typedef int (*GenCloseLibFunc)();
typedef int (*GenGetInfoFunc)(GenInfoC *gi);

/**
 * @brief Keeps pointers to the external C module functions, required
 * to work with PRNG made as DLL extensions.
 */
typedef struct {
    GenInitLibFunc gen_initlib;
    GenCloseLibFunc gen_closelib;
    GenGetInfoFunc gen_getinfo;
} GenCModule;

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief Conversion of unsigned (pseudorandom) 64-bit integer
 * to the double that belongs to the [0;1) interval.
 */
static inline double uint64_to_udouble(uint64_t val)
{
    union {
        uint64_t i;
        double f;
    } x;
    x.i = val;
    x.i = (x.i >> 12) | 0x3ff0000000000000;
    x.f -= 1.0;
    return x.f;
}
double uint64_to_udouble(uint64_t val);
void set_generator(const GenInfoC *gi);
int run_smallcrush();
int run_crush();
int run_bigcrush();
#ifdef __cplusplus
}
#endif

#endif
