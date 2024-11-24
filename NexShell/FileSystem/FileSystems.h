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
 * @brief Gets the disk space of the specified drive down to the nearest 1,024 bytes.
 *
 * @param[in] char *Directory - The volume drive to get the space of.  This is something like "R:/".
 * @param[in] DISK_SPACE_TYPE DiskSpaceType - The type of disk space to get the value of.
 * @param[in] DISK_SPACE_UNITS DiskSpaceUnits - The units to get the space in.
 * @param[in/out] UINT64 *Size - The calculated space is written here upon success of the method.
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

/*!
 * @brief Gets the disk space of the specified drive.
 *
 * @param[in] char *Directory - The volume drive to get the space of.  This is something like "R:/".
 * @param[in] DISK_SPACE_TYPE DiskSpaceType - The type of disk space to get the value of.
 * @param[in] DISK_SPACE_UNITS DiskSpaceUnits - The units to get the space in.
 * @param[in/out] FLOAT64 *Size - The calculated space is written here upon success of the method.
 *
 * @return FRESULT - The result of the operation.  If anything but FR_OK is returned, the value in *Size
 * cannot be considered valid.
 *
 * @details
 * This function is called to find various types of space on a drive.  The drive must have been
 * initialized with a call to \ref f_mount() prior to calling this method.  This method will calculate
 * the empty space down to the nearest byte.  To get the rough disk space down to the nearest 
 * 1,024 bytes use the \ref method GetDiskSpace().
 *
 * @sa GetDiskSpace()
 */
FRESULT GetDiskSpacePrecise(BYTE *Drive, DISK_SPACE_TYPE DiskSpaceType, DISK_SPACE_UNITS DiskSpaceUnits, FLOAT64 *Size);

/*!
 * @brief Compares two strings to see if the are equal.
 *
 * @param[in] const BYTE *Filename1 - A pointer to a filename string to compare.
 * @param[in] const BYTE *Filename2 - A pointer to a filename string to compare.
 * @param[in] BOOL FileExtensionCaseSensitive - If TRUE the filenames case must match, and a 
 * period must exist in the names at the same spot.  If FALSE the filenames case doesn't need
 * to match and no period is needed.
 *
 * @return BOOL - TRUE if the filenames match, FALSE otherwise.
 *
 * @details
 * This method compares 2 strings to see if they are matching.  If the case must match set
 * FileExtensionCaseSensitive to TRUE.  In this mode a period must exist in each name.
 */
BOOL FileNamesMatch(const BYTE *Filename1, const BYTE *Filename2, BOOL FileExtensionCaseSensitive);

/*!
 * @brief Searches a buffer to find a new line sequence.
 *
 * @param[in] char *Buffer - A char pointer to an array that will be searched for a new line.
 *
 * @return BOOL - TRUE if the buffer has a new line character in it, FALSE otherwise.
 *
 * @details
 * This method searches the string in the buffer to see if a new line sequence is available.
 * If it is TRUE is returned.  Any other outcome returns FALSE.
 */
BOOL ContainsNewLine(char *Buffer);

/*!
 * @brief Searches for a specific file.
 *
 * @param[in] char *FilePath - A pointer to a full file path and name.  An example would
 * be something like "R:/directory/filename.txt".
 *
 * @return BOOL - TRUE if the file exists, FALSE otherwise.
 *
 * @details
 * This method opens an existing file with the specified path.  If the file opens successfully
 * then TRUE is returned.  After opening the existing file it is always closed.
 */
BOOL FileExists(char *FilePath);

/*!
 * @brief Searches for a specific directory.
 *
 * @param[in] char *DirectoryPath - A pointer to a full file path and name.  An example would
 * be something like "R:/directory".
 *
 * @return BOOL - TRUE if the directory exists, FALSE otherwise.
 *
 * @details
 * This method opens an existing directory with the specified path.  If the directory opens 
 * and closes successfully then TRUE is returned.  After opening the existing directory it is 
 * always closed.
 */
BOOL DirectoryExists(char *DirectoryPath);

/*!
 * @brief Searches for a specific file and gets the FILINFO of that file.
 *
 * @param[in] char *FullFilename - A pointer to a full file path and name.  An example would
 * be something like "R:/directory/filename.txt".
 * 
 * @param[in/out] FILINFO *FileInfo - An empty FILINFO structure to put the file information
 * into once found.
 *
 * @return BOOL - TRUE if the FILINFO was successfully read, FALSE otherwise.
 *
 * @details
 * This method reads the associated FILINFO data into the structure from the specified file.
 */
BOOL GetFileInfo(char *FullFilename, FILINFO *FileInfo);

#endif // end of #ifdef FILE_SYSTEMS_H