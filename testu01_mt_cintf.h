/**
 * @file testu01th_lib.h
 * @brief Simple C-style interface that enables an interaction between
 * TestU01-threads library and C programs.
 *
 * @details Two styles of interactions are supported:
 *
 * - Calling SmallCrush, Crush and BigCrush test batteries from C program.
 * - Calling PRNG from shared library written in pure C.
 *
 * If PRNG is compiled as a separate shared C library it should export
 * the next three functions:
 *
 * - `int EXPORT gen_initlib()` (initialized the library after loading)
 * - `int EXPORT gen_closelib()` (run before unloading the library)
 * - `int EXPORT gen_getinfo(GenInfoC *gi)` (fills GenInfoC structure
 *   with function pointers for the used PRNG)
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
#include <time.h>

typedef double (*GetU01CallbackC)(void *param, void *state);
typedef long unsigned int (*GetBits32CallbackC)(void *param, void *state);
typedef uint64_t (*GetBits64CallbackC)(void *param, void *state);
typedef void (*GetArray32CallbackC)(void *param, void *state, uint32_t *out, size_t len);
typedef void (*GetArray64CallbackC)(void *param, void *state, uint64_t *out, size_t len);
typedef uint32_t (*GetSum32CallbackC)(void *param, void *state, size_t len);
typedef uint64_t (*GetSum64CallbackC)(void *param, void *state, size_t len);
typedef void *(*InitStateCallbackC)(void);
typedef void (*DeleteStateCallbackC)(void *param, void *state);
typedef uint64_t (*GetSeed64CallbackC)(void);
typedef int (*SelfTestCallbackC)(void);

/**
 * @brief Keeps the information for initialization and destruction
 * of PRNG objects. Can be considered as UniformGenerator class equivalent
 * for pure C.
 */
typedef struct {
    const char *name; /**< Generator name */
    const char *options; /**< Generator options */
    InitStateCallbackC init_state; /**< Function that initializes the PRNG state */
    DeleteStateCallbackC delete_state; /**< Function that deletes the PRNG state */
    GetU01CallbackC get_u01; /**< Function returns the double pseudorandom number */
    GetBits32CallbackC get_bits32; /**< Function returns the uint32_t pseudorandom number */
    GetBits64CallbackC get_bits64; /**< Function returns the uint64_t pseudorandom number */
    GetArray32CallbackC get_array32; /**< Functions fills the uint32_t array buffer with pseudorandom numbers */
    GetArray64CallbackC get_array64; /**< Functions fills the uint64_t array buffer with pseudorandom numbers */
    GetSum32CallbackC get_sum32; /**< Function that returns the sum of 32-bit pseudorandom numbers */
    GetSum64CallbackC get_sum64; /**< Function that returns the sum of 64-bit pseudorandom numbers */
    SelfTestCallbackC run_self_test; /**< Functions that runs the internal self-test */
} GenInfoC;

/**
 * @brief Keeps pointers to the caller API that are used for PRNG modules.
 * Some functions are intentionally duplicating the standard library to
 * allow writing freestanding modules.
 */
typedef struct {
    GetSeed64CallbackC get_seed64; /**< Get random 64-bit seed */
    void *(*malloc)(size_t len); /**< Pointer to malloc function */
    void (*free)(void *); /**< Pointer to free function */
    int (*printf)(const char *format, ... ); /**< Pointer to printf function */
    int (*strcmp)(const char *lhs, const char *rhs); /**< Pointer to strcmp function */
} CallerAPI;

#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64) || defined(__MINGW32__) || defined(__MINGW64__)
#include <windows.h>
#define EXPORT __declspec( dllexport )
#define USE_LOADLIBRARY
#ifndef __cplusplus
#define SHARED_ENTRYPOINT_CODE \
int DllMainCRTStartup(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) { \
    (void) hinstDLL; (void) fdwReason; (void) lpvReserved; return TRUE; }
#else
#define SHARED_ENTRYPOINT_CODE
#endif
#else
#define EXPORT
#define SHARED_ENTRYPOINT_CODE
#endif


typedef int (*GenInitLibFunc)(CallerAPI *intf);
typedef int (*GenCloseLibFunc)(void);
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


void GenInfoC_init(GenInfoC *obj);

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


/**
 * @brief Conversion of unsigned (pseudorandom) 32-bit integer
 * to the double that belongs to the [0;1) interval.
 */
static inline double uint32_to_udouble(uint32_t val)
{
    static const double INV32 = 2.3283064365386963E-10;
    return val * INV32;
}



#if defined(__GNUC__) && defined(__x86_64__)
#include <x86intrin.h>
#undef STDC_HEADERS
#endif


