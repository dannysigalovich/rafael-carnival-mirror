/** =====================================================================================
 *  @file 	fsi.h
 * 	@brief 	File System Interface - Header File
 *	-------------------------------------------------------------------------------------
 *
 *		In this file are  defined functions, variables and structures used by File System
 *	Interface (FSI). Main FSI purpose  is to control execution  of LittleFS functions and
 *	also to properly conduct Block Device initialization. One of the main FSI tasks is to
 *	remember path to the current user working directory, because LittleFS does not support
 *	that functionality which can be so useful, especially when number of files on the file
 *	system increase. Also, FSI prepare all parameters and structures necessary for successful
 *	execution of LittleFS functions. In other words, FSI is a LittleFS wrapper. User can
 *	implement their own	LittleFS wrapper functions  and in that case, functions defined in
 *	this file can be used as example how to use LittleFS functions. Functions define in this
 *	file  are created  in order to demonstrate  LittleFS possibilities  and also to connect
 *	LittleFS with Command Line Interface Library
 *
 *	---------------------------------------------------------------------------------------
 *  @date 	April 2020
 *  @author TregoLTD
 *  @author Haris Turkmanovic
 *  @email	harist@thevtool.com
 *  ========================================================================================
 */
#ifndef 	INC_FSI_H_
#define 	INC_FSI_H_
/*----------------------------------- Standard Includes -----------------------------------*/
#include 	"stdint.h"
/*----------------------------------- LittleFS Includes -----------------------------------*/
#include 	"LittleFS/lfs.h"
/*----------------------------------Block Device Driver Includes -----------------------------------*/
#include 	"LittleFS/BD_Drivers/Flash_BD/bd_driver_flash.h"


/*--------------------------- File System Interface Configuration -------------------------*/
#define 	FSI_BUFFER_SIZE					1024



/*! @struct FSI_Config
    @brief 	File System configuration structure.
	This structure is used to configure File System Interface. It is
	passed to FSI_Init structure and based on the structure values,
	File System Interface initialization is done.
*/
typedef struct{
	/*Block device function used to read content from one block of block device.*/
	int 		(*bd_read)(const struct lfs_config *cfg, lfs_block_t block,
	        				lfs_off_t off, void *buffer, lfs_size_t size);
	/*Block device function used to write content to one block of block device.*/
	int 		(*bd_write)(const struct lfs_config *cfg, lfs_block_t block,
	        				lfs_off_t off, const void *buffer, lfs_size_t size);
	/*Block device function used to erase content from one block of block device.*/
	int 		(*bd_erase)(const struct lfs_config *cfg, lfs_block_t block);
	/*Block device function used to write content from block device cache back to memory*/
	int			(*bd_sync)(const struct lfs_config *cfg);
	/*Block device initialization function*/
	int			(*bd_init)(const struct lfs_config *cfg, const LFS_BD_Flash_Config *bdcfg);
	/*Size of the one memory block in Bytes*/
	uint32_t	BlockSize;
	/*Number of blocks used with file system*/
	uint32_t	BlockCount;
	/*Minimum data length which can be write as single unit to block device*/
	uint32_t	ProgSize;
	/*Minimum data length which can be read as single unit from block device*/
	uint32_t	ReadSize;
	/*Block device base address*/
	uint32_t	BaseAddres;
	/*Number of first block in memory which is used with file system*/
	uint32_t	StartBlockNo;
	/*Number of bank used with file system*/
	uint32_t	StartBankNo;
	/* This buffer is used by LittleFS. It must be allocated by user and buffer size
	 * is minimum equal to value of Read size */
	uint8_t*	ReadBuffer;
	/* This buffer is used by LittleFS. It must be allocated by user and buffer size
	 * is minimum equal to value of Prog size */
	uint8_t*	WriteBuffer;
	/* This buffer is used by LittleFS. It must be allocated by user and buffer size
	 * is minimum equal to value of prog size */
	uint8_t*	LookaheadBuffer;
	/*Pointer to buffer used to stored path to the current working directory in file system*/
	char*		PathBuffer;
	/*Path buffer length*/
	uint32_t	PathLength;
	/**
	 * @brief 	User defined function which writes data to console
	 * @param 	Data 			- 	Pointer to null terminated string
	 * @return 	0	 			- 	If data is successfully written to console
	 * @return  					Value different from 0 if there is some errors
	 */
	uint32_t	(*WriteLine)(const char* Data);
	/**
	 * @brief 	User defined function which reads data from console
	 *
	 * 			User function should be able to read user inputs until user press the enter on the
	 * 			keyboard which indicate that entering one data line is finished. In that case,
	 * 			function return 0. If user press ESC button on the keyboard, that also must be
	 * 			detected and, in that case, function return 2. If Maximum length is reached,
	 * 			function return 1.
	 *
	 * @param 	Data 			- 	Pointer to string buffer which is use to store data	read
	 * 								from console
	 * @param	DataLength		-	Length of data read from console
	 * @param	MaxDataLength	-	Max data length which can be stored to *Data string buffer
	 * @return 	0	 			- 	If data is successfully read from console
	 * @return  1				-	Maximum data length reached
	 * @return  2				-	ESC char is detected
	 */
	uint32_t	(*ReadLine)(char* Data, uint32_t* DataLength,uint32_t MaxLength);
}FSI_Config;

