#ifndef __LOG_H__
#define __LOG_H__


#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    LOG_DBG,
    LOG_SUCCESS,
    LOG_WARN,
    LOG_ERR,
    LOG_LEVEL_LEN
} LOG_LEVEL;

void log_info(LOG_LEVEL level, const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif // __LOG_H__
