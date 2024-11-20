#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "FileSystems.h"

const BYTE* gNewLineStrings[] = {
    (const BYTE*)"\n",
    (const BYTE*)"\r\n"
};

static FRESULT GetDriveSizeInBytes(BYTE* Drive, UINT64* Size)
{
    FATFS* FS;
    FRESULT FResult;
    DWORD FreeClusters;

    /* Get volume information and free clusters of drive 1 */
    FResult = f_getfree((const TCHAR*)Drive, &FreeClusters, &FS);

    if (FResult != FR_OK)
        return FR_INVALID_PARAMETER;

    /* Get total sectors and free sectors */
    *Size = (UINT64)((FS->n_fatent - 2) * FS->csize);

    // now normalize for the size of a sector
    *Size *= MEDIA_SECTOR_SIZE_IN_BYTES;

    return FR_OK;
}

static FRESULT GetDriveFreeSpaceInBytes(BYTE* Drive, UINT64* FreeSpace)
{
    FATFS* FS;
    FRESULT FResult;
    DWORD FreeClusters;

    /* Get volume information and free clusters of drive 1 */
    FResult = f_getfree((const TCHAR*)Drive, &FreeClusters, &FS);

    if (FResult != FR_OK)
        return FR_INVALID_PARAMETER;

    /* Get total sectors and free sectors */
    *FreeSpace = FreeClusters * FS->csize;

    // now normalize for the size of a sector
    *FreeSpace *= MEDIA_SECTOR_SIZE_IN_BYTES;

    return FR_OK;
}

static FRESULT GetDriveUsedSpaceInBytes(BYTE* Drive, UINT64* UsedSpace)
{
    UINT64 FreeSpace, Size;
    FRESULT FResult;

    // get the overall size
    FResult = GetDriveSizeInBytes(Drive, &Size);

    if (FResult != FR_OK)
        return FResult;

    // get how many bytes are free
    FResult = GetDriveFreeSpaceInBytes(Drive, &FreeSpace);

    if (FResult != FR_OK)
        return FResult;

    // now subtract the two
    *UsedSpace = Size - FreeSpace;

    return FR_OK;
}

static FRESULT GetDiskSpaceBytes(BYTE* Drive, DISK_SPACE_TYPE DiskSpaceType, UINT64* Size)
{
    switch (DiskSpaceType)
    {
    case TOTAL_DISK_SPACE:
        return GetDriveSizeInBytes(Drive, Size);

    case FREE_DISK_SPACE:
        return GetDriveFreeSpaceInBytes(Drive, Size);

    case USED_DISK_SPACE:
        return GetDriveUsedSpaceInBytes(Drive, Size);

    default:
    case NUMBER_OF_DISK_SPACES:
        return FR_INVALID_PARAMETER;
    }
}

BOOL ContainsNewLine(BYTE* Buffer)
{
    UINT32 i;

    for (i = 0; i < sizeof(gNewLineStrings) / sizeof(const BYTE*); i++)
    {
        if (strstr((char*)Buffer, (char*)gNewLineStrings[i]) != (char*)NULL)
            return TRUE;
    }

    return FALSE;
}

BOOL FileReadLine(FIL* File, BYTE* Buffer, UINT32 BufferSize)
{
    UINT32 i;
    UINT BytesRead;

    // clear the buffer out first
    memset((void*)Buffer, 0, BufferSize);

    //  now read 1 byte at a time looking for the new line sequence
    for (i = 0; i < BufferSize - 1; i++)
    {
        // read in 1 byte
        if (f_read(File, &Buffer[i], 1, &BytesRead) != FR_OK)
            return FALSE;

        if (BytesRead != 1)
            return FALSE;

        // does the buffer contain a new line?
        if (ContainsNewLine(Buffer) == TRUE)
            return TRUE;
    }

    return FALSE;
}

BOOL FileExists(BYTE* FilePath)
{
    FIL File;
    FRESULT FResult;

    // check the file path to make sure it's safe
    if (FilePath == NULL)
        return FALSE;

    // just open the file
    FResult = f_open(&File, (const TCHAR*)FilePath, FA_OPEN_EXISTING);

    // now close it
    if (FResult == FR_OK)
        f_close(&File);

    // return the result
    return (BOOL)(FResult == FR_OK);
}

BOOL DirectoryExists(BYTE* DirectoryPath)
{
    DIR Directory;
    FRESULT FResult;

    // check the file path to make sure it's safe
    if (DirectoryPath == NULL)
        return FALSE;

    // just open the file
    FResult = f_opendir(&Directory, (const TCHAR*)DirectoryPath);

    // now close it
    if (FResult == FR_OK)
        f_closedir(&Directory);

    // return the result
    return (BOOL)(FResult == FR_OK);
}

