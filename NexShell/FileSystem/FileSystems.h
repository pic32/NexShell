#ifndef FILE_SYSTEMS_H
    #define FILE_SYSTEMS_H

/** \file
 * This contains an interface to some simple file opeations.
 */

#include "ff.h"
#include "LinkedList.h"
#include "GenericTypes.h"

// The size of a sector
// Must be 512, 1024, 2048, or 4096
// 512 bytes is the value used by most cards
#define MEDIA_SECTOR_SIZE_IN_BYTES                  FF_MAX_SS
#define MAX_FILENAME_LENGTH_IN_BYTES                FF_MAX_LFN
#define FILE_LIST_ROOT_DIRECTORY_SIZE_IN_BYTES      (FF_MAX_LFN + 1) 

/*!
* \enum DISK_SPACE_TYPE
* This is used to denote which type of space needs to be calculated in get space methods.
*/
typedef enum 
{
    /*! Get total space on the disk. */
    TOTAL_DISK_SPACE = 0,

    /*! Get total free space on the disk. */
    FREE_DISK_SPACE,

    /*! Get total disk space used on the disk. */
    USED_DISK_SPACE,

    /*! Used internally by the system */
    NUMBER_OF_DISK_SPACES
}DISK_SPACE_TYPE;

/*!
* \enum DISK_SPACE_UNITS
* This is used to denote which type of units to calculate the space in when calling any of the get space methods..
*/
typedef enum 
{
    /*! Get the disk space in bytes. */
    DISK_SPACE_BYTES = 0,

    /*! Get the disk space in kilobytes. */
    DISK_SPACE_KB,

    /*! Get the disk space in megabytes. */
    DISK_SPACE_MB,

    /*! Get the disk space in gigabytes. */
    DISK_SPACE_GB,

    /*! Used internally by the system */
    NUMBER_OF_DISK_SPACE_UNITS
}DISK_SPACE_UNITS;

/*!
 * @brief Gets the disk space of the specified drive.
 *
 * @param[in] char *Directory - The volume drive to get the space of.  This is something like "R:/".
 * @param[in] DISK_SPACE_TYPE DiskSpaceType - The type of disk space to get the value of.
 * @param[in] DISK_SPACE_UNITS DiskSpaceUnits - The units to get the space in.
 * @param[in/out] UINT64 *Size - The calculated empty space is written here upon success of the method.
 *
 * @return FRESULT - The result of the operation.  If anything but FR_OK is returned, the value in *Size
 * cannot be considered valid.
 *
 * @details
 * This function is called to find various types of space on a drive.  The drive must have been
 * initialized with a call to \ref f_mount() prior to calling this method.  This method will calculate
 * the empty space down to the nearest 1,024 byte chunk.  To get the exact space down to the byte use the
 * method GetDiskSpacePrecise().
 * 
 * @sa GetDiskSpacePrecise()
 */
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