/*! @struct FSI_Info
    @brief 	File System Interface structure.
	This structure is used to store basic informations used by File System interface
*/
typedef struct{
	/*Null terminated string contains path to current working directory*/
	char*		Path;
	/*Max path length*/
	uint32_t	PathLength;
	/**
	 * @brief 	Assigned function which writes data to console
	 * @param 	Data 			- 	Pointer to null terminated string
	 * @return 	0	 			- 	If data is successfully written to console
	 * @return  					Value different from 0 if there is some errors
	 */
	uint32_t	(*WriteLine)(const char*);
	/**
	 * @brief 	Assigned function which reads data from console
	 *
	 *
	 * @param 	Data 			- 	Pointer to string buffer which is use to store data	read
	 * 								from console
	 * @param	DataLength		-	Length of data read from console
	 * @param	MaxDataLength	-	Max data length which can be stored to *Data string buffer
	 * @return 	0	 			- 	If data is successfully read from console
	 * @return  1				-	Maximum data length reached
	 * @return  2				-	ESC char is detected
	 */
	uint32_t	(*ReadLine)(char* Data, uint32_t* DataLength,uint32_t MaxLength);
}FSI_Info;

/**
 * @brief 	File System Interface initialization function
 * @param 	Configuration	- 	Pointer to FSI_Config structure which contains all informations
 * 								necessary to properly configuration File System Interface.
 * 								@see FSI_Config structure
 * @return 	0	 			- 	Initialization is successful
 * @return  1				- 	Block device initialization is failed
 * @return  2				-	File system mounting problem
 */
uint8_t		fsi_init(FSI_Config* Configuration);
/**
 * @brief 	Get current working path
 * @param 	Path			- 	Pointer to buffer where Path string will be store. This buffer must
 * 								be initialize with 0 before call this function, or otherwise wrong
 * 								read may occur
 * @return 	Path string length
 */
uint32_t	fsi_path_get(char* Path);
/**
 * @brief 	Make directory
 *
 * 			This function make new directory on file system.
 *
 * @param 	Param			- 	Pointer to null terminated string which represents directory name.
 * @param 	ParamSize		- 	Directory name string length.
 * @return	0				- 	Directory is successfully created
 * @return	1				- 	Function parameters are wrong
 * @return	2				- 	Directory creation is failed
 */
uint8_t 	fsi_dir_make(const char* Param, uint32_t ParamSize);
/**
 * @brief 	Remove directory
 *
 * 			This function remove existing directory from file system.
 *
 * @param 	Param			- 	Pointer to null terminated string which represents directory name.
 * @param 	ParamSize		- 	Directory name string length.
 * @return	0				- 	Directory is successfully removed from File system
 * @return	1				- 	Function parameters are wrong
 * @return	2				- 	Problem occurs during removing directory
 * @return	3				- 	Try to remove something from File system which is not directory
 */
uint8_t		fsi_dir_remove(const char* Param, uint32_t ParamSize);
/**
 * @brief 	Open directory
 *
 * @param 	Param			- 	Pointer to null terminated string which represents directory name.
 * @param 	ParamSize		- 	Directory name string length.
 * @return	0				- 	Directory is successfully open
 * @return	1				- 	Directory can not be open
 */
uint8_t 	fsi_dir_open(const char* Param, uint32_t ParamSize);
/**
 * @brief 	Close directory
 *
 * @param 	Param			- 	Pointer to null terminated string which represents directory name.
 * @param 	ParamSize		- 	Directory name string length.
 * @return	0				- 	Directory is successfully close
 */
uint8_t 	fsi_dir_close(const char* Param, uint32_t ParamSize);
/**
 * @brief 	List directory content
 *
 * @param 	Param			- 	Pointer to null terminated string which represents directory name.
 * @param 	ParamSize		- 	Directory name string length.
 * @return	0				- 	Directory content is successfully listed
 * @return	1				-	Wrong function arguments
 * @return	2				-	Directory can not be openned
 */
uint8_t 	fsi_dir_list(const char* Param, uint32_t ParamSize);
/**
 * @brief 	Read from file
 *
 * 			This function calls user define console function to print file content to serial console
 *
 * @param 	Param			- 	Pointer to null terminated string which represents file name.
 * @param 	ParamSize		- 	Directory name string length.
 * @return	0				- 	File content is successfully read
 * @return	1				-	File does not exist
 * @return	2				-	Problem with file read
 * @return	3				-	Problem opening the file
 */
uint8_t 	fsi_file_read(const char* Param, uint32_t ParamSize);
/**
 * @brief 	Write to file
 *
 * 			This function calls user define console functions to read content over serial interface
 *
 * @param 	Param			- 	Pointer to null terminated string which represents file name.
 * @param 	ParamSize		- 	Directory name string length.
 * @return	0				- 	Content is successfully written to file
 * @return	1				-	File can not be created
 * @return	2				-	Content can not be added to the file
 */
uint8_t 	fsi_file_write(const char* Param, uint32_t ParamSize);
/**
 * @brief 	Remove file from file system
 *
 * @param 	Param			- 	Pointer to null terminated string which represents file name.
 * @param 	ParamSize		- 	Directory name string length.
 * @return	0				- 	File is successfully removed
 * @return	1				-	Wrong command parameters
 * @return	2				-	FIle name is too long
 * @return  3				- 	File can not be removed because file system problem occurs
 * @return  4				- 	Trying to remove something from file system which is not a file
 */
uint8_t 	fsi_file_remove(const char* Param, uint32_t ParamSize);
/**
 * @brief 	Remove file from file system
 *
 * @param 	Param			- 	Pointer to null terminated string which represents file name.
 * @param 	ParamSize		- 	Directory name string length.
 * @return	0				- 	File is successfully removed
 * @return	1				-	Wrong command parameters
 * @return	2				-	FIle name is too long
 * @return  3				- 	File can not be removed because file system problem occurs
 * @return  4				- 	Trying to remove something from file system which is not a file
 */
uint8_t		fsi_stat_size(const char* Param, uint32_t ParamSize);

int demo_r( const char* Param, const uint32_t addr_Buf ); // TODO remove after debug

#endif /* INC_FSI_H_ */
