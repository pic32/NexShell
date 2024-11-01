#ifndef VIRTUAL_DIRECTORY_H
	#define VIRTUAL_DIRECTORY_H

#include "NexShell.h"
#include "LinkedList.h"
#include "VirtualObjects.h"

typedef enum
{
	PHSYCIAL_MEDIA = 0,
	VIRTUAL_MEDIA,
	TRANSITION_POINT_MEDIA,
	NUMBER_OF_DIRECTORY_TYPES
}DIRECTORY_TYPE;

BOOL IsDirectoryVirtual(const char* FullFilePath);

BOOL IsDirectoryAtVirtualTransition(const char* FullFilePath);

BOOL DirectoryExists(char* DirectoryPath);

SHELL_RESULT VirtualDirectoryGetPath(VIRTUAL_DIRECTORY* DirectoryToGetPath, char* PathBuffer);

VIRTUAL_DIRECTORY* GenerateRootVirtualDirectory(void);

SHELL_RESULT CreateVirtualDirectory(char* ParentDirectory, VIRTUAL_DIRECTORY* NewDirectoryToInitialize, char *DirectoryName);

VIRTUAL_DIRECTORY* VirtualDirectoryNameExists(VIRTUAL_DIRECTORY* Directory, char* DirectoryNameToFind);

VIRTUAL_DIRECTORY*FollowVirtualDirectory(char *Buffer, VIRTUAL_DIRECTORY*RootDir, VIRTUAL_DIRECTORY*CurrentDir, BOOL *Outcome);

char *GetNextVirtualDirectorySection(char *Buffer, char *DirectoryHolder, UINT32 DirectoryHolderSizeInBytes);

UINT32 VirtualDirectoryGetNumberOfDirectories(VIRTUAL_DIRECTORY* Directory);

UINT32 VirtualDirectoryGetNumberOfFiles(VIRTUAL_DIRECTORY* Directory);

VIRTUAL_DIRECTORY* VirtualDirectoryGetDirectory(VIRTUAL_DIRECTORY* Directory, UINT32 DirectoryIndex);

VIRTUAL_FILE* VirtualDirectoryGetFile(VIRTUAL_DIRECTORY* Directory, UINT32 FileIndex);

#endif // end of #ifndef VIRTUAL_DIRECTORY_H
