#include <string.h>
#include <ctype.h>

#include "NexShell.h"
#include "VirtualObjects.h"
#include "VirtualDirectory.h"

// this is our root directory
extern VIRTUAL_DIRECTORY *gCurrentWorkingVirtualDirectory;
extern VIRTUAL_DIRECTORY gRootVirtualDirectory;

static BOOL VirtualFileCharacterIsValid(char Character)
{
	// we can't have spaces
	if (isgraph((int)Character) == 0)
		return FALSE;

	if (Character == '\\' || Character == '/' || Character == ' ')
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

VIRTUAL_FILE* VirtualFileNameExists(VIRTUAL_DIRECTORY* Directory, char* FileNameToFind)
{
	UINT32 i, Size;

	if (Directory == NULL || FileNameToFind == NULL)
		return NULL;

	Size = LinkedListGetSize(&Directory->Files);

	for (i = 1; i <= Size; i++)
	{
		VIRTUAL_FILE* FileNodeToCompare = LinkedListGetData(&Directory->Files, i);

		// compare the names
		if (strcmp(FileNodeToCompare->FileName, FileNameToFind) == 0)
			return FileNodeToCompare;
	}

	return NULL;
}

VIRTUAL_FILE* VirtualShellGetWorkingFile(char *Buffer, VIRTUAL_DIRECTORY*RootDir, VIRTUAL_DIRECTORY*CurrentDir)
{
	VIRTUAL_DIRECTORY*WorkingDirectory;
	char TempBuffer[SIZE_OF_SHELL_STACK_BUFFER_IN_BYTES + 1];

	if (Buffer == NULL)
		return NULL;

	// now are we relative or absolute for the path?
	if (*Buffer == '/' || *Buffer == '\\')
	{
		// they must have passed in a valid directory
		if (RootDir == NULL)
			return NULL;

		// we are absolute
		WorkingDirectory = RootDir;

		// iterate beyond the slashes
		Buffer++;
	}
	else
	{
		// did they pass in a valid directory?
		if (CurrentDir == NULL)
			return NULL;

		// we are relative
		WorkingDirectory = CurrentDir;
	}

	// now follow the directory
	while (1)
	{
		// get the next portion of the directory
		// it'll look like folder1/folder2/textfile
		// and we will return folder1\0 in TempBuffer
		// and Buffer will be pointing to folder2/textfile 
		if ((Buffer = GetNextVirtualDirectorySection(Buffer, TempBuffer, sizeof(TempBuffer))) == NULL)
			return NULL;

		// did we get our last string?
		if (*Buffer == 0)
		{
			// we did, TempBuffer should hold a file name
			return VirtualFileNameExists(WorkingDirectory, TempBuffer);
		}

		// find the specified directory
		if (TempBuffer[0] == '.')
		{
			// this is a relative directory
			if (TempBuffer[1] == '.')
			{
				// this is going up
				WorkingDirectory = (VIRTUAL_DIRECTORY*)WorkingDirectory->ParentDirectory;

				// is it even valid?
				if (WorkingDirectory == NULL)
				{
					WorkingDirectory = RootDir;
				}
			}
			else
			{
				// this is the current directory
				// do nothing
			}
		}
		else
		{
			// does the directory exist?
			VIRTUAL_DIRECTORY* Directory = VirtualDirectoryNameExists(WorkingDirectory, TempBuffer);

			if (Directory == NULL)
			{
				// so the directory doesn't exist
				return NULL;
			}

			// the directory does exist, iterate to it
			WorkingDirectory = Directory;
		}
	}
}

SHELL_RESULT CreateVirtualFile(char* ParentDirectory, VIRTUAL_FILE* NewFileToInitialize,

	char *FileName, SHELL_RESULT(*ReadFileData)(GENERIC_BUFFER *), 


	SHELL_RESULT(*WriteFileData)(char* [], UINT32 , GENERIC_BUFFER *), 


	SHELL_RESULT(*ExecuteFile)(char* [], UINT32 , GENERIC_BUFFER *)

	#if (USE_VIRTUAL_FILE_DESCRIPTION == 1)
		, char* FileDescription
	#endif // end of #if (USE_FILE_DESCRIPTION == 1)

	#if (USE_VIRTUAL_FILE_HELP == 1)
		, char* FileHelp
	#endif // end of #if (USE_FILE_HELP == 1)

)
{
	VIRTUAL_DIRECTORY* ParentDirectoryNode;
	BOOL Outcome;

	// check all incoming parameters
	if (ParentDirectory == NULL)
		return SHELL_INVALID_INPUT_PARAMETER;

	if (NewFileToInitialize == NULL)
		return SHELL_INVALID_INPUT_PARAMETER;

	if (FileName == NULL)
		return SHELL_INVALID_INPUT_PARAMETER;

	// now, is the name valid?
	if (VirtualFileNameIsValid(FileName) == FALSE)
		return SHELL_INVALID_DIRECTORY_NAME;

	// so we know the characters in the name are valid
	// does the name already exist?
	ParentDirectoryNode = FollowVirtualDirectory(ParentDirectory, &gRootVirtualDirectory, gCurrentWorkingVirtualDirectory, &Outcome);

	// was it valid?
	if (Outcome == FALSE)
		return SHELL_INVALID_DIRECTORY_NAME;

	// does it already exist in the parent directory as a directory or file?
	if (VirtualDirectoryNameExists(ParentDirectoryNode, FileName) != NULL)
		return SHELL_VIRTUAL_DIRECTORY_NAME_ALREADY_EXISTS;

	// does the file already exist?
	if (VirtualFileNameExists(ParentDirectoryNode, FileName) != NULL)
		return SHELL_VIRTUAL_FILENAME_ALREADY_EXISTS;

	// set the name
	NewFileToInitialize->FileName = FileName;

	// it is a valid directory, add it
	if (LinkedListAddLast(&ParentDirectoryNode->Files, NewFileToInitialize) == FALSE)
		return SHELL_LINKED_LIST_OPERATION_FAILURE;

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