FRESULT GetDiskSpace(BYTE* Drive, DISK_SPACE_TYPE DiskSpaceType, DISK_SPACE_UNITS DiskSpaceUnits, UINT64* Size)
{
    FRESULT FResult;

    if (Drive == NULL || Size == NULL)
        return FR_INVALID_PARAMETER;

    if (DiskSpaceType >= NUMBER_OF_DISK_SPACES)
        return FR_INVALID_PARAMETER;

    if (DiskSpaceUnits >= NUMBER_OF_DISK_SPACE_UNITS)
        return FR_INVALID_PARAMETER;

    FResult = GetDiskSpaceBytes(Drive, DiskSpaceType, Size);

    if (FResult != FR_OK)
        return FResult;

    while (DiskSpaceUnits-- != DISK_SPACE_BYTES)
        *Size /= (UINT64)1024;

    return FR_OK;
}

FRESULT GetDiskSpacePrecise(BYTE* Drive, DISK_SPACE_TYPE DiskSpaceType, DISK_SPACE_UNITS DiskSpaceUnits, FLOAT64* Size)
{
    UINT64 DiskSize;
    FRESULT FResult;

    if (Drive == NULL || Size == NULL)
        return FR_INVALID_PARAMETER;

    if (DiskSpaceType >= NUMBER_OF_DISK_SPACES)
        return FR_INVALID_PARAMETER;

    if (DiskSpaceUnits >= NUMBER_OF_DISK_SPACE_UNITS)
        return FR_INVALID_PARAMETER;

    FResult = GetDiskSpaceBytes(Drive, DiskSpaceType, &DiskSize);

    if (FResult != FR_OK)
        return FResult;

    *Size = (FLOAT64)DiskSize;

    while (DiskSpaceUnits-- != DISK_SPACE_BYTES)
        *Size /= (FLOAT64)1024.0;

    return FR_OK;
}

BOOL FileNamesMatch(const BYTE* Filename1, const BYTE* Filename2, BOOL FileExtensionCaseSensitive)
{
    // are they the same size
    if (strlen((const char*)Filename1) != strlen((const char*)Filename2))
        return FALSE;

    // if case sensitive, just compare the strings
    if (FileExtensionCaseSensitive == TRUE)
    {
        return (BOOL)(strncmp((const char*)Filename1, (const char*)Filename2, strlen((const char*)Filename1)) == 0);
    }
    else
    {
        // else we will compare the names, and then case insensitive the file extension
        BYTE* Filename1Period, * Filename2Period;

        // is there a period in the filename?
        if ((Filename1Period = (BYTE*)strchr((const char*)Filename1, (int)'.')) == (BYTE*)NULL)
            return FALSE;

        // is there a period in the filename?
        if ((Filename2Period = (BYTE*)strchr((const char*)Filename2, (int)'.')) == (BYTE*)NULL)
            return FALSE;

        // both filenames have a period, are they in the same spot?
        if (Filename1Period - Filename1 != Filename2Period - Filename2)
            return FALSE;

        // they both have a period in the same place

        // now compare all chars up until the period
        if (strncmp((const char*)Filename1, (const char*)Filename2, strlen((const char*)Filename1) - strlen((const char*)Filename1Period)) != 0)
            return FALSE;

        // now check the file extension in a case insensitive manner
        do
        {
            if (*Filename1Period != (BYTE)toupper((int)*Filename2Period) && *Filename1Period != (BYTE)tolower((int)*Filename2Period))
                return FALSE;

            // increment the pointers
            Filename1Period++;
            Filename2Period++;
        } 
        while (*Filename1Period != 0);

        return TRUE;
    }
}

BOOL GetFileInfo(BYTE* FullFilename, FILINFO* FileInfo)
{
    UINT32 i;
    DIR Directory;
    BOOL ForwardSlashFound = FALSE;

    if (FullFilename == (BYTE*)NULL)
        return FALSE;

    if (FileInfo == (FILINFO*)NULL)
        return FALSE;

    // we're going to get the date time first before modifying the file extension
    for (i = strlen((char*)FullFilename); i > 0; i--)
    {
        // did we find the beginning of the file extension?
        if (FullFilename[i] == '\\')
        {
            // make this null and redo the file extension
            FullFilename[i] = 0;

            ForwardSlashFound = TRUE;

            break;
        }
    }

    // if no extension was found, leave
    if (ForwardSlashFound == FALSE)
        return FALSE;

    // open the directory path
    if (f_opendir(&Directory, (const TCHAR*)FullFilename) != FR_OK)
        return FALSE;

    // now put this back
    FullFilename[i] = '\\';

    // now start to search through for the file in question
    do
    {
        // read in the next item
        if (f_readdir(&Directory, FileInfo) != FR_OK)
        {
            f_closedir(&Directory);

            return FALSE;
        }

        // if it is a null string, the end has been reached
        if (FileInfo->fname[0] == 0)
        {
            f_closedir(&Directory);

            return FALSE;
        }
    } 
    while (FileNamesMatch((const BYTE*)&FullFilename[i + 1], (const BYTE*)FileInfo->fname, FALSE) == FALSE);

    if (f_closedir(&Directory) != FR_OK)
        return FALSE;

    return TRUE;
}
