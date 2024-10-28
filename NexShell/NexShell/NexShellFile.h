#ifndef NEXSHELL_FILE_H
	#define NEXSHELL_FILE_H

#include "NexShell.h"

typedef enum
{
	SHELL_FILE_DISK = 0,
	SHELL_FILE_VIRTUAL,
	NUMBER_OF_SHELL_FILES
}SHELL_FILE_TYPE;

typedef struct
{
	SHELL_RESULT(*ReadFileData)(GENERIC_BUFFER* OutputStream);
	SHELL_RESULT(*WriteFileData)(char* Args[], UINT32 NumberOfArgs, GENERIC_BUFFER* OutputStream);
	SHELL_RESULT(*ExecuteFile)(char* Args[], UINT32 NumberOfArgs, GENERIC_BUFFER* OutputStream);

	char *FileName;
	char *FileLocation;
	char *FileData;

	#if (USE_FILE_DESCRIPTION == 1)
		char *FileDescription;
	#endif // end of #if (USE_FILE_DESCRIPTION == 1)

	#if (USE_FILE_HELP == 1)
		char *FileHelp;
	#endif // end of #if (USE_FILE_HELP == 1)
}VIRTUAL_FILE;

#if (USING_USER_VIRTUAL_FILES == 1)
	typedef struct
	{
		VIRTUAL_FILE *File;

		char *FilePath;
		char *Filename;
	}VIRTUAL_FILE_LIST_NODE;
#endif // end of #if (USING_USER_VIRTUAL_FILES == 1)

typedef struct
{
	SHELL_FILE_TYPE FileType;
	void *File; // this points to a FIL or a VIRTUAL_FILE
}SHELL_FILE;

SHELL_RESULT NexShellFileOpen(char *FilePath, SHELL_FILE *File, BYTE OperatingMode);

SHELL_RESULT NexShellFileClose(SHELL_FILE *File);

SHELL_RESULT NexShellFileWrite(SHELL_FILE *File, char *Buffer, GENERIC_BUFFER *Stream, UINT32 BytesToWrite);

SHELL_RESULT NexShellFileRead(SHELL_FILE *File, char *Buffer, GENERIC_BUFFER *Stream, UINT32 BytesToRead);

SHELL_RESULT NexShellFileDelete(char *FilePath);

#endif // end of NEXSHELL_FILE_H