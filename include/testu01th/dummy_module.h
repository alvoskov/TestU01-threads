/**
 * @file dummy_module.h
 * @brief An implementation of "dummy" PRNG that always returns 0.
 * It is used only for performance measurements!
 */
#ifndef __DUMMY_MODULE_H
#define __DUMMY_MODULE_H
#include "testu01th/cinterface.h"

#ifdef __cplusplus
extern "C" {
#endif

int dummy_gen_getinfo(GenInfoC *gi);
GenCModule dummy_init_cmodule();

#ifdef __cplusplus
}
#endif

#endif

