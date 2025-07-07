/**
 * @details
 * 1. https://sourceforge.net/p/gjrand/discussion/446985/thread/3f92306c58/
 * 2. https://gist.github.com/imneme/7a783e20f71259cc13e219829bcea4ac
 */
#include "testu01th/cinterface.h"

PRNG_CMODULE_PROLOG

typedef struct {
    uint32_t a;
    uint32_t b;
    uint32_t c;
    uint32_t d;
} Gjrand32State;


static unsigned long get_bits32_raw(void *param, void *state)
{
    Gjrand32State *obj = state;
    (void) param;
	obj->b += obj->c; obj->a = rotl32(obj->a, 16); obj->c ^= obj->b;
	obj->d += 0x96a5;
	obj->a += obj->b; obj->c = rotl32(obj->c, 11); obj->b ^= obj->a;
	obj->a += obj->c; obj->b = rotl32(obj->b, 19); obj->c += obj->a;
	obj->b += obj->d;
	return obj->a;
}


static void Gjrand32State_init(Gjrand32State *obj, uint32_t seed)
{
    obj->a = seed;
    obj->b = 0;
    obj->c = 2000001;
    obj->d = 0;
    for (int i = 0; i < 14; i++) {
        (void) get_bits32_raw(NULL, obj);
    }
}


static void *init_state()
{
    Gjrand32State *obj = intf.malloc(sizeof(Gjrand32State));
    Gjrand32State_init(obj, intf.get_seed64());
    return obj;
}

MAKE_UINT32_PRNG("gjrand32", NULL)
