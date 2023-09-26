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
/*-------------------------------------- STANDARD Includes --------------------------------*/
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/*-------------------------------- File System Interface Includes --------------------------*/
#include "fsi.h"
/*-------------------------------------- LittleFS Includes ---------------------------------*/
#include "LittleFS/lfs.h"
/*-----------------------------------Block Device Driver Includes --------------------------*/
#include "LittleFS/BD_Drivers/Flash_BD/bd_driver_flash.h"




FSI_Info					FSI;	/*!< Main File System Interface information are stored in this instance */


lfs_t 						LFS;		/*!< LittleFS main instance */
struct lfs_config 			LFS_CFG;	/*!< LittleFS configuration instance */
struct lfs_info 			LFS_INFO;	/*!< Variable used to store information in some functions */
LFS_BD_Flash 				LFS_BD;		/*!< Block device instance */
LFS_BD_Flash_Config		 	LFS_BD_CFG;	/*!< Block device configuration instance*/



char						FSI_BUFFER[FSI_BUFFER_SIZE];	/*!< Buffer used by FSI for some internal calculation */

/**
 * @brief 	Make file system interface root path
 *
 * 			THis function is usually call only once when LittleFS mounting is successfully done
 */
 static inline void fsi_make_rooth_path(){
	memset(FSI.Path,0,FSI.PathLength);
	FSI.Path[0]='/';
}
/**
 * 	@brief 	Remove current working directory name from path string
 *
 *	@return 0	-	Directory is successfully removed from path
 *	@return 1	-	Path string is return to the beginning
 */
uint8_t 	fsi_path_dir_close(){
	int i = 0;
	//find the end of the path
	while(FSI.Path[i]!=0)i++;
	if( i==1 && FSI.Path[i-1]=='/')return 1;
	//back one directory
	while(FSI.Path[i]!='/'){
		FSI.Path[i] = 0;
		i--;
	}
	if(i!=0)FSI.Path[i] = 0;
	return 0;
}
/**
 * 	@brief 	Add directory name to the working path string
 *
 *	@return 0	-	Directory name is successfully added to working path
 *	@return 1	-	Path string reach maximum length
 */
uint8_t 	fsi_path_dir_open(const char* DirName, uint32_t DirNameLength){
	int i = 0, j = 0;
	//Check if user enter ".." as argument which indicate that current
	//directory should be close
	if(DirNameLength == 2 && DirName[0]=='.' && DirName[1]=='.'){
		fsi_path_dir_close();
		return 0;
	}
	//Otherwise, as first step go to the end of path string
	while(FSI.Path[i]!=0)i++;
	//Chick whether or not directory name fit to available path string
	if((i+DirNameLength+1) > FSI.PathLength) return 1;
	//Detect if user enter "/" as argument which indicate that we should now
	//return back to the root directory
	if(DirName[0] == '/' && DirNameLength ==1 )
	{
		memset(FSI.Path,0,FSI.PathLength);
		FSI.Path[0]='/';
		return 0;
	}
	//Add directory to the current working path
	if(i!=1)FSI.Path[i++] = '/';
	while(j<DirNameLength){
		//Check whether the user is accidental enter '/' at the beginning
		//or at the end of dir name string. If it is, skip this characters
		if( (j== 0 && DirName[j]=='/') || (j== (DirNameLength-1) && DirName[j]=='/'))j++;
		else
		{
			FSI.Path[i++]=DirName[j++];
		}
	}
	return 0;
}
/**
 * 	@brief 	Get current working directory name
 *
 *	@return length of Directory name string
 */
uint32_t 	fsi_path_dir_get_name(char* DirName){
	int i = 0;
	int DirNameLength = 0;
	//find the end of the path
	while(FSI.Path[i]!=0)i++;
	//back one directory
	while(FSI.Path[i]!='/')i--;
	//
	while(FSI.Path[i]!=0){
		DirName[DirNameLength] = FSI.Path[i];
		DirNameLength++;
		i++;
	}
	return DirNameLength;
}
/**
 * 	@brief 	Add file name to the end of the current path string
 *
 *	@return 0	-	File name is successfully added
 *	@return 1	-	File name is too long to fit in path buffer string
 */
