/** =====================================================================================
 *  @file 	bd_driver_flash.h
 * 	@brief 	Block Device (Embedded Flash) driver - Header File
 *	-------------------------------------------------------------------------------------
 *		Block device is storage device that supports reading and writing data in fixed-size
 *	blocks. Blocks size is generally 512 bytes or multiple of this size. It is important to
 *	mention that block device can be programmed with data size less than block size, and also
 *	data, which size is less than block size, can be read from block device. But, in order to
 *	delete  data from block storage device, which size is less than block size, erase process
 *	must be conducted  on the entire block. That means  if file  system wont to delete, for
 *	example file name which use only 12 bytes, it must erase entire block which minimum size
 *	is 512Bytes. So,  it is  important  to consider  this fact when using file system on MCU
 *	Embedded FLash Memory. In case  of LittleFs (and generally any other File system)  it is
 *	important  that  MCU Embedded flash memory  be organized  as block  storage device which
 *	maximum block (page) size is not bigger than 4KB and number of block should  not  be less
 *	than 256.
 *		In this file  are defined all  functions which is used to interface with block device.
 *	Embedded flash memory used with this functions is STML476RG MCU Flash memory. In order to
 *	access  these memory to write to and to read from  it, STM FLASH HAL Driver functions are
 *	used. IF user wont to  use these project with  some other type of memory, for example SPI
 *	flash, he must to implements his own block device function. In that case, functions defined
 *	in this file can be used as example how to properly implement that functions
 *
 *	---------------------------------------------------------------------------------------
 *  @date 	Sep 2023
 *  @author TregoLTD
 *  @author Elad Sez
 * @email	elad@trego.co.il
 *  ========================================================================================
 */
/*----------------------------------- Standard Includes -----------------------------------*/
#include <string.h>
/*------------------------------------ STMCUbe Includes -----------------------------------*/

/*------------------------------ Block Device Drive Includes ------------------------------*/
#include "bd_driver_flash.h"
#include "stm32h7xx_hal_flash.h"



int bd_driver_flash_createcfg(const struct lfs_config *LFSConfig, const LFS_BD_Flash_Config *FlashBDCfg) {
    LFS_BD_Flash *bd = LFSConfig->context;
    bd->cfg = FlashBDCfg;
    bd->buffer = bd->cfg->Buffer;
    return 0;
}


int bd_driver_flash_init(const struct lfs_config *LFSConfig, const LFS_BD_Flash_Config *FlashBDCfg){
	return bd_driver_flash_createcfg(LFSConfig,FlashBDCfg);
}


int bd_driver_flash_read(const struct lfs_config *LFSConfig, lfs_block_t BlockNo, lfs_off_t Offset, void* Buffer, lfs_size_t ReadSize)
{
	LFS_BD_Flash *bd = LFSConfig->context;
	uint16_t     data;
	uint32_t     idx=0;

	for(idx=0; idx < ReadSize; idx+=2)
	{
		if (!IS_FLASH_PROGRAM_ADDRESS_BANK2((uint32_t *)&bd->buffer[BlockNo*LFSConfig->block_size + Offset + idx])){
			return 1;
		}

		data = *((uint16_t *)((uint32_t)(&bd->buffer[BlockNo*LFSConfig->block_size + Offset + idx])));

		*((uint16_t *)((uint32_t)Buffer +idx)) = data;
	}

	return 0;
}

int bd_driver_flash_prog(const struct lfs_config *LFSConfig, lfs_block_t BlockNo, lfs_off_t Offset, const void* Data, lfs_size_t DataSize)
{
	LFS_BD_Flash *bd = LFSConfig->context;
	uint32_t idx=0;

	if (HAL_FLASH_Unlock() != HAL_OK)
		return 1;

	for(idx=0; idx < DataSize; idx+=2)
	{
		// if(HAL_NOR_Program(&hnor1, (uint32_t *)(&bd->buffer[BlockNo*LFSConfig->block_size + Offset + idx]),
		// 		(uint16_t *)((uint32_t)Data + idx)) != HAL_OK)
		// {
		// 	return 1;
		// }
		// if(HAL_NOR_GetStatus(&hnor1, (uint32_t)&bd->buffer[BlockNo*LFSConfig->block_size + Offset + idx],
		// 		HAL_MAX_DELAY) != HAL_NOR_STATUS_SUCCESS)
		// 	return 1;
		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_FLASHWORD, (uint32_t)(&bd->buffer[BlockNo*LFSConfig->block_size + Offset + idx]),
				*((uint16_t *)((uint32_t)Data + idx))) != HAL_OK)
		{
			return 1;
		}
	}
	return 0;
}

int bd_driver_flash_erase(const struct lfs_config *LFSConfig, lfs_block_t BlockNo)
{
	LFS_BD_Flash *bd = LFSConfig->context;

	if(HAL_NOR_Erase_Block(&hnor1, (uint32_t)&bd->buffer[BlockNo*LFSConfig->block_size], 0) != HAL_OK)
		return 1;

	if(HAL_NOR_GetStatus(&hnor1, (uint32_t)&bd->buffer[BlockNo*LFSConfig->block_size],
			HAL_MAX_DELAY) != HAL_NOR_STATUS_SUCCESS)
		return 1;

	return 0;
}

int bd_driver_flash_sync(const struct lfs_config *LFSConfig)
{
	(void)LFSConfig;

    return 0;
}
