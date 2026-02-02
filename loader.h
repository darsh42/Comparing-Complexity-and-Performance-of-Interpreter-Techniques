#ifndef __LOADER_H__
#define __LOADER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "mips.h"
#include "memory.h"

void loader_elf(struct mips   *mips, 
                struct memory *memory, 
                const char *elffile);

#ifdef __cplusplus
}
#endif

#endif // __LOADER_H__
