#ifndef VIRTUAL_OBJECTS_H
	#define VIRTUAL_OBJECTS_H

#include "NexShell.h"
#include "LinkedList.h"

typedef struct
{
	LINKED_LIST Directories; // this is of type DIRECTORY
	LINKED_LIST Files; // this is of type VIRTUAL_FILE
	void *ParentDirectory; // this is of type DIRECTORY
	char *DirectoryName;
}VIRTUAL_DIRECTORY;


typedef struct
{

	SHELL_RESULT(*ReadFileData)(GENERIC_BUFFER* OutputBuffer);

	SHELL_RESULT(*WriteFileData)(char* Args[], UINT32 NumberOfArgs, GENERIC_BUFFER *OutputBuffer);
	
	SHELL_RESULT(*ExecuteFile)(char* Args[], UINT32 NumberOfArgs, GENERIC_BUFFER *OutputBuffer);

	char* FileName;

	#if (USE_VIRTUAL_FILE_DESCRIPTION == 1)
		char* FileDescription;
	#endif // end of #if (USE_FILE_DESCRIPTION == 1)

	#if (USE_VIRTUAL_FILE_HELP == 1)
		char* FileHelp;
	#endif // end of #if (USE_FILE_HELP == 1)
}VIRTUAL_FILE;



#endif // end of #ifndef VIRTUAL_OBJECTS_H
