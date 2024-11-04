#include <string.h>
#include <ctype.h>

#include "NexShell.h"
#include "VirtualFile.h"

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

SHELL_RESULT CreateVirtualFile(VIRTUAL_FILE* NewFileToInitialize, char *FileName, SHELL_RESULT(*ReadFileData)(GENERIC_BUFFER *), SHELL_RESULT(*WriteFileData)(char* [], UINT32 , GENERIC_BUFFER *), SHELL_RESULT(*ExecuteFile)(char* [], UINT32 , GENERIC_BUFFER *)

	#if (USE_VIRTUAL_FILE_DESCRIPTION == 1)
		, char* FileDescription
	#endif // end of #if (USE_FILE_DESCRIPTION == 1)

	#if (USE_VIRTUAL_FILE_HELP == 1)
		, char* FileHelp
	#endif // end of #if (USE_FILE_HELP == 1)

)
{
	BOOL Outcome;

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