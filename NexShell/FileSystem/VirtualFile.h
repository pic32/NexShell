#ifndef VIRTUAL_FILE_H
	#define VIRTUAL_FILE_H

#include "NexShell.h"

typedef struct
{
	SHELL_RESULT(*ReadFileData)(GENERIC_BUFFER* OutputBuffer);

	SHELL_RESULT(*WriteFileData)(char* Args[], UINT32 NumberOfArgs, GENERIC_BUFFER* OutputBuffer);

	SHELL_RESULT(*ExecuteFile)(char* Args[], UINT32 NumberOfArgs, GENERIC_BUFFER* OutputBuffer);

	char* FileName;

	#if (USE_VIRTUAL_FILE_DESCRIPTION == 1)
		char* FileDescription;
	#endif // end of #if (USE_FILE_DESCRIPTION == 1)

	#if (USE_VIRTUAL_FILE_HELP == 1)
		char* FileHelp;
	#endif // end of #if (USE_FILE_HELP == 1)
}VIRTUAL_FILE;

VIRTUAL_FILE* VirtualFileNameExists(VIRTUAL_DIRECTORY* Directory, char* FileNameToFind);

SHELL_RESULT CreateVirtualFile(char* ParentDirectory, VIRTUAL_FILE* NewFileToInitialize,

	char *FileName, SHELL_RESULT(*ReadFileData)(GENERIC_BUFFER *),


	SHELL_RESULT(*WriteFileData)(char*[], UINT32, GENERIC_BUFFER *),


	SHELL_RESULT(*ExecuteFile)(char*[], UINT32, GENERIC_BUFFER *)

	#if (USE_VIRTUAL_FILE_DESCRIPTION == 1)
		, char* FileDescription
	#endif // end of #if (USE_FILE_DESCRIPTION == 1)

	#if (USE_VIRTUAL_FILE_HELP == 1)
		, char* FileHelp
	#endif // end of #if (USE_FILE_HELP == 1)

);

SHELL_RESULT VirtualShellProcessOutgoingData(char* Data, GENERIC_BUFFER* Buffer, UINT32 NumberOfBytesToProcess, UINT32 TransferSizeInBytes, SHELL_RESULT(*WriteTasks)(GENERIC_BUFFER* Buffer));

SHELL_RESULT VirtualShellWriteTasks(GENERIC_BUFFER* GenericBuffer);

#endif // end of #ifndef VIRTUAL_FILE_H
