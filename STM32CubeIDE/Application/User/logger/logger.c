
#include "logger.h"


extern lfs_t 						LFS;		/*!< LittleFS main instance */
extern struct lfs_config 			LFS_CFG;	/*!< LittleFS configuration instance */
extern FSI_Config                   DEFAULT_FSI_CONFIGURATION;
lfs_file_t 							curr_log_file;		/*!< LittleFS file current log */

uint32_t inc_log(void){

    uint32_t counter;
    char buff[MAX_LOG_COUNTER_SIZE] = {0};
    
    /* Open the file */
    if (lfs_file_open(&LFS, &curr_log_file, "/log_counter", LFS_O_RDWR) != LFS_ERR_OK){/* if the file doesn't exist */
        lfs_file_open(&LFS, &curr_log_file, "/log_counter", LFS_O_RDWR | LFS_O_CREAT); /* Create the file */
        counter = 0;
    }
    else{  
    	/* Read the counter */
        lfs_file_read(&LFS, &curr_log_file, buff, MAX_LOG_COUNTER_SIZE);
        counter = atoi(buff);
        /* Increment the counter */
        counter++;
    }

    /* Write the counter */
    sprintf(buff, "%lu", counter);
    lfs_file_rewind(&LFS, &curr_log_file);
    lfs_file_write(&LFS, &curr_log_file, buff, strlen(buff));

    /* Close the file */
    lfs_file_close(&LFS, &curr_log_file);

    memset(&curr_log_file, 0, sizeof(lfs_file_t));

    return counter;
}

void Logger_Init(void){

#ifdef LOGGER_ON

    /* The fsi init will config the littlefs with our flash and mount a rootfs for us */
    fsi_init(&DEFAULT_FSI_CONFIGURATION);

    uint32_t log_counter = inc_log();

    char log_file_name[MAX_LOG_FILE_NAME] = {0};
    sprintf(log_file_name, "/run_%lu.log", log_counter);

    /* Create a log file for the current running */
    lfs_file_open(&LFS, &curr_log_file, log_file_name, LFS_O_WRONLY | LFS_O_CREAT);
    log_info("Log started with run number %d", log_counter);


#endif

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


void level_log(log_level_t level, const char *message, va_list args) {

    /* Get the current time with HAL_GetTick() */
    uint32_t mstime = HAL_GetTick();

    /* Make the time from integer ms to float seconds */
    float seconds = (float)mstime / 1000.0f;

    /* Add the time to the message in the format: [seconds] level: message */
    char log_message[MAX_LOG_MESSAGE_SIZE];
    char level_str[10];
    get_level(level, level_str);

    int chars_written = snprintf(log_message, MAX_LOG_MESSAGE_SIZE, "[%f] %s: ", seconds, level_str);
    if (chars_written < 0 || chars_written >= MAX_LOG_MESSAGE_SIZE) {
        // Handle error or truncation
        return;
    }

    chars_written += vsnprintf(log_message + chars_written, MAX_LOG_MESSAGE_SIZE - chars_written, message, args);

    log_message[chars_written] = '\n';
    log_message[++chars_written] = '\0';

    va_end(args);

    lfs_file_write(&LFS, &curr_log_file, log_message, strlen(log_message));

}

void log_info(const char *message, ...) {
#ifdef LOGGER_ON
    va_list args;
    va_start(args, message);
    level_log(LOG_INFO, message, args);
    va_end(args);
#endif
}

void log_warning(const char *message, ...) {
#ifdef LOGGER_ON
    va_list args;
    va_start(args, message);
    level_log(LOG_WARNING, message, args);
    va_end(args);
#endif
}

void log_error(const char *message, ...) {
#ifdef LOGGER_ON
    va_list args;
    va_start(args, message);
    level_log(LOG_ERROR, message, args);
    va_end(args);
#endif
}

void log_critical(const char *message, ...) {
#ifdef LOGGER_ON
    va_list args;
    va_start(args, message);
    level_log(LOG_CRITICAL, message, args);
    va_end(args);
#endif
}

void close_logger(void){

#ifdef LOGGER_ON

    /* Close the file */
    lfs_file_close(&LFS, &curr_log_file);

#endif
}

int read_log(char* buff, uint32_t size, uint32_t log_counter){

#ifdef LOGGER_ON
    /* Read the file */

    char log_file_name[MAX_LOG_FILE_NAME] = {0};
    sprintf(log_file_name, "/run_%lu.log", log_counter);

    lfs_file_t read_file;

    int ret = lfs_file_open(&LFS, &read_file, log_file_name, LFS_O_RDONLY);

    if (ret < LFS_ERR_OK){
        return ret;
    }

    int size_read = lfs_file_read(&LFS, &read_file, buff, size);
    lfs_file_close(&LFS, &read_file);
    buff[size_read] = '\0';
    return size_read;
#else
    return 0;
#endif

}

int list_log_files(char *buff, uint32_t size){

#ifdef LOGGER_ON
    /* List all the log files */
    lfs_dir_t dir;
    lfs_dir_open(&LFS, &dir, "/");

    int size_read = 0;
    while (true) {
        struct lfs_info info;
        int res = lfs_dir_read(&LFS, &dir, &info);
        if (res < 0) {
            lfs_dir_close(&LFS, &dir);
            return res;
        }

        if (info.type == LFS_TYPE_REG && strncmp(info.name, "run_", 4) == 0) {
            size_read += snprintf(buff + size_read, size - size_read, "%s\n", info.name);
        }

        if (res == 0) {
            break;
        }
    }

    lfs_dir_close(&LFS, &dir);
    return size_read;

#else
    return 0;
#endif
}

int logger_sync(){
#ifdef LOGGER_ON
	return lfs_file_sync(&LFS, &curr_log_file);
#else
	return 0;
#endif
}

void logger_test(void){
#ifdef LOGGER_ON
    log_info("This is a test log message");
    log_warning("This is a test log message");
    log_error("This is a test log message");
    log_critical("This is a test log message");

    close_logger();
#endif
}
