#include "NexShellFile.h"
#include "LinkedList.h"

extern char gCurrentWorkingDirectory[];

BOOL FileExists(char* FullFilePath)
{
	FIL File;

	if (f_open(&File, FullFilePath, FA_OPEN_EXISTING) != FR_OK)
		return FALSE;

	return (BOOL)(f_close(&File) == FR_OK);
}

SHELL_RESULT NexShellFileOpen(char* FilePath, SHELL_FILE* File, FILE_MODE OperatingMode)
{

}

SHELL_RESULT NexShellFileClose(SHELL_FILE* File)
{

}

SHELL_RESULT NexShellFileWrite(SHELL_FILE* File, char* Buffer, PIPE* Stream, UINT32 BytesToWrite)
{

}

SHELL_RESULT NexShellFileRead(SHELL_FILE* File, char* Buffer, PIPE* Stream, UINT32 BytesToRead)
{

}

SHELL_RESULT NexShellFileDelete(char* FilePath)
{

}
