#include "NexShellFile.h"
#include "LinkedList.h"

extern char gCurrentWorkingDirectory[];

#if (USING_USER_VIRTUAL_FILES == 1)
	LINKED_LIST gUserVirtualFiles;

	static SHELL_RESULT IsFileVirtual(const char* FullFilePath, UINT32* Index)
	{
		UINT32 i, Size;
		char* VirtualFileFullPath;

		// make the index invalid to begin with
		*Index = 0;

		Size = LinkedListGetSize(&gUserVirtualFiles);

		for (i = 0; i < Size; i++)
		{
			// get the next item in the list
			VirtualFileFullPath = (char*)LinkedListGetData(&gUserVirtualFiles, i + 1);

			// is it valid?
			if (VirtualFileFullPath == NULL)
				return SHELL_LINKED_LIST_OPERATION_FAILURE;

			// now compare
			if (strcmp(FullFilePath, VirtualFileFullPath) == 0)
			{
				// it was valid
				*Index = i + 1;

				return SHELL_SUCCESS;
			}
		}

		return SHELL_SUCCESS;
	}

	SHELL_RESULT NexShellFileModuleInit(void)
	{
		if (CreateLinkedList(&gUserVirtualFiles, NULL, NULL) == FALSE)
			return SHELL_LINKED_LIST_CREATE_FAILURE;

		return SHELL_SUCCESS;
	}
#endif // end of #if (USING_USER_VIRTUAL_FILES == 1)

SHELL_RESULT NexShellFileOpen(char* FilePath, SHELL_FILE* File, FILE_MODE OperatingMode)
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
