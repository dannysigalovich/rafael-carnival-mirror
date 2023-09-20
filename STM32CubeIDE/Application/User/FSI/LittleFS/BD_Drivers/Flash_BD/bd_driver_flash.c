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
	uint32_t     data;
	uint32_t     idx=0;

	for(idx=0; idx < ReadSize; idx+=4)
	{
		if (!IS_FLASH_PROGRAM_ADDRESS_BANK2((uint32_t *)&bd->buffer[BlockNo*LFSConfig->block_size + Offset + idx])){
			return 1;
		}

		data = *(uint32_t*)(&(bd->buffer[BlockNo*LFSConfig->block_size + Offset + idx]));

		*(uint32_t *)(((uint8_t *)Buffer) +idx) = data;
	}

	return 0;
}

int flash_write_helper(const uint32_t *src, uint32_t dst, uint32_t size)
{
    // Unlock flash
    if (HAL_FLASH_Unlock() != HAL_OK) {
		return 1;
	}

    // Program the flash 32 bytes at a time.
    for (int i=0; i<size/32; i++) {
		if (!IS_FLASH_PROGRAM_ADDRESS_BANK2((uint32_t *)dst)){
			// the address is not in the flash of the block device
			HAL_FLASH_Lock(); // lock the flash
			return 1;
		}
        if (HAL_FLASH_Program(TYPEPROGRAM_WORD, dst, (uint64_t)(uint32_t) src) != HAL_OK) {
            // error occurred during flash write
            HAL_FLASH_Lock(); // lock the flash
			return 1;
        }

		if (HAL_FLASH_GetError() != HAL_FLASH_ERROR_NONE) {
			// error occurred during flash write
			HAL_FLASH_Lock(); // lock the flash
			return 1;
		}

        src += 8;
        dst += 32;
    }

    // lock the flash
    HAL_FLASH_Lock();
	return 0;
}

int bd_driver_flash_prog(const struct lfs_config *LFSConfig, lfs_block_t BlockNo, lfs_off_t Offset, const void* Data, lfs_size_t DataSize)
{
	LFS_BD_Flash *bd = LFSConfig->context;
	uint32_t idx=0;

	uint32_t dst = (uint32_t)(&bd->buffer[BlockNo*LFSConfig->block_size + Offset]);
	uint32_t *src = (uint32_t *)Data;

	return flash_write_helper(src, dst, DataSize);
}

/**
  * @brief  Gets the sector of a given address
  * @param  Address: Flash address
  * @retval The sector of a given address
  */
uint32_t get_sector(uint32_t address)
{
    uint32_t sector = 0;

    if (address < (FLASH_BASE + FLASH_BANK_SIZE)) {
        sector = (address - FLASH_BASE) / FLASH_SECTOR_SIZE;
    } else {
        sector = (address - (FLASH_BASE + FLASH_BANK_SIZE)) / FLASH_SECTOR_SIZE;
    }

    return sector;
}

int bd_driver_flash_erase(const struct lfs_config *LFSConfig, lfs_block_t BlockNo)
{
	LFS_BD_Flash *bd = LFSConfig->context;

	FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t SectorError = 0;

	EraseInitStruct.TypeErase   = FLASH_TYPEERASE_SECTORS;
	EraseInitStruct.Banks       = FLASH_BANK_2;
	EraseInitStruct.Sector      = get_sector((uint32_t)&bd->buffer[BlockNo*LFSConfig->block_size]);
	EraseInitStruct.NbSectors   = 1;
	EraseInitStruct.VoltageRange= FLASH_VOLTAGE_RANGE_3;

	if (EraseInitStruct.Sector == 0xFFFFFFFF || HAL_FLASH_Unlock() != HAL_OK) {
		return 1;
	}

	if (HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK) {
		HAL_FLASH_Lock();
		return 1;
	}

	if (HAL_FLASH_GetError() != HAL_FLASH_ERROR_NONE || SectorError != 0xFFFFFFFF) {
		HAL_FLASH_Lock();
		return 1;
	}

	HAL_FLASH_Lock();

	return 0;
}

int bd_driver_flash_sync(const struct lfs_config *LFSConfig)
{
	(void)LFSConfig;
	return FLASH_WaitForLastOperation(HAL_MAX_DELAY, FLASH_BANK_2);
}