uint8_t 	fsi_path_file_open(const char* FileName, uint32_t FileNameLength){
	int i = 0,j = 0;
	//As first go to the end of path string
	while(FSI.Path[i]!=0)i++;
	//Check if file name can be added to the end of path string
	if((i+FileNameLength+1) > FSI.PathLength) return 1;
	if(FSI.Path[i-1]!='/') FSI.Path[i++] = '/';
	//Copy file name to the end of path string buffer
	while(j<FileNameLength)FSI.Path[i++]=FileName[j++];
	return 0;
}
/**
 * 	@brief 	Remove file name from the end of current path string
 *
 *	@return 0	-	File name is successfully removed from working path
 *	@return 1	-	End of path string
 */
uint8_t 	fsi_path_file_close(){
	uint32_t i = 0;
	while(FSI.Path[i]!=0)i++;
	if( i==1 && FSI.Path[i-1]=='/')return 1;
	while(FSI.Path[i]!='/'){
		FSI.Path[i--]=0;
	}
	if(i!=0)FSI.Path[i] = 0;
	return 0;
}

uint32_t		fsi_path_get(char* Path){
	uint32_t i = 0;
	while(FSI.Path[i]!=0){
		Path[i]=FSI.Path[i];
		i++;
	}
	return i;
}
uint8_t		fsi_init(FSI_Config* Configuration)
{
int32_t	stat=0;

	//File system configuration
	LFS_CFG.read 			= Configuration->bd_read;
	LFS_CFG.prog 			= Configuration->bd_write;
	LFS_CFG.erase 			= Configuration->bd_erase;
	LFS_CFG.sync			= Configuration->bd_sync;
	LFS_CFG.context			= &LFS_BD;
	LFS_CFG.prog_size 		= Configuration->ProgSize;
	LFS_CFG.read_size 		= Configuration->ReadSize;
	LFS_CFG.block_count 	= Configuration->BlockCount;
	LFS_CFG.block_size 		= Configuration->BlockSize;
	LFS_CFG.lookahead_size 	= Configuration->ProgSize;
	LFS_CFG.cache_size	 	= Configuration->ProgSize;
	LFS_CFG.block_cycles	= 100;
	LFS_CFG.read_buffer		= Configuration->ReadBuffer;
	LFS_CFG.prog_buffer		= Configuration->WriteBuffer;
	LFS_CFG.lookahead_buffer= Configuration->LookaheadBuffer;
	//Block device configuration
	LFS_BD_CFG.StartBlock		= Configuration->StartBlockNo;
	LFS_BD_CFG.StartBank 		= Configuration->StartBankNo;
	LFS_BD_CFG.Buffer			= (uint8_t*)Configuration->BaseAddres;
	LFS_BD_CFG.EraseValue       = -1;
	//File system interface configuration
	FSI.Path			= Configuration->PathBuffer;
	FSI.PathLength		= Configuration->PathLength;
	FSI.ReadLine		= Configuration->ReadLine;
	FSI.WriteLine		= Configuration->WriteLine;

	if(Configuration->bd_init(&LFS_CFG,&LFS_BD_CFG) != 0){
		FSI.WriteLine("Problem with block device initialization\n\r");
		return 1;
	}
/*	stat = lfs_format(&LFS, &LFS_CFG);
	if(stat != 0)
		return 2;
*/
	if (lfs_mount(&LFS, &LFS_CFG) != 0)
	{
		FSI.WriteLine("Problem with file system mounting\n\r");
		stat = lfs_format(&LFS, &LFS_CFG);
		if (stat != 0)
			return 2;
		if (lfs_mount(&LFS, &LFS_CFG) != 0)
			return 2;
	}
	FSI.WriteLine("File system is successfully mount on entry point / \n\r");
	fsi_make_rooth_path();
	return 0;
}
uint8_t 	fsi_dir_make(const char* Param, uint32_t ParamSize)
{
	if(Param == 0 || ParamSize == 0){
		FSI.WriteLine("Invalid parameters\r\n");
		return 1;
	}
	if(fsi_path_dir_open(Param,ParamSize)!=0){
		FSI.WriteLine("Directory name is too long\r\n");
		return 1;
	}
	int res = lfs_mkdir(&LFS,FSI.Path);
	if(res<0)
	{
		FSI.WriteLine("Directory can not be created\r\n");
		fsi_path_dir_close();
		return 2;
	}
	FSI.WriteLine("Directory \"");
	FSI.WriteLine(Param);
	FSI.WriteLine("\" is successfully created\r\n");
	fsi_path_dir_close();
	return 0;
}
uint8_t		fsi_dir_remove(const char* Param, uint32_t ParamSize){
	if(Param == 0 || ParamSize == 0){
		FSI.WriteLine("Wrong command arguments\r\n");
		return 1;
	}
	int res;
	if(fsi_path_dir_open(Param,ParamSize)!=0){
		FSI.WriteLine("Dir name is too long\r\n");
		return 1;
	}
	res = lfs_stat(&LFS,FSI.Path,&LFS_INFO);
	if(res!=0){
		fsi_path_dir_close();
		FSI.WriteLine("Problem with removing directory or path\r\n");
		return 2;
	}
	if(LFS_INFO.type != LFS_TYPE_DIR){
		fsi_path_dir_close();
		FSI.WriteLine("Not directory\r\n");
		return 3;
	}
	res = lfs_remove(&LFS,FSI.Path);
	if(res!=0){
		if(res == LFS_ERR_NOTEMPTY){
			FSI.WriteLine("Directory is not empty\r\n");
		}
		else{
			FSI.WriteLine("Problem with removing directory or path\r\n");
		}
		fsi_path_dir_close();
		return 2;
	}
	FSI.WriteLine("Directory ");
	FSI.WriteLine(Param);
	FSI.WriteLine(" is successfully removed\r\n");
	fsi_path_dir_close();
	return 0;
}
/* 		As first, this function check did directory exist on File system by calling File system
 * function which open directory. If directory exist on File system, string path to the working
 * directory is expand by directory name and after that directory on file system is closed, but
 * working directory string path still contains directory name. This approach is necessary because
 * LittleFS does not support some additional actions while directory is open.Because of that File
 * System Interface remember current working path and user does not know what really happens. For
 * example, let assume that directory named Test already exists on the File system. In that case,
 * this is how system works:
 *
 * 				User View(in console)					File System View (what really happens)
 *
 * 			-	/> cd Test								- Open Directory on File system
 * 														- If directory exist, expand working path
 * 														- Close Directory on File system
 * 			-	/Test>									-
 */
