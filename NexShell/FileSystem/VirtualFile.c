#include <string.h>
#include <ctype.h>

#include "NexShell.h"
#include "LinkedList.h"
#include "VirtualFile.h"

// this holds data of type VIRTUAL_FILE_DIRECTORY
LINKED_LIST gUserVirtualFilesDirectories;

static BOOL VirtualFileCharacterIsValid(char Character)
{
	// we can't have spaces
	if (isgraph((int)Character) == 0)
		return FALSE;

	if (Character == '\\' || Character == '/' || Character == ' ')
		return FALSE;

	return TRUE;
}

static BOOL VirtualDirectoryCharacterIsValid(char Character)
{
	// we can't have spaces
	if (isgraph((int)Character) == 0)
		return FALSE;

	if (Character == ' ')
		return FALSE;

	return TRUE;
}

static BOOL VirtualFileNameIsValid(char* Buffer)
{
	if (*Buffer == 0)
		return FALSE;

	while (1)
	{
		if (*Buffer == 0)
			return TRUE;

		if (VirtualFileCharacterIsValid(*Buffer++) == FALSE)
			return FALSE;
	}
}

VIRTUAL_FILE* VirtualFileNameExists(LINKED_LIST *FileList, char* FileNameToFind)
{
	UINT32 i, Size;

	if (FileList == NULL || FileNameToFind == NULL)
		return NULL;

	Size = LinkedListGetSize(FileList);

	for (i = 1; i <= Size; i++)
	{
		VIRTUAL_FILE* FileNodeToCompare = LinkedListGetData(FileList, i);

		// compare the names
		if (strcmp(FileNodeToCompare->FileName, FileNameToFind) == 0)
			return FileNodeToCompare;
	}

	return NULL;
}

static INT32 UserVirtualFileDirectoryCompareTo(const void *Data1, const void *Data2)
{
	VIRTUAL_FILE_DIRECTORY* Directory;

	// get a handle on the data stored in here
	Directory = (VIRTUAL_FILE_DIRECTORY*)Data1;

	// Data2 is what the user passed in
	return strcmp(Directory->DirectoryName, Data2);
}

SHELL_RESULT VirtualFileInit(void)
{
	if (CreateLinkedList(&gUserVirtualFilesDirectories, UserVirtualFileDirectoryCompareTo, NULL) == FALSE)
		return SHELL_LINKED_LIST_CREATE_FAILURE;

	return SHELL_SUCCESS;
}

LINKED_LIST* GetVirtualFileList(char* Directory)
{
	VIRTUAL_FILE_DIRECTORY* FileList;
	UINT32 Index;

	Index = LinkedListContains(&gUserVirtualFilesDirectories, Directory, 0, 0);

	// did we find it?
	if (Index == 0)
		return SHELL_LINKED_LIST_OPERATION_FAILURE;

	FileList = LinkedListGetData(&gUserVirtualFilesDirectories, Index);

	if (FileList == NULL)
		return SHELL_LINKED_LIST_OPERATION_FAILURE;

	return &FileList->VirtualFileList;
}

SHELL_RESULT VirtualFileAddToVirtualFileSystem(VIRTUAL_FILE *VirtualFile, char *DirectoryPath)
{
	UINT32 i, Size;
	VIRTUAL_FILE_DIRECTORY* Directory;

	if (DirectoryPath == NULL)
		return SHELL_INVALID_ARGUMENT;

	if(VirtualFile == NULL)
		return SHELL_INVALID_ARGUMENT;

	// do we have a valid directory name?
	Size = strlen(DirectoryPath);

	for (i = 0; i < Size; i++)
	{
		if (VirtualDirectoryCharacterIsValid(DirectoryPath[i]) == FALSE)
			return SHELL_INVALID_DIRECTORY_NAME;
	}

	// first see if the directory already exists
	if (LinkedListContains(&gUserVirtualFilesDirectories, DirectoryPath, 0, 0) == FALSE)
	{
		// we need to add the directory
		Directory = NexShellMalloc(sizeof(VIRTUAL_FILE_DIRECTORY));

		// did we get it?
		if (Directory == NULL)
			return SHELL_MALLOC_FAILURE;

		if (CreateLinkedList(&Directory->VirtualFileList, NULL, NULL) == NULL)
			return SHELL_LINKED_LIST_CREATE_FAILURE;

		// allocate room for the directory struct
		Directory->DirectoryName = NexShellMalloc(strlen(Directory) + 1);

		// did we get the space?
		if (Directory->DirectoryName == NULL)
		{
			NexShellFreeMethod(Directory);

			return SHELL_MALLOC_FAILURE;
		}

		// now copy the data over
		strcpy(Directory->DirectoryName, DirectoryPath);

		// now add it to our list
		if (LinkedListAdd(&gUserVirtualFilesDirectories, Directory) == FALSE)
		{
			// free the space
			NexShellFreeMethod(Directory->DirectoryName);
			NexShellFreeMethod(Directory);

			return SHELL_LINKED_LIST_OPERATION_FAILURE;
		}
	}

	// now we know the node exists to add this file, so add it, first we have to find it
	i = LinkedListContains(&gUserVirtualFilesDirectories, DirectoryPath, 0, 0);

	if(i == 0)
		return SHELL_LINKED_LIST_OPERATION_FAILURE;

	Directory = LinkedListGetData(&gUserVirtualFilesDirectories, i);

	if (Directory == NULL)
		return SHELL_LINKED_LIST_OPERATION_FAILURE;

	// now we have the directory, add the file to it
	if (LinkedListAdd(&Directory->VirtualFileList, VirtualFile) == FALSE)
		return SHELL_LINKED_LIST_OPERATION_FAILURE;

	return SHELL_SUCCESS;
}

SHELL_RESULT CreateVirtualFile(VIRTUAL_FILE* NewFileToInitialize, char *FileName, SHELL_RESULT(*ReadFileData)(GENERIC_BUFFER *), SHELL_RESULT(*WriteFileData)(char* [], UINT32 , GENERIC_BUFFER *), SHELL_RESULT(*ExecuteFile)(char* [], UINT32 , GENERIC_BUFFER *)

	#if (USE_VIRTUAL_FILE_DESCRIPTION == 1)
		, char* FileDescription
	#endif // end of #if (USE_FILE_DESCRIPTION == 1)

	#if (USE_VIRTUAL_FILE_HELP == 1)
		, char* FileHelp
	#endif // end of #if (USE_FILE_HELP == 1)

)
{
	// check all incoming parameters
	if (NewFileToInitialize == NULL)
		return SHELL_INVALID_INPUT_PARAMETER;

	if (FileName == NULL)
		return SHELL_INVALID_INPUT_PARAMETER;

	// now, is the name valid?
	if (VirtualFileNameIsValid(FileName) == FALSE)
		return SHELL_INVALID_DIRECTORY_NAME;

	// set the name
	NewFileToInitialize->FileName = FileName;

	// it was valid, initialize it


	// add the description if present
	#if (USE_VIRTUAL_FILE_DESCRIPTION == 1)
		NewFileToInitialize->FileDescription = FileDescription;
	#endif // end of #if (USE_FILE_DESCRIPTION == 1)

		// add the description if present
	#if (USE_VIRTUAL_FILE_HELP == 1)
		NewFileToInitialize->FileHelp = FileHelp;
	#endif // end of #if (USE_FILE_HELP == 1)

	NewFileToInitialize->ReadFileData = ReadFileData;
	NewFileToInitialize->WriteFileData = WriteFileData;
	NewFileToInitialize->ExecuteFile = ExecuteFile;

	return SHELL_SUCCESS;
}