/* gcc -nostdlib -shared lfib_vfloat_shared.c -o lfib_vfloat_shared.dll */
#include "testu01_mt_cintf.h"
#include <stdio.h>
#include <stdlib.h>

#define LFIB_A 10
#define LFIB_B 7
// 17, 5

static uint32_t global_seed;

static const double c = 5566755282872655.0 / 9007199254740992.0; /**< shift */
static const double r = 9007199254740881.0 / 9007199254740992.0; /**< base (prime) */

static inline double amb_mod_r(double a, double b)
{
    double x = a - b;
    return (x >= 0.0) ? x : (x + r);
}

typedef struct {
    double U[LFIB_A]; /**< Buffer for pseudorandom number */
    double z[LFIB_A];
    double w[LFIB_A];
    size_t up_ind[LFIB_A / LFIB_B + 2];
    size_t low_ind[LFIB_A / LFIB_B + 2];
    size_t pos;
    
} LFibFloat;

//////////////////////////////////////////
///// Begin of Windows-specific part /////
//////////////////////////////////////////
/*
#include <windows.h>
int DllMainCRTStartup(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    (void) hinstDLL;
    (void) fdwReason;
    (void) lpvReserved;
    return TRUE;
}
*/

static void *malloc_util(size_t len)
{
    return malloc(len);
}

static void free_util(void *ptr)
{
    free(ptr);
}




////////////////////////////////////////
///// End of Windows-specific part /////
////////////////////////////////////////

/*
static void *malloc_util(size_t len)
{
    return malloc(len);
}

static void free_util(void *ptr)
{
    free(ptr);
}
*/





void EXPORT fill_u01(void *param, void *state)
{
    LFibFloat *obj = (LFibFloat *) state;
    (void) param;

    /* Phase 1: calculate elements X_{A-B},...,X_{A} that depend
       on the previous content of the buffer */
    for (size_t i = LFIB_A - 1; i >= LFIB_A - LFIB_B; i--) {
        obj->U[i] -= obj->U[i - (LFIB_A - LFIB_B)];
    }    
    for (size_t i = LFIB_A - 1; i >= LFIB_A - LFIB_B; i--) {
        if (obj->U[i] < 0.0) obj->U[i] += 1.0;
    }
    /* Phase 2: calculate elements that depend on the elements
       calculate at Phase 1 */
    for (size_t k = 0; obj->up_ind[k] != 0; k++) {
        size_t up_ind = obj->up_ind[k];
        size_t low_ind = obj->low_ind[k];
        for (size_t i = low_ind, j = low_ind + LFIB_B; i <= up_ind; i++, j++) {
            obj->U[i] -= obj->U[j];
        }
        for (size_t i = low_ind, j = low_ind + LFIB_B; i <= up_ind; i++, j++) {
            if (obj->U[i] < 0.0) obj->U[i] += 1.0;
        }
    }

    for (size_t k = 0; k < LFIB_A; k++) {
        obj->z[k] -= c;
        if (obj->z[k] < 0.0) obj->z[k] += r;
    }

    for (size_t k = 0; k < LFIB_A; k++) {
        obj->w[k] -= obj->z[k];
        if (obj->w[k] < 0.0) obj->w[k] += r;
    }

    for (size_t k = 0; k < LFIB_A; k++) {
        obj->U[k] = obj->U[k] - obj->w[k];
    }

    for (size_t k = 0; k < LFIB_A; k++) {
        if (obj->U[k] < 0.0) obj->U[k] += 1.0;
    }
}


static double get_u01(void *param, void *state)
{
    LFibFloat *obj = (LFibFloat *) state;
    (void) param;

    /* Subtractive Lagged Fibbonaci part */
    double x = obj->U[obj->pos++];
    if (obj->pos == LFIB_A) {
        fill_u01(param, state);
        obj->pos = 0;
    }
    return x;
    
/*
    double x = obj->U[obj->i] - obj->U[obj->j];
    if (x < 0.0) x += 1.0;
    obj->U[obj->i] = x;
    if(--obj->i == 0) obj->i = LFIB_A;
	if(--obj->j == 0) obj->j = LFIB_A;
*/
    /* Nested Weyl sequence part */
/*
    obj->z = amb_mod_r(obj->z, c);
    obj->w = amb_mod_r(obj->w, obj->z);
    obj->w2 = amb_mod_r(obj->w2, obj->w);
*/
    /* Combine two generators */
/*
    x -= obj->w2;
    return (x < 0.0) ? (x + 1.0) : x;
*/
}

static long unsigned int get_bits32(void *param, void *state)
{
    const double m_2_pow_32 = 4294967296.0;
    return get_u01(param, state) * m_2_pow_32;
}


static void fill_wz(LFibFloat *obj, uint32_t seed)
{
    obj->z[0] = (double) seed / UINT_MAX;
    obj->w[0] = obj->z[0];
    for (size_t k = 1; k < LFIB_A; k++) {
        obj->z[k] = amb_mod_r(obj->z[k - 1], c);
        obj->w[k] = amb_mod_r(obj->w[k - 1], obj->z[k]);
    }
}

static void *init_state()
{
    LFibFloat *obj = (LFibFloat *) malloc_util(sizeof(LFibFloat));
    fill_wz(obj, global_seed);
//    uint32_t seed = global_seed;
//    obj->z = (double) seed / UINT_MAX;
//    obj->w = obj->z;
    //double w2 = obj->z, w3 = obj->z;
    for (size_t k = 0; k < LFIB_A; k++) {
        //obj->z = amb_mod_r(obj->z, c);
        //obj->w = amb_mod_r(obj->w, obj->z);
        //w2 = amb_mod_r(w2, obj->w);
        //w3 = amb_mod_r(w3, w2);
        obj->U[k] = obj->w[k];
    }
    obj->pos = LFIB_A - 1;

    /* Index boundaries for phase 2 of generator work */
    int pos = 0;
    for (int i = LFIB_A - LFIB_B - 1; i >= 0; i -= LFIB_B, pos++) {
        int j = i - LFIB_B + 1;
        if (j < 0) j = 0;
        obj->up_ind[pos] = i;
        obj->low_ind[pos] = j;        
    }
    obj->up_ind[pos] = 0;
    obj->low_ind[pos] = 0;

    return (void *) obj;
}

static void delete_state(void *param, void *state)
{
    (void) param;
    free_util(state);
}

int EXPORT gen_initlib(uint64_t seed, void *data)
{
    global_seed = seed;
    (void) data;
    return 1;
}

int EXPORT gen_closelib()
{
    return 1;
}

int EXPORT gen_getinfo(GenInfoC *gi)
{
    static const char name[] = "LFib_float";
    gi->name = name;
    gi->init_state = init_state;
    gi->delete_state = delete_state;
    gi->get_u01 = get_u01;
    gi->get_bits32 = get_bits32;
    return 1;
}
