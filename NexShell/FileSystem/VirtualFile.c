#include <string.h>
#include <ctype.h>

#include "NexShell.h"
#include "LinkedList.h"
#include "VirtualFile.h"

// this holds data of type VIRTUAL_FILE_DIRECTORY
static LINKED_LIST gUserVirtualFilesDirectories;

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

static INT32 UserVirtualFileDirectoryCompareTo(const void *Data1, const void *Data2)
{
	VIRTUAL_FILE_DIRECTORY* Directory;

	// get a handle on the data stored in here
	Directory = (VIRTUAL_FILE_DIRECTORY*)Data1;

	// Data2 is what the user passed in
	return strcmp(Directory->DirectoryName, Data2);
}

static INT32 UserVirtualFileCompareTo(const void* Data1, const void* Data2)
{
	VIRTUAL_FILE *File;

	// get a handle on the data stored in here
	File = (VIRTUAL_FILE*)Data1;

	// Data2 is what the user passed in
	return strcmp(File->FileName, Data2);
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
		return NULL;

	FileList = LinkedListGetData(&gUserVirtualFilesDirectories, Index);

	if (FileList == NULL)
		return NULL;

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
	Size = (UINT32)strlen(DirectoryPath);

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

		if (CreateLinkedList(&Directory->VirtualFileList, UserVirtualFileCompareTo, NULL) == NULL)
			return SHELL_LINKED_LIST_CREATE_FAILURE;

		// allocate room for the directory struct
		Directory->DirectoryName = NexShellMalloc(strlen(DirectoryPath) + 1);

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

SHELL_RESULT CreateVirtualFile(VIRTUAL_FILE* NewFileToInitialize, const char *FileName, SHELL_RESULT(*ReadFileData)(PIPE *), SHELL_RESULT(*WriteFileData)(char* [], UINT32 , PIPE *), SHELL_RESULT(*ExecuteFile)(char* [], UINT32 , PIPE *), const char* FileDescription, const char* FileHelp)
{
	// check all incoming parameters
	if (NewFileToInitialize == NULL)
		return SHELL_INVALID_INPUT_PARAMETER;

	if (FileName == NULL)
		return SHELL_INVALID_INPUT_PARAMETER;

	// now, is the name valid?
	if (VirtualFileNameIsValid((char*)FileName) == FALSE)
		return SHELL_INVALID_DIRECTORY_NAME;

	// set the name
	NewFileToInitialize->FileName = (char*)FileName;

	// it was valid, initialize it


	// add the description if present
	NewFileToInitialize->FileDescription = (char*)FileDescription;

	// add the description if present
	NewFileToInitialize->FileHelp = (char*)FileHelp;

	NewFileToInitialize->ReadFileData = ReadFileData;
	NewFileToInitialize->WriteFileData = WriteFileData;
	NewFileToInitialize->ExecuteFile = ExecuteFile;

	return SHELL_SUCCESS;
}

VIRTUAL_FILE* GetVirtualFile(char* FullFilenamePath)
{
	char* Filename;
	VIRTUAL_FILE* VirtualFile;
	UINT32 Index;
	LINKED_LIST* VirtualFileList;
	SHELL_RESULT Result;
	char CurrentWorkingDirectory[SHELL_MAX_DIRECTORY_SIZE_IN_BYTES + 1];

	if (FullFilenamePath == NULL)
		return NULL;

	// get the potential file name beginning
	Filename = strrchr(FullFilenamePath, '/');

	if (Filename != NULL)
	{
		// so it is not just a filename, but also has a path
		Filename++;

		// copy in the path so we can modify it
		strcpy(CurrentWorkingDirectory, FullFilenamePath);

		// knock out the / in the string
		*(char*)strrchr(CurrentWorkingDirectory, '/') = 0;

		// set the directory
		Result = f_chdir(CurrentWorkingDirectory);

		if (Result != SHELL_SUCCESS)
			return NULL;
	}
	else
	{
		Filename = FullFilenamePath;
	}

	// now get the current working directory
	// get the current directory
	Result = f_getcwd(CurrentWorkingDirectory, sizeof(CurrentWorkingDirectory));

	if (Result != SHELL_SUCCESS)
	{
		// set the directory
		f_chdir(NexShellGetCurrentWorkingDirectory());

		return NULL;
	}

	// get the virtual file list
	VirtualFileList = GetVirtualFileList(CurrentWorkingDirectory);

	// get the index of the virtual file in the list
	Index = LinkedListContains(VirtualFileList, Filename, 0, 0);

	// set the directory back to the starting one
	Result = f_chdir(NexShellGetCurrentWorkingDirectory());

	if (Result != SHELL_SUCCESS)
		return NULL;

	// did we get the virtual file?
	if (Index == 0)
		return NULL;

	// get the actual file
	VirtualFile = LinkedListGetData(VirtualFileList, Index);

	return VirtualFile;
}
