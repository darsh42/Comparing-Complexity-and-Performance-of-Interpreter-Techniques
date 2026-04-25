#include "benchmark.h"

HOOK ENTER_PROCESSING(void)  { 
    __asm__ __volatile__ (""); 
}
HOOK EXIT_PROCESSING(void)   { 
    __asm__ __volatile__ (""); 
}
HOOK ENTER_DISPATCHING(void) { 
    __asm__ __volatile__ (""); 
}
HOOK EXIT_DISPATCHING(void)  { 
    __asm__ __volatile__ (""); 
}
