#ifndef VIRTUAL_FILE_H
	#define VIRTUAL_FILE_H

#include "NexShell.h"
#include "VirtualObjects.h"

VIRTUAL_FILE* VirtualFileNameExists(VIRTUAL_DIRECTORY* Directory, char* FileNameToFind);

VIRTUAL_FILE* VirtualShellGetWorkingFile(char* Buffer, VIRTUAL_DIRECTORY* RootDir, VIRTUAL_DIRECTORY* CurrentDir);

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
