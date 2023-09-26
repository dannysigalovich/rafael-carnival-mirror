

#ifndef LOGGER_LOGGER_H
#define LOGGER_LOGGER_H

#include "FSI/LittleFS/lfs.h"
#include "FSI/fsi.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#define MAX_LOG_MESSAGE_SIZE 256


typedef enum {
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,
    LOG_CRITICAL
} log_level_t;


void Logger_Init(void);

void logger_test(void);

void log_info(const char *message, ...);

void log_warning(const char *message, ...);

void log_error(const char *message, ...);

void log_critical(const char *message, ...);

void close_logger(void);

int read_log(char* buff, uint32_t size);

#endif /* LOGGER_LOGGER_H */
