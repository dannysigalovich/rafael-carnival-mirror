

#include "logger.h"


extern lfs_t 						LFS;		/*!< LittleFS main instance */
extern struct lfs_config 			LFS_CFG;	/*!< LittleFS configuration instance */
extern FSI_Config                   DEFAULT_FSI_CONFIGURATION;


void init_logger(void){
    /* The fsi init will config the littlefs with our flash and mount a rootfs for us */
    fsi_init(&DEFAULT_FSI_CONFIGURATION);
}


void logger_test(void){
    /* Create a file */
    lfs_file_t file;
    lfs_file_open(&LFS, &file, "/run.log", LFS_O_WRONLY | LFS_O_CREAT);

    /* Write to the file */
    const char message[] = "Hello World!\n";
    lfs_file_write(&LFS, &file, message, sizeof(message)-1);

    /* Close the file */
    lfs_file_close(&LFS, &file);

    /* Read the file */
    char buffer[32];
    lfs_file_open(&LFS, &file, "/run.log", LFS_O_RDONLY);
    int size = lfs_file_read(&LFS, &file, buffer, sizeof(buffer));
    lfs_file_close(&LFS, &file);
    // print it with a for loop 
    for(int i = 0; i < size; i++){
        printf("%c", buffer[i]);
    }
    printf("\n");
}
