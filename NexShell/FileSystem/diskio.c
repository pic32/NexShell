/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/
#include <string.h>
#include <stdio.h>

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */
#include "GenericTypes.h"

/* Definitions of physical drive number for each drive */
#define DEV_RAM		0	/* Example: Map Ramdisk to physical drive 0 */
#define DEV_MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
#define DEV_USB		2	/* Example: Map USB MSD to physical drive 2 */

FILE *gDiskContentsFile = NULL;
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
			if (gDiskContentsFile != NULL)
				return 0;

			// open the file, this assumes it exists
			gDiskContentsFile = fopen("Disk.txt", "r+");

			if (gDiskContentsFile == NULL)
			{
				// we must make the file
				gDiskContentsFile = fopen("Disk.txt", "w+");

				if (gDiskContentsFile == NULL)
				{
					// error 
					return STA_NOINIT;
				}

				// now initialize the contents, make it look like flash
				memset(gRAMBuffer, 0xFF, sizeof(gRAMBuffer));

				if (fwrite(gRAMBuffer, 1, sizeof(gRAMBuffer), gDiskContentsFile) != sizeof(gRAMBuffer))
				{
					// some kind of disk error
					fclose(gDiskContentsFile);

					// error 
					return STA_NOINIT;
				}

				// now go back to the beginning
				if (fseek(gDiskContentsFile, 0, SEEK_SET) != 0)
				{
					// some kind of disk error
					fclose(gDiskContentsFile);

					// error 
					return STA_NOINIT;
				}
			}

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
				if (fseek(gDiskContentsFile, (sector + i) * 512, SEEK_SET) != 0)
				{
					// some kind of disk error
					fclose(gDiskContentsFile);

					// error 
					return RES_ERROR;
				}

				if (fread(buff, 1, 512, gDiskContentsFile) != 512)
				{
					// some kind of disk error
					fclose(gDiskContentsFile);

					// error 
					return RES_ERROR;
				}

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
				// go to the sector
				if (fseek(gDiskContentsFile, (sector + i) * 512, SEEK_SET) != 0)
				{
					// some kind of disk error
					fclose(gDiskContentsFile);

					// error 
					return RES_ERROR;
				}

				if (fwrite(buff, 1, 512, gDiskContentsFile) != 512)
				{
					// some kind of disk error
					fclose(gDiskContentsFile);

					// error 
					return RES_ERROR;
				}

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

