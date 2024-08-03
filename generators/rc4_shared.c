/**
 * @file rc4_shared.c
 * @brief Implementation of RC4 CSPRNG (obsolete algorithm). 
 * @details This algorithm passes TestU01 BigCrush but fails practrand
 * on 1 TB of data.
 *
 * 1. Press W.H., Teukolsky S.A., Vetterling W.T., Flannery B.P.
 *    Numerical recipes. The Art of Scientific Computing. Third Edition.
 *    2007. Cambridge University Press. ISBN 978-0-511-33555-6.
 * 2. Sleem L., Couturier R. TestU01 and Practrand: Tools for a randomness
 *    evaluation for famous multimedia ciphers. Multimedia Tools and
 *    Applications, 2020, 79 (33-34), pp.24075-24088. ffhal-02993846f
 * 3. Khovayko O., Schelkunov D. RC4OK. An improvement of the RC4 stream
 *    cipher // Cryptology ePrint Archive, Paper 2023/1486.
 *    https://eprint.iacr.org/2023/1486
 */
#include "testu01_mt_cintf.h"

/////////////////////////////////////////////////
///// Entry point for -nostdlib compilation /////
/////////////////////////////////////////////////
SHARED_ENTRYPOINT_CODE

static CallerAPI intf;

typedef struct {
    uint8_t s[256];
    uint8_t i;
    uint8_t j;
} RC4State;

unsigned long EXPORT get_bits32(void *param, void *state)
{
    RC4State *obj = (RC4State *) state;
    uint32_t v = 0;
    uint8_t *s = obj->s, i = obj->i, j = obj->j;
    (void) param;    
    for (size_t k = 0; k < 4; k++) {
        uint8_t ss = s[++i];
        j += ss;
        s[i] = s[j];
        s[j] = ss;
        uint8_t u = s[i] + s[j];
        v = (v << 8) | s[u];
    }
    obj->i = i; obj->j = j;
    return (unsigned long) v;
}

static double get_u01(void *param, void *state)
{
    return uint32_to_udouble(get_bits32(param, state));
}

static void *init_state()
{
    RC4State *obj = (RC4State *) intf.malloc(sizeof(RC4State));
    uint64_t v = UINT64_C(0x9E3779B97F4A7C15) ^ intf.get_seed64();
    for (size_t i = 0; i < 256; i++) {
        obj->s[i] = i;
    }
    for (size_t i = 0, j = 0; i < 256; i++) {
        uint8_t ss = obj->s[i];
        j = (j + ss + (v >> 56)) & 0xFF;
        obj->s[i] = obj->s[j];
        obj->s[j] = ss;
        v = (v << 56) | (v >> 8);
    }
    obj->i = 0;
    obj->j = 0;
    for (size_t k = 0; k < 64; k++) {
        (void) get_bits32(NULL, (void *) obj);
    }
    return (void *) obj;
}

static void delete_state(void *param, void *state)
{
    (void) param;
    intf.free(state);
}

int EXPORT gen_initlib(CallerAPI *intf_)
{
    intf = *intf_;
    return 1;
}

int EXPORT gen_closelib()
{
    return 1;
}

int EXPORT gen_getinfo(GenInfoC *gi)
{
    static const char name[] = "RC4";
    gi->name = name;
    gi->init_state = init_state;
    gi->delete_state = delete_state;
    gi->get_u01 = get_u01;
    gi->get_bits32 = get_bits32;
    return 1;
}
