
#include "logger.h"


extern lfs_t 						LFS;		/*!< LittleFS main instance */
extern struct lfs_config 			LFS_CFG;	/*!< LittleFS configuration instance */
extern FSI_Config                   DEFAULT_FSI_CONFIGURATION;
lfs_file_t 							curr_log_file;		/*!< LittleFS file current log */


void Logger_Init(void){
    /* The fsi init will config the littlefs with our flash and mount a rootfs for us */
    fsi_init(&DEFAULT_FSI_CONFIGURATION);

    /* Create a log file for the current running */
    lfs_file_open(&LFS, &curr_log_file, "/run.log", LFS_O_WRONLY | LFS_O_CREAT);
}

void get_level(log_level_t level, char* ret){
    switch(level){
        case LOG_INFO:
            strcpy(ret, "INFO");
            break;
        case LOG_WARNING:
            strcpy(ret, "WARNING");
            break;
        case LOG_ERROR:
            strcpy(ret, "ERROR");
            break;
        case LOG_CRITICAL:
            strcpy(ret, "CRITICAL");
            break;
        default:
            strcpy(ret, "UNKNOWN");
            break;
    }
}


void level_log(log_level_t level, const char *message, ...) {
    /* Get the current time with HAL_GetTick() */
    uint32_t mstime = HAL_GetTick();

    /* Make the time from integer ms to float seconds */
    float seconds = (float)mstime / 1000.0f;

    /* Add the time to the message in the format: [seconds] level: message */
    char log_message[MAX_LOG_MESSAGE_SIZE];
    char level_str[10];
    get_level(level, level_str);

    va_list args;
    va_start(args, message);

    int chars_written = snprintf(log_message, MAX_LOG_MESSAGE_SIZE, "[%f] %s: ", seconds, level_str);
    if (chars_written < 0 || chars_written >= MAX_LOG_MESSAGE_SIZE) {
        // Handle error or truncation
        return;
    }

    chars_written = vsnprintf(log_message + chars_written, MAX_LOG_MESSAGE_SIZE - chars_written, message, args);

    va_end(args);

    lfs_file_write(&LFS, &curr_log_file, log_message, strlen(log_message));
}

void log_info(const char *message, ...) {
    va_list args;
    va_start(args, message);
    level_log(LOG_INFO, message, args);
    va_end(args);
}

void log_warning(const char *message, ...) {
    va_list args;
    va_start(args, message);
    level_log(LOG_WARNING, message, args);
    va_end(args);
}

void log_error(const char *message, ...) {
    va_list args;
    va_start(args, message);
    level_log(LOG_ERROR, message, args);
    va_end(args);
}

void log_critical(const char *message, ...) {
    va_list args;
    va_start(args, message);
    level_log(LOG_CRITICAL, message, args);
    va_end(args);
}

void close_logger(void){
    /* Close the file */
    lfs_file_close(&LFS, &curr_log_file);
}

int read_log(char* buff, uint32_t size){
    /* Read the file */
    lfs_file_open(&LFS, &curr_log_file, "/run.log", LFS_O_RDONLY);
    int size_read = lfs_file_read(&LFS, &curr_log_file, buff, size);
    lfs_file_close(&LFS, &curr_log_file);
    buff[size_read] = '\0';
    return size_read;
}

void logger_test(void){
    /* Test the logger */
    init_logger();
    log_info("This is a test info message");
    log_warning("This is a test warning message");
    log_error("This is a test error message");
    log_critical("This is a test critical message");
    close_logger();

    /* Read the log file */
    char buff[1000] = {0};
    read_log(buff, 1000);
    printf("%s", buff);
}
