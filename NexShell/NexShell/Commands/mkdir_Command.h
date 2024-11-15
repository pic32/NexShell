#ifndef MKDIR_COMMAND_H
	#define MKDIR_COMMAND_H

#include "NexShell.h"

#define MKDIR_HELP_TEXT \
\
"Usage: " MKDIR_COMMAND_NAME " [OPTION]... DIRECTORY..." SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB "Create the DIRECTORY(ies), if they do not already exist." SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB "Options:" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB"-m        set file mode (as in chmod), not a=rwx - umask" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB"-p        no error if existing, make parent directories as needed, with their file modes unaffected by any - m option." SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB "Examples:" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB MKDIR_COMMAND_NAME " -p ./a-folder/another-folder" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB MKDIR_COMMAND_NAME " directory-to-add" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_DEFAULT_END_OF_LINE_SEQUENCE

typedef union
{
	BYTE Value;
	struct
	{
		unsigned int AllowParentDirectoryCreation : 1;
		unsigned int Mode : 1;
		unsigned int Reserved : 6;
	}Bits;
}MKDIR_OPTIONS;

typedef struct
{
	MKDIR_OPTIONS Options;
	BYTE FolderCreateMode;
}MKDIR_OPTIONS_DATA;

SHELL_RESULT mkdirCommandExecuteMethod(char* Args[], UINT32 NumberOfArgs, PIPE* OutputStream);

#endif // end of MKDIR_COMMAND_H