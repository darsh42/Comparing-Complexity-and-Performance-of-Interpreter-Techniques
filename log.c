#include "log.h"

#include <stdio.h>
#include <stdarg.h>

char * type[LOG_LEVEL_LEN] = {
    "DEBUG",
    "SUCCESS",
    "WARN",
    "ERR"
}; 

char * colors[LOG_LEVEL_LEN] = {
    "\x1b[0m",
    "\x1b[32m",
    "\x1b[1;33m",
    "\x1b[31m"
};

void log_info(LOG_LEVEL level, const char * fmt, ...) {
#if 0
    va_list args;
    va_start(args, fmt);
    printf("[%s] ", type[level]); 
    vfprintf(stdout, fmt, args);
    printf("\n%s", colors[LOG_DBG]);
    va_end(args);
#endif
}
