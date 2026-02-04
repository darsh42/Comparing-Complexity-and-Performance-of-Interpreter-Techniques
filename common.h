#ifndef __COMMON_H__
#define __COMMON_H__

#ifndef __MACRO_EXPANSION__
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdatomic.h>

#include <elf.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>
#endif // __MACRO_EXPANSION__

#define CACHELINE_SIZE 64

#define U32MAX  0xffffffff
#define S32MIN ((s64)-2147483648LL)

typedef  int64_t s64;
typedef  int32_t s32;
typedef  int16_t s16;
typedef   int8_t  s8;

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef  uint8_t  u8;


#endif // __COMMON_H__
