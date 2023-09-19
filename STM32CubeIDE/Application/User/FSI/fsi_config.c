/** ======================================================================================
 *  @file 	default_config.c
 * 	@brief 	System Run Configuration - Source File
 *	---------------------------------------------------------------------------------------
 *	In this file are defined all default  configuration  structure. It demonstrate  how  to
 *	properly create and configure project settings. If user wont, he can add some custom
 *	modification in order to adapt system setting to some other project requirements
 *	---------------------------------------------------------------------------------------
 *  @date 	April 2020
 *  @author TregoLTD
 *  @author Haris Turkmanovic
 *  @email	harist@thevtool.com
 *  ========================================================================================
 */
/*--------------------------------- Block Device Driver Includes --------------------------*/
#include "FSI/LittleFS/BD_Drivers/Flash_BD/bd_driver_flash.h"
/*-------------------------------- File system Interface Includes -------------------------*/
#include "fsi.h"
/*-------------------------------- Input-Output Interface Includes ------------------------*/
/*-------------------------------- Command Line Interface Includes ------------------------*/
//#include "cli.h"
#include <stdio.h>

/*---------------------------- File System Interface Configuration ------------------------*/
//---Buffer configured by user which are necessary for FSI---//
char	    				LFS_BUFFER_WORKING_DIRECTORY[1024];
uint32_t                    lfs_buff_write[BD_DRIVER_FLASH_PROG_SIZE/4];
uint32_t 					lfs_buff_read[BD_DRIVER_FLASH_READ_SIZE/4];
uint32_t					lfs_buff_lookahead[BD_DRIVER_FLASH_PROG_SIZE/4];
//uint8_t 					LFS_BUFFER_WRITE[BD_DRIVER_FLASH_PROG_SIZE];
//uint8_t 					LFS_BUFFER_READ[BD_DRIVER_FLASH_READ_SIZE];
//uint8_t 					LFS_BUFFER_LOOKAHEAD[BD_DRIVER_FLASH_PROG_SIZE];

//---File System Interface default configuration---//
FSI_Config DEFAULT_FSI_CONFIGURATION =
{
    //Block device operations
    .bd_read  	= bd_driver_flash_read,
    .bd_write 	= bd_driver_flash_prog,
    .bd_erase 	= bd_driver_flash_erase,
    .bd_sync  	= bd_driver_flash_sync,
	.bd_init	= bd_driver_flash_init,

    //Block device configuration
    .ReadSize 			= 	BD_DRIVER_FLASH_READ_SIZE,
    .ProgSize 			= 	BD_DRIVER_FLASH_PROG_SIZE,
    .BlockSize 			= 	BD_DRIVER_FLASH_BLOCK_SIZE,
    .BlockCount 		= 	BD_DRIVER_FLASH_BLOCK_COUNT,
	.BaseAddres 		= 	BD_DRIVER_FLASH_MEMORY_START_ADDRESS,
	.StartBlockNo		= 	BD_DRIVER_FLASH_MEMORY_START_BLOCK,
	.StartBankNo	 	= 	BD_DRIVER_FLASH_MEMORY_START_BANK,
    //File system configuration
	.ReadBuffer			= 	(uint8_t *)lfs_buff_read,
	.WriteBuffer		= 	(uint8_t *)lfs_buff_write,
	.LookaheadBuffer	= 	(uint8_t *)lfs_buff_lookahead,
	//FSI Configuration
	.PathBuffer			=	LFS_BUFFER_WORKING_DIRECTORY,
	.PathLength			=	1024,
	.ReadLine			= 	scanf,
	.WriteLine		 	= 	printf
};
#if 0 //MR

/*-------------------------- Command Line Interface Configuration ------------------------*/
//--- Buffer configured by user which are necessary for CLI ---//
char 		CLI_BUFFER_INPUT[1024];
char		CLI_BUFFER_WORKING[1024];

//--- Supported command list ---//
CLI_Command_Info CLI_SUPPORTED_COMMAND[] = {
		{"test", "Test Cli cmd",   test_cli_cmd}
//		{"mkdir", 	"Make new directory",	fsi_dir_make},
//		{"cd",		"Open directory",		fsi_dir_open},
//		{"vi",		"Open new file, if "
//				"file does not exist, or "
//				"write to the end of file "
//				"if file already exist",	fsi_file_write},
//		{"cat",		"Read file content",	fsi_file_read},
//		{"cd..","Close directory. Also, "
//				"\"cd ..\" command can be"
//				" used",					fsi_dir_close},
//		{"ls",	"List directory content", 	fsi_dir_list},
//		{"rmdir","Remove directory",		fsi_dir_remove},
//		{"rm",	 "Remove file",  			fsi_file_remove},
//		{"stat", "Print blocks number "
//				"used by file system",		fsi_stat_size}
};
uint32_t kakaTest(char *buff)
{
	sprintf(buff,"1");
	return 2;
}
//--- CLI Configuration Structure ---//
CLI_Config DEFAULT_CLI_CONFIGURATION = {
		.CommandsCount 		= 1,
		.CommandsList 		= CLI_SUPPORTED_COMMAND,
		.InputBuffer 		= CLI_BUFFER_INPUT,
		.InputBufferLength 	= 1024,
		.WorkingBuffer 		= CLI_BUFFER_WORKING,
		.WorkingBufferLength= 1024,
		.ReadLine			= scanf,
		.WriteLine		 	= printf,
//		.GetPath			= fsi_path_get
		.GetPath			= kakaTest
};
#endif

