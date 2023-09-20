

/*
    test for file system interface (FSI) with LittleFS
*/

#include "FSI/fsi.h"

extern FSI_Config DEFAULT_FSI_CONFIGURATION;

/*

Rubustness test for FSI including the following functions:

uint8_t		fsi_init(FSI_Config* Configuration);
uint32_t	fsi_path_get(char* Path); // get cwd
uint8_t 	fsi_dir_make(const char* Param, uint32_t ParamSize);
uint8_t		fsi_dir_remove(const char* Param, uint32_t ParamSize);
uint8_t 	fsi_dir_open(const char* Param, uint32_t ParamSize);
uint8_t 	fsi_dir_close(const char* Param, uint32_t ParamSize);
uint8_t 	fsi_dir_list(const char* Param, uint32_t ParamSize);
uint8_t 	fsi_file_read(const char* Param, uint32_t ParamSize);
uint8_t 	fsi_file_write(const char* Param, uint32_t ParamSize);
uint8_t 	fsi_file_remove(const char* Param, uint32_t ParamSize);

the test will print the result of each function to the console

*/
void FSI_test(){

    fsi_init(&DEFAULT_FSI_CONFIGURATION);

    printf("FSI test\n");

    char cwd[1024];

    printf("cwd: %s\n", fsi_path_get(cwd));
    printf("mkdir /test status: %d\n", fsi_dir_make("test", 4));
    printf("mkdir /test/test2 status: %d\n", fsi_dir_make("/test/test2", 10));
    printf("ls / status: %d\n", fsi_dir_list("/", 1));
    printf("ls /test status: %d\n", fsi_dir_list("/test", 5));
    printf("ls /test/test2 status: %d\n", fsi_dir_list("/test/test2", 10));

    printf("write /test/test2/test.txt status: %d\n", fsi_file_write("/test/test2/test.txt", 20));
    printf("read /test/test2/test.txt status: %d\n", fsi_file_read("/test/test2/test.txt", 20));

    


}