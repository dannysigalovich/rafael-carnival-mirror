

#ifndef LOGGER_LOGGER_H
#define LOGGER_LOGGER_H

#include "FSI/LittleFS/lfs.h"
#include "FSI/fsi.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

#define MAX_LOG_MESSAGE_SIZE 256
#define MAX_LOG_FILE_NAME 20
#define MAX_LOG_COUNTER_SIZE 10
#define MAX_LOG_FILE_SIZE 4096

#define REQ_LOG "REQUEST_LOG"
#define REQ_LOG_LIST "REQUEST_LIST_LOGS"

//#define LOGGER_ON


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

int list_log_files(char *buff, uint32_t size);

int read_log(char* buff, uint32_t size, uint32_t log_counter);

int logger_sync();

#endif /* LOGGER_LOGGER_H */
