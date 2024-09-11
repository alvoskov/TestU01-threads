#ifndef __SPLITMIX_GEN_H
#define __SPLITMIX_GEN_H

#include "cinterface.h"

typedef struct {
    uint64_t x;
} SplitMixState;

#ifdef __cplusplus
extern "C" {
#endif
int splitmix_get_geninfo(GenInfoC *gi);
#ifdef __cplusplus
}
#endif

#endif
