#ifndef NEXSHELL_FILE_H
	#define NEXSHELL_FILE_H

#include "NexShell.h"
#include "ff.h"

typedef enum
{
	SHELL_FILE_DISK = 0,
	SHELL_FILE_VIRTUAL,
	NUMBER_OF_SHELL_FILES
}SHELL_FILE_TYPE;

typedef enum
{
	FILE_MODE_OPEN_EXISTING = 0x00,
	FILE_MODE_READ			= 0x01,
	FILE_MODE_WRITE			= 0x02,
	FILE_MODE_CREATE_NEW	= 0x04,
	FILE_MODE_CREATE_ALWAYS	= 0x08,
	FILE_MODE_OPEN_ALWAYS	= 0x10,
	FILE_MODE_OPEN_APPEND	= 0x30,
	FILE_MODE_VIRTUAL       = 0x40
}FILE_MODE;

typedef struct
{
	SHELL_RESULT(*ReadFileData)(PIPE* OutputStream);
	SHELL_RESULT(*WriteFileData)(char* Args[], UINT32 NumberOfArgs, PIPE* OutputStream);
	SHELL_RESULT(*ExecuteFile)(char* Args[], UINT32 NumberOfArgs, PIPE* OutputStream);

	char *FileName;

	#if (USE_FILE_DESCRIPTION == 1)
		char *FileDescription;
	#endif // end of #if (USE_FILE_DESCRIPTION == 1)

	#if (USE_FILE_HELP == 1)
		char *FileHelp;
	#endif // end of #if (USE_FILE_HELP == 1)
}VIRTUAL_FILE;

typedef struct
{
	SHELL_FILE_TYPE FileType;
	void *File; // this points to a FIL or a VIRTUAL_FILE
}SHELL_FILE;

BOOL FileExists(char* FullFilePath);

SHELL_RESULT NexShellFileOpen(char *FilePath, SHELL_FILE *File, BYTE OperatingMode);

SHELL_RESULT NexShellFileClose(SHELL_FILE *File);

SHELL_RESULT NexShellFileWrite(SHELL_FILE *File, char *Buffer, PIPE *Stream, UINT32 BytesToWrite);

SHELL_RESULT NexShellFileRead(SHELL_FILE *File, char *Buffer, PIPE *Stream, UINT32 BytesToRead);

SHELL_RESULT NexShellFileDelete(char *FilePath);

#endif // end of NEXSHELL_FILE_H