#ifndef FILE_SYSTEMS_H
    #define FILE_SYSTEMS_H

#include "ff.h"
#include "LinkedList.h"
#include "GenericTypes.h"

// The size of a sector
// Must be 512, 1024, 2048, or 4096
// 512 bytes is the value used by most cards
#define MEDIA_SECTOR_SIZE_IN_BYTES                  FF_MAX_SS
#define MAX_FILENAME_LENGTH_IN_BYTES                FF_MAX_LFN
#define FILE_LIST_ROOT_DIRECTORY_SIZE_IN_BYTES      (FF_MAX_LFN + 1) 

#define FileSystemMemAlloc(Mem)                     malloc(Mem)
#define FileSystemMemDealloc(Mem)                   free(Mem)

#define DEFAULT_MEDIA_VOLUME                        FLASH_VOLUME

typedef enum 
{
    TOTAL_DISK_SPACE = 0,
    FREE_DISK_SPACE,
    USED_DISK_SPACE,
    NUMBER_OF_DISK_SPACES
}DISK_SPACE_TYPE;

typedef enum 
{
    DISK_SPACE_BYTES = 0,
    DISK_SPACE_KB,
    DISK_SPACE_MB,
    DISK_SPACE_GB,
    NUMBER_OF_DISK_SPACE_UNITS
}DISK_SPACE_UNITS;

typedef enum 
{
    SD_CARD_VOLUME = 0,
    USB_VOLUME,
    FLASH_VOLUME,
    NUMBER_OF_MEDIA_VOLUME_NAMES
}MEDIA_VOLUME_NAME;

// Tested --------------------------------------------------------------------------------------------
FRESULT GetDiskSpace(BYTE *Drive, DISK_SPACE_TYPE DiskSpaceType, DISK_SPACE_UNITS DiskSpaceUnits, UINT64 *Size);

FRESULT GetDiskSpacePrecise(BYTE *Drive, DISK_SPACE_TYPE DiskSpaceType, DISK_SPACE_UNITS DiskSpaceUnits, FLOAT64 *Size);

BOOL FileNamesMatch(const BYTE *Filename1, const BYTE *Filename2, BOOL FileExtensionCaseSensitive);

BOOL ContainsNewLine(BYTE *Buffer);

BOOL FileReadLine(FIL *File, BYTE *Buffer, UINT32 BufferSize);
//----------------------------------------------------------------------------------------------------



// Implemented----------------------------------------------------------------------------------------
BOOL FileExists(BYTE *FilePath);

BOOL DirectoryExists(BYTE *DirectoryPath);

BOOL GetFileInfo(BYTE *FullFilename, FILINFO *FileInfo);
//----------------------------------------------------------------------------------------------------


#endif // end of #ifdef FILE_SYSTEMS_H