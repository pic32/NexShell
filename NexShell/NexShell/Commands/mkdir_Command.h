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
SHELL_TAB SHELL_TAB"-m        Set file mode, bit values can logically or'ed." SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB SHELL_TAB "        Valid bit values and their meanings are as follows:" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB SHELL_TAB"          1 = Read Only" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB SHELL_TAB"          2 = Hidden" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB SHELL_TAB"          4 = System" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB SHELL_TAB"          32 = Archive" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB"-p        no error if existing, make parent directories as needed." SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB"          Their file modes would be unaffected by any - m option." SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB "Examples:" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB MKDIR_COMMAND_NAME " -p ./a-folder/another-folder" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB MKDIR_COMMAND_NAME " -m5 directory-to-add" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
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