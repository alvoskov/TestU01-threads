/**
 * @brief RANLUX++ pseudorandom number generator. Taken from
 * ranluxpp-portable and ROOT Data Analysis Framework.
 * @details The implementation of the Linear Congruential Random Number
 * Generator with a large modulus defined by the recurrence:
 *
 * \f[
 *   x_{i+1} = x_{i} * A mod m
 * \f]
 *
 * where the recurrence parameters are based on the RANLUX generator:
 *
 * - the base \f$ b = 2^{24} \f$
 * - the modulus \f$ m = b^{24} - b^{10} + 1 = 2^{576} - 2^{240} + 1 \f$
 * - the multiplier A is a power of a -- \f$ A = a^p mod m \f$
 *
 * where 
 * 
 * \f[
 *   a = m - (m-1)/b
 *         = b^{24}  - b^{23}  - b^{10}  + b^{9}   + 1
 *         = 2^{576} - 2^{552} - 2^{240} + 2^{216} + 1
 * \f]
 * is the multiplicative inverse of the base \f$ b = 2^{24} \f$
 * i.e. a * b mod m = 1
 *
 * This PRNG passes SmallCrush, Crush and BigCrush test batteries.
 *
 * References:
 * 1. Alexei Sibidanov. A revision of the subtract-with-borrow random number
 *    generators // Computer Physics Communications. 2017. V. 221. P.299-303.
 *    https://doi.org/10.1016/j.cpc.2017.09.005
 * 2. Jonas Hahnfeld and Lorenzo Moneta. A Portable Implementation of RANLUX++.
 *    25th International Conference on Computing in High Energy and Nuclear
 *    Physics (CHEP 2021). V. 251. https://doi.org/10.1051/epjconf/202125103008
 *
 * @copyright (c) 2020-2021 Jonas Hahfeld, Jirka Hladky (original library);
 * (c) 2024 Alexey L. Voskov (modifications for TestU01-threads and MinGW)
 *
 * @license This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 * 
 * See ranluxpp_LICENSE.txt next to this file for the text of the LGPL version 2.1.
 * For questions regarding the ROOT license, please contact rootdev@cern.ch.
 */
#include "testu01_mt_cintf.h"
#include "ranluxpp_mulmod.h"

PRNG_CMODULE_PROLOG

///////////////////////////////////
///// RANLUX++ implementation /////
///////////////////////////////////

/**
 * @brief RANLUX++ internal state.
 */
typedef struct {
    uint64_t x[9]; ///< State vector.
    uint64_t A[9]; ///< Multiplier.
    int pos; ///< Output position for 64-bit output.
    union {
        uint64_t u64val; ///< To be splitted to two 32-bit values.
        uint32_t u32val[2]; ///< 32-bit values.
    } buf32; ///< Internal buffer for 32-bit outputs.
    int pos32; ///< Output position for 32-bit output.
} RanluxppState;

/**
 * @brief \f$ a = m - (m-1)/b = 2^{576} - 2^{552} - 2^{240} + 2^{216} + 1 \f$
 */
static const uint64_t initial_a[9] = {
    0x0000000000000001ull, 0x0000000000000000ull, 0x0000000000000000ull,
    0xffff000001000000ull, 0xffffffffffffffffull, 0xffffffffffffffffull,
    0xffffffffffffffffull, 0xffffffffffffffffull, 0xfffffeffffffffffull};

/**
 * @brief RANLUX++ initialization subroutine.
 * @details Seed the generator by jumping to the state
 * \f[ 
 *   x_{seed} = x_{0} * A^(2^{96} * seed) mod m
 * \f]
 * The scheme guarantees non-colliding sequences.
 * Set multiplier A: \f$ A = a^{p} mod m \f$.
 * Recommended p = 2048 (skipping 2048 values).
 */
void RanluxppState_init(RanluxppState *r, uint64_t seed, uint64_t p)
{
    r->x[0] = 1;
    for (int i = 1; i < 9; i++) r->x[i] = 0;
    for (int i = 0; i < 9; i++) r->A[i] = initial_a[i];
    powermod(r->A, r->A, p); // A = a^p mod m. Recommended p = 2048 (skipping!)
    // Seed the generator
    uint64_t a[9];
    powermod(r->A, a, 1ull << 48); powermod(a, a, 1ull << 48);
    powermod(a, a, seed);
    mulmod(a, r->x);
    r->pos = 0; // 64-bit values buffer is full
    r->pos32 = 2; // 32-bit values buffer is empty
}

/**
 * @brief Generate the next 64-bit values.
 * @details It is LCG -- modular multiplication.
 */
static inline void RanluxppState_next(RanluxppState *r)
{
    mulmod(r->A, r->x);
}

/////////////////////
///// Interface /////
/////////////////////

/**
 * @brief A basic function for construction of all types of output:
 * 64-bit, 32-bit, double. 
 */
