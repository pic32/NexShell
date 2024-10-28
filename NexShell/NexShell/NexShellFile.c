#include "NexShellFile.h"

SHELL_RESULT NexShellFileOpen(char* FilePath, SHELL_FILE* File, BYTE OperatingMode)
{
	// we don't know if this is virtual or on disk
	FIL DiskFile;
	SHELL_RESULT Result;

	// try opening from disk first
	Result = f_open(&DiskFile, FilePath, OperatingMode);

	// did the file opem?
	if (Result == SHELL_SUCCESS)
	{
		// the file did open, it is a disk file

	}
	else
	{
		// the file did not open on disk, try to find it as a virtual file
		
	}
}

SHELL_RESULT NexShellFileClose(SHELL_FILE* File)
{

}

SHELL_RESULT NexShellFileWrite(SHELL_FILE* File, char* Buffer, GENERIC_BUFFER* Stream, UINT32 BytesToWrite)
{

}

SHELL_RESULT NexShellFileRead(SHELL_FILE* File, char* Buffer, GENERIC_BUFFER* Stream, UINT32 BytesToRead)
{

}

SHELL_RESULT NexShellFileDelete(char* FilePath)
{

}
