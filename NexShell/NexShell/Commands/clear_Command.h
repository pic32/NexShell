#ifndef CLEAR_COMMAND_H
	#define CLEAR_COMMAND_H

#include "NexShell.h"

#define CLEAR_HELP_TEXT \
\
CLEAR_COMMAND_NAME ": " CLEAR_COMMAND_NAME SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB "Clear the terminal screen." SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB "This command will clear the terminal screen." SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB "This will also reset the cursor position to 0, 0." SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB "Example:" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB CLEAR_COMMAND_NAME SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_DEFAULT_END_OF_LINE_SEQUENCE

SHELL_RESULT clearCommandExecuteMethod(char* Args[], UINT32 NumberOfArgs, GENERIC_BUFFER* Buffer);

#endif // end of CLEAR_COMMAND_H