static inline uint64_t get_bits64_raw(void *param, void *state)
{
    RanluxppState *obj = state;
    (void) param;
    if (obj->pos == 9) {
        RanluxppState_next(obj);
        obj->pos = 0;
    }
    return obj->x[obj->pos++];
}


static uint64_t get_bits64(void *param, void *state)
{
    return get_bits64_raw(param, state);    
}

/**
 * @brief Decomposes each 64-bit value into a pair of 32-bit
 * values.
 */
static long unsigned int get_bits32(void *param, void *state)
{
    RanluxppState *obj = state;
    if (obj->pos32 == 2) {
        obj->buf32.u64val = get_bits64_raw(param, state);
        obj->pos32 = 0;
    }
    return obj->buf32.u32val[obj->pos32++];
}

/**
 * @brief Returns floating point value. The algorithm is different
 * from the original RANLUX28/RANLUX48 and gives 52 bits in mantissa
 * (the lower bit is always 0).
 */
static double get_u01(void *param, void *state)
{
    return uint64_to_udouble(get_bits64_raw(param, state));
}

static double get_u01_from32(void *param, void *state)
{
    return uint32_to_udouble(get_bits32(param, state));
}


static void *init_state()
{
    RanluxppState *obj = intf.malloc(sizeof(RanluxppState));
    RanluxppState_init(obj, intf.get_seed64(), 2048);
    return (void *) obj;
}


static void delete_state(void *param, void *state)
{
    (void) param;
    intf.free(state);
}

/**
 * @brief An internal self-test based on values taken from
 * the `ranluxpp_p_2048_seed_1.output` file from the ranluxpp-portable
 * library.
 */
static int run_self_test()
{
    static const uint64_t x_ref[] = {
        0x9F1C67142C84C502, 0x024D94E3C4B490E8, 0xE9D460859F0659B6,
        0xD697D9321E8373B1, 0x1164275F61142884, 0xD644D1BD1837C737,
        0xAD4191BCF0926C6B, 0x2624A1B9EF2C42C0, 0xF671BBCEE85222AB,
        0xF20959A668A3FA5F, 0x0BD822C14908C4C1, 0xC19C29FBB45A6FD7,
        0xE640BCE7A53E141B, 0xC30C5EB51143F1F5, 0xD4947FBF9B07F171,
        0x0BE1DC0017B06A80, 0xD95429BF32A2EBF7, 0x3E042F1021F30C85,
        0x92AF8C6E15BBC6F2, 0x09557A493B2B69CC, 0xF152A492A46C3FC2,
        0xD057BDAFA5DF3CAF, 0x4113FD5E513D238E, 0xA37F5FDC9F2ED7BA,
        0x1DB8B904CCE203F5, 0x7E71DFBDBCECB939, 0xEEDBDECB9D89CD16,
        0x4C062ADEA10832A2, 0x4F3900A8E5AA5F93, 0xDD43A9A706955F7C,
        0xF09B8E28DFA41959, 0x54DAF09FFA230111, 0xF3F6C0AFA4699583,
        0xCF591031733FD865, 0x6D9CF804822A3149, 0x162B4DFF3E0EC818,
        0xE2267F795B7A0607, 0xB97C4BCD3BB34681, 0x5F775BE87747F1BB,
        0xBDF63EA476ECF296, 0x4E0F84968809B0E0, 0xA585B0177153FC3F,
        0x41674EEC2F9040F7, 0xEE19576A890A3FB7, 0xB0504528C2256B82
    };
    int passed = 1;
    RanluxppState *obj = intf.malloc(sizeof(RanluxppState));
    RanluxppState_init(obj, 1, 2048);
    for (size_t i = 0; i < sizeof(x_ref) / sizeof(x_ref[0]); i++) {
        uint64_t x = get_bits64_raw(NULL, obj);
        if (x != x_ref[i]) {
            passed = 0;
        }
    }
    intf.free(obj);
    return passed;
}

/**
 * @brief Initialization subroutine.
 * @details Two command line options are supported:
 *
 * - u32 -- use 32-bit integers for making doubles; it improves sensitivity
 *   of TestU01 to anomalies. Also don't create 64-bit output.
 * - u64 -- use 64-bit integers for making doubles, also creates 64-bit
 *   output. Improves quality of doubles, useful for PractRand.
 *
 * The default option is u64.
 */
int EXPORT gen_getinfo(GenInfoC *gi)
{
    gi->init_state = init_state;
    gi->delete_state = delete_state;
    gi->get_bits32 = get_bits32;
    if (!intf.strcmp(gi->options, "u32")) {
        gi->name = "RANLUX++:u32";
        gi->get_u01 = get_u01_from32;
    } else {
        gi->name = "RANLUX++:u64";
        gi->get_u01 = get_u01;
        gi->get_bits64 = get_bits64;
        // Warning in the case of unknown option.
        if (intf.strcmp(gi->options, "u64") &&
            intf.strcmp(gi->options, "")) {
            intf.printf("Warning: unknown option '%s'\n", gi->options);
        }
    }
    gi->run_self_test = run_self_test;
    return 1;
}
