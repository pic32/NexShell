#ifndef VIRTUAL_FILE_H
	#define VIRTUAL_FILE_H

#include "NexShell.h"
#include "LinkedList.h"

typedef struct
{
	SHELL_RESULT(*ReadFileData)(PIPE* OutputBuffer);

	SHELL_RESULT(*WriteFileData)(char* Args[], UINT32 NumberOfArgs, PIPE* OutputBuffer);

	SHELL_RESULT(*ExecuteFile)(char* Args[], UINT32 NumberOfArgs, PIPE* OutputBuffer);

	char* FileName;
	char* FileDescription;
	char* FileHelp;
}VIRTUAL_FILE;

typedef struct
{
	LINKED_LIST VirtualFileList;
	char* DirectoryName;
}VIRTUAL_FILE_DIRECTORY;

SHELL_RESULT VirtualFileInit(void);

LINKED_LIST* GetVirtualFileList(char* Directory);

SHELL_RESULT VirtualFileAddToVirtualFileSystem(VIRTUAL_FILE* VirtualFile, char* DirectoryPath);

VIRTUAL_FILE* VirtualFileNameExists(LINKED_LIST* FileList, char* FileNameToFind);

VIRTUAL_FILE* GetVirtualFile(char* Directory, char* Filename);

SHELL_RESULT CreateVirtualFile(VIRTUAL_FILE* NewFileToInitialize, const char *FileName, SHELL_RESULT(*ReadFileData)(PIPE *), SHELL_RESULT(*WriteFileData)(char*[], UINT32, PIPE *), SHELL_RESULT(*ExecuteFile)(char*[], UINT32, PIPE *), const char* FileDescription, const char* FileHelp);

#endif // end of #ifndef VIRTUAL_FILE_H