uint8_t 	fsi_dir_open(const char* Param, uint32_t ParamSize){
	if(Param == 0 || ParamSize == 0){
		FSI.WriteLine("Wrong command arguments\r\n");
		return 1;
	}
	lfs_dir_t dir;
	int res;
	if(fsi_path_dir_open(Param,ParamSize)!=0){
		FSI.WriteLine("Dir name is too long\r\n");
		return 1;
	}
	res = lfs_dir_open(&LFS,&dir,FSI.Path);
	if(res != 0){
		fsi_path_dir_close();
		FSI.WriteLine("File system can not open directory\r\n");
		return 1;
	}
	res = lfs_dir_close(&LFS,&dir);
	if(res != 0){
		fsi_path_dir_close();
		FSI.WriteLine("File system can not close directory\r\n");
		return 1;
	}
	return 0;
}

/* 		This function implements the opposite functionality than fsi_dir_open function. It just
 * reduce path and remember it for further works with file system
 */
uint8_t 	fsi_dir_close(const char* Param, uint32_t ParamSize){
	fsi_path_dir_close();
	return 0;
}
uint8_t 	fsi_file_write(const char* Param, uint32_t ParamSize){
	if(Param == 0 || ParamSize == 0) return 1;
	lfs_file_t file;
	int res = fsi_file_read(Param,ParamSize);
	if(fsi_path_file_open(Param,ParamSize)!=0){
		FSI.WriteLine("File path is not valid\n\r");
		return 2;
	}
	if(res == 1){
		file = (lfs_file_t){0};
		res = lfs_file_open(&LFS,&file,FSI.Path,LFS_O_RDWR | LFS_O_CREAT);
		if(res != 0){
			FSI.WriteLine("File can not be created\n\r");
			fsi_path_file_close();
			return 1;
		}
	}
	else{
		res = lfs_file_open(&LFS,&file,FSI.Path,LFS_O_APPEND);
		if(res != 0){
			FSI.WriteLine("File can not be open for write\n\r");
			fsi_path_file_close();
			return 2;
		}
	}
	FSI.WriteLine("File is open and ready to be written\n\r");
	FSI.WriteLine("Pres ESC to exit\n\r");
	memset(FSI_BUFFER,0,FSI_BUFFER_SIZE);
	uint32_t DataLength = 0;
	FSI.WriteLine(">");
	while(FSI.ReadLine(FSI_BUFFER,&DataLength,FSI_BUFFER_SIZE)!=2){
		res = lfs_file_write(&LFS,&file,FSI_BUFFER,DataLength);
		if(res != DataLength){
			FSI.WriteLine("Problem during file write\n\r");
			fsi_path_file_close();
			return 2;
		}
		FSI.WriteLine("\n>");
		memset(FSI_BUFFER,0,FSI_BUFFER_SIZE);
		DataLength = 0;
	}
	FSI.WriteLine("\n\r");
	res = lfs_file_close(&LFS,&file);
	if(res!= 0){
		FSI.WriteLine("Problem during file closing\n\r");
		fsi_path_file_close();
		return 2;
	}
	FSI.WriteLine("!File is sucesffuly written\n\r");
	fsi_path_file_close();
	return 0;
}
uint8_t		fsi_file_read(const char* Param, uint32_t ParamSize){
	if(Param == 0 || ParamSize == 0) return 1;
	lfs_file_t file;
	if(fsi_path_file_open(Param, ParamSize)!=0){
		FSI.WriteLine("File path is not valid");
		return 3;
	}
	int res = lfs_file_open(&LFS, &file, FSI.Path, LFS_O_RDONLY);
	if(res == LFS_ERR_NOENT){
		FSI.WriteLine("File");
		FSI.WriteLine(Param);
		FSI.WriteLine(" does not exist\r\n");
		fsi_path_file_close();
		return 1;
	}
	if(res == 0){
		memset(FSI_BUFFER,0,FSI_BUFFER_SIZE);
		res = lfs_file_read(&LFS,&file,FSI_BUFFER,FSI_BUFFER_SIZE);
		if(res < 0){
			FSI.WriteLine("Problem while read file\r\n");
			lfs_file_close(&LFS,&file);
			return 2;
		}
		FSI.WriteLine("\r\nFile content:\r\n--\r\n");
		FSI.WriteLine(FSI_BUFFER);
		FSI.WriteLine("\r\n--\r\n\r\n");
		lfs_file_close(&LFS,&file);
		fsi_path_file_close();
		return 0;
	}
	FSI.WriteLine("Problem opening the file\n\r");
	fsi_path_file_close();
	return 3;
}
uint8_t 	fsi_dir_list(const char* Param, uint32_t ParamSize){
	int res;
	lfs_dir_t OpenDir;
	int i =0;
	char FileSizeString[6];
	if(Param != 0 && ParamSize != 0){
		if(fsi_path_dir_open(Param, ParamSize)!=0){
			return 2;
		}
	}
	res = lfs_dir_open(&LFS,&OpenDir,FSI.Path);
	if(res < 0){
		fsi_path_dir_close();
		FSI.WriteLine("Directory can not be open\n\r");
		return 2;
	}
	res = lfs_dir_rewind(&LFS, &OpenDir);
	if(res<0){
		fsi_path_dir_close();
		FSI.WriteLine("Directory can not be open\n\r");
		res = lfs_dir_close(&LFS, &OpenDir);
		return 2;
	}
	FSI.WriteLine("\n\r");
	do
	{
		res = lfs_dir_read(&LFS,&OpenDir,&LFS_INFO);
		if(LFS_INFO.name[0]!='.' && LFS_INFO.name[0]!='\0')
		{

			switch(LFS_INFO.type){
			case LFS_TYPE_DIR:
				FSI.WriteLine("**D ");
				memset(FileSizeString,' ',5);
			break;
			case LFS_TYPE_REG:
				FSI.WriteLine("--F ");
				memset(FileSizeString,0,5);
				sprintf(FileSizeString,"%dB",(unsigned int)LFS_INFO.size);
			break;
			}
			i = 0;
			while(i < 5){
				if(FileSizeString[i]==0)FileSizeString[i]=' ';
				i++;
			}
			FileSizeString[i] = 0;
			FSI.WriteLine(FileSizeString);
			FSI.WriteLine(" - ");
			FSI.WriteLine(LFS_INFO.name);
			FSI.WriteLine(" - ");
			FSI.WriteLine("\n\r");
		}
	}while(LFS_INFO.name[0]!='\0');
	res = lfs_dir_close(&LFS, &OpenDir);
	if(res < 0){
		FSI.WriteLine("Problem to close file\n\r");
	}
	FSI.WriteLine("\n\r");
	if(Param != 0 && ParamSize != 0){
		if(fsi_path_dir_close()!=0){
			return 2;
		}
	}
	return 0;
}
uint8_t 	fsi_file_remove(const char* Param, uint32_t ParamSize){
	if(Param == 0 || ParamSize == 0){
		FSI.WriteLine("Wrong command arguments\r\n");
		return 1;
	}
	int res;
	if(fsi_path_file_open(Param,ParamSize)!=0){
		FSI.WriteLine("File name is too long\r\n");
		return 2;
	}
	res = lfs_stat(&LFS,FSI.Path,&LFS_INFO);
	if(res!=0){
		fsi_path_file_close();
		FSI.WriteLine("Problem with removing file\r\n");
		return 3;
	}
	if(LFS_INFO.type != LFS_TYPE_REG){
		fsi_path_file_close();
		FSI.WriteLine("Not file\r\n");
		return 4;
	}
	res = lfs_remove(&LFS,FSI.Path);
	if(res!=0){
		FSI.WriteLine("Problem with removing directory or path\r\n");
		fsi_path_file_close();
		return 3;
	}
	FSI.WriteLine("File ");
	FSI.WriteLine(Param);
	FSI.WriteLine(" is successfully removed\r\n");
	fsi_path_file_close();
	return 0;
}
int		fsi_test(void* Param, lfs_block_t BlockNumber){
	memset(FSI_BUFFER,0,FSI_BUFFER_SIZE);
	sprintf(FSI_BUFFER,"%d",(int)BlockNumber);
	FSI.WriteLine("Block: ");
	FSI.WriteLine(FSI_BUFFER);
	FSI.WriteLine("\n\r");
	return 0;
}
uint8_t		fsi_stat_size(const char* Param, uint32_t ParamSize){
	uint32_t NumberOfBlocks = lfs_fs_size(&LFS);
	memset(FSI_BUFFER,0,FSI_BUFFER_SIZE);
	sprintf(FSI_BUFFER,"%d",(int)NumberOfBlocks);
	FSI.WriteLine("Number of used blocks: ");
	FSI.WriteLine(FSI_BUFFER);
	FSI.WriteLine(" \n\r");
	memset(FSI_BUFFER,0,FSI_BUFFER_SIZE);
	lfs_fs_traverse(&LFS,fsi_test,0);
	return 0;
}

#if 0
//TODO remove after demo dbg
int demo_r( const char* Param, const uint32_t addr_Buf )
{
    int32_t size = 0;
    lfs_file_t file;
    int res = lfs_file_open(&LFS, &file, Param, LFS_O_RDONLY);
    int ret_code = -1;

    while (res == 0)
    {
        if (res != 0)
            break;      //Failed exit

        size = lfs_file_size(&LFS, &file);

        if (size <= 0)
            break;      //Failed exit

        res = lfs_file_read(&LFS, &file, (const char*) (addr_Buf), size);

        if (res <= 0)
            break;     //Failed exit

        ret_code = 0;  // Read OK
        break;         // Exit
    }

    if( lfs_file_close(&LFS,&file) != 0)
        ret_code = -1; //Failed

    return ret_code;   //Success
}

#endif
