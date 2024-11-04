#ifndef VIRTUAL_FILE_H
	#define VIRTUAL_FILE_H

#include "NexShell.h"
#include "LinkedList.h"

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

typedef struct
{
	LINKED_LIST VirtualFileList;
	char* DirectoryName;
}VIRTUAL_FILE_DIRECTORY;

LINKED_LIST* GetVirtualFileList(char* Directory);

SHELL_RESULT VirtualFileAddToVirtualFileSystem(VIRTUAL_FILE* VirtualFile, char* DirectoryPath);

SHELL_RESULT VirtualFileInit(void);

VIRTUAL_FILE* VirtualFileNameExists(LINKED_LIST* FileList, char* FileNameToFind);

SHELL_RESULT CreateVirtualFile(VIRTUAL_FILE* NewFileToInitialize,

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
