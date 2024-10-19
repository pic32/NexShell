/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/
#include <string.h>

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */
#include "GenericTypeDefs.h"

/* Definitions of physical drive number for each drive */
#define DEV_RAM		3	/* Example: Map Ramdisk to physical drive 0 */
#define DEV_MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
#define DEV_USB		2	/* Example: Map USB MSD to physical drive 2 */

BYTE gRAMBuffer[1024 * 1024 * 32];

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat;
	int result;

	switch (pdrv) 
	{
		case DEV_RAM :
		{
			// translate the reslut code here

			return 0;
		}
	}

	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat;
	int result;

	switch (pdrv)
	{
		case DEV_RAM:
		{
			memset(gRAMBuffer, 0, sizeof(gRAMBuffer));

			// translate the reslut code here

			return 0;
		}
	}

	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	LBA_t sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	DRESULT res;
	int result;

	switch (pdrv)
	{
		case DEV_RAM:
		{
			UINT32 i;

			for (i = 0; i < count; i++)
			{
				memcpy(buff, &gRAMBuffer[(sector + i) * 512], 512);

				buff += 512;
			}

			// translate the reslut code here

			return RES_OK;
		}
	}

	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	LBA_t sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	DRESULT res;
	int result;

	switch (pdrv)
	{
		case DEV_RAM:
		{
			UINT32 i;

			for (i = 0; i < count; i++)
			{
				memcpy(&gRAMBuffer[(sector + i) * 512], buff, 512);

				buff += 512;
			}

			return RES_OK;
		}
	}

	return RES_PARERR;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res = RES_PARERR;

	switch (pdrv)
	{
		case DEV_RAM:
		{
			switch (cmd)
			{
				case CTRL_SYNC:			/* Nothing to do */
					res = RES_OK;
					break;

				case GET_SECTOR_COUNT:	/* Get number of sectors on the drive */
					*(LBA_t*)buff = sizeof(gRAMBuffer);
					res = RES_OK;
					break;

				case GET_SECTOR_SIZE:	/* Get size of sector for generic read/write */
					*(WORD*)buff = 1024;
					res = RES_OK;
					break;

				case GET_BLOCK_SIZE:	/* Get internal block size in unit of sector */
					*(DWORD*)buff = 4096;
					res = RES_OK;
					break;
			}
		}
	}

	return res;
}