/**
 * @brief pcg_rxs_m_xs64 PRNG that has a good quality and can be used
 * for initialization for other PRNGs such as lagged Fibbonaci.
 */
static inline uint64_t pcg_bits64(uint64_t *state)
{
    uint64_t word = ((*state >> ((*state >> 59) + 5)) ^ *state) *
        12605985483714917081ull;
    *state = *state * 6364136223846793005ull + 1442695040888963407ull;
    return (word >> 43) ^ word;
}


void set_generator(const GenInfoC *gi);
int run_smallcrush();
int run_crush();
int run_bigcrush();

/*
 * External modules functions prototypes. They are needed for
 * protection from function types mismatchin
 */
int gen_initlib(CallerAPI *intf);
int gen_closelib(void);
int gen_getinfo(GenInfoC *gi);


/**
 * @brief Default prolog of PRNG C module that contains default entry point
 * and default `gen_initlib` and `gen_closelib` functions.
 */
#define PRNG_CMODULE_PROLOG SHARED_ENTRYPOINT_CODE /* Entry point for -nostdlib compilation */ \
static CallerAPI intf; /* Keeps pointers to caller API functions */ \
int EXPORT gen_initlib(CallerAPI *intf_) \
{ \
    intf = *intf_; \
    return 1; \
} \
int EXPORT gen_closelib() \
{ \
    return 1; \
}


/**
 * @brief Some default boilerplate code for scalar PRNG that returns
 * unsigned 32-bit numbers. Requires the next functions to be defined:
 *
 * - `static inline unsigned long get_bits32_raw(void *param, void *state);`
 * - `static void *init_state();`
 *
 * It also relies on default prolog (intf static variable, some exports etc.),
 * see PRNG_CMODULE_PROLOG
 */
#define MAKE_UINT32_PRNG(prng_name, selftest_func) \
EXPORT long unsigned int get_bits32(void *param, void *state) { \
    return get_bits32_raw(param, state) & 0xFFFFFFFF; \
} \
EXPORT double get_u01(void *param, void *state) { \
    return uint32_to_udouble(get_bits32_raw(param, state)); \
} \
EXPORT void get_array32(void *param, void *state, uint32_t *out, size_t len) { \
    for (size_t i = 0; i < len; i++) \
        out[i] = get_bits32_raw(param, state); \
} \
EXPORT uint32_t get_sum32(void *param, void *state, size_t len) { \
    uint32_t sum = 0; \
    for (size_t i = 0; i < len; i++) \
        sum += get_bits32_raw(param, state); \
    return sum; \
} \
static void delete_state(void *param, void *state) {\
    (void) param; intf.free(state); \
} \
int EXPORT gen_getinfo(GenInfoC *gi) { \
    gi->name = prng_name; \
    gi->init_state = init_state; \
    gi->delete_state = delete_state; \
    gi->get_u01 = get_u01; \
    gi->get_bits32 = get_bits32; \
    gi->get_array32 = get_array32; \
    gi->get_sum32 = get_sum32; \
    gi->run_self_test = selftest_func; \
    return 1; \
}


/**
 * @brief Some default boilerplate code for scalar PRNG that returns
 * unsigned 64-bit numbers. Requires the next functions to be defined:
 *
 * - `static inline unsigned long get_bits64_raw(void *param, void *state);`
 * - `static void *init_state();`
 *
 * It also relies on default prolog (intf static variable, some exports etc.),
 * see PRNG_CMODULE_PROLOG
 */
#define MAKE_UINT64_UPTO32_PRNG(prng_name, selftest_func) \
EXPORT uint64_t get_bits64(void *param, void *state) { \
    return get_bits64_raw(param, state); \
} \
EXPORT long unsigned int get_bits32(void *param, void *state) { \
    return get_bits64_raw(param, state) >> 32; \
} \
EXPORT double get_u01(void *param, void *state) { \
    return uint64_to_udouble(get_bits64_raw(param, state)); \
} \
EXPORT void get_array64(void *param, void *state, uint64_t *out, size_t len) { \
    for (size_t i = 0; i < len; i++) \
        out[i] = get_bits64_raw(param, state); \
} \
static void delete_state(void *param, void *state) {\
    (void) param; intf.free(state); \
} \
int EXPORT gen_getinfo(GenInfoC *gi) { \
    gi->name = prng_name; \
    gi->init_state = init_state; \
    gi->delete_state = delete_state; \
    gi->get_u01 = get_u01; \
    gi->get_bits32 = get_bits32; \
    gi->get_bits64 = get_bits64; \
    gi->get_array64 = get_array64; \
    gi->run_self_test = selftest_func; \
    return 1; \
}




#ifdef __cplusplus
}
#endif


#endif
