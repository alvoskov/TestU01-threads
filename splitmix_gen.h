#ifndef __SPLITMIX_GEN_H
#define __SPLITMIX_GEN_H

#include "testu01_mt_cintf.h"

typedef struct {
    uint64_t x;
} SplitMixState;

#ifdef __cplusplus
extern "C" {
#endif
void splitmix_get_geninfo(GenInfoC *gi);
#ifdef __cplusplus
}
#endif

#endif
