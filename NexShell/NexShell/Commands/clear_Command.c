#include <string.h>

#include "clear_Command.h"

SHELL_RESULT clearCommandExecuteMethod(char* Args[], UINT32 NumberOfArgs, PIPE* OutputStream)
{
	// output help if they asked
	if (NumberOfArgs != 0)
	{
		if (strcmp(Args[0], "--help") == 0)
		{
			if (PipeWrite(OutputStream, CLEAR_HELP_TEXT, (UINT32)strlen(CLEAR_HELP_TEXT), NULL) != OS_SUCCESS)
				return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

			return SHELL_SUCCESS;
		}
		else
		{
			return SHELL_INVALID_INPUT_PARAMETER;
		}
	}
	
	// this clears the screen
	if (PipeWrite(OutputStream, SHELL_CLEAR_SCREEN_COMMAND, (UINT32)strlen(SHELL_CLEAR_SCREEN_COMMAND), NULL) != OS_SUCCESS)
		return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

	// this puts the cursor in the upper left hand corner of the screen
	if (PipeWrite(OutputStream, SHELL_HOME_CURSOR_COMMAND, (UINT32)strlen(SHELL_HOME_CURSOR_COMMAND), NULL) != OS_SUCCESS)
		return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

	return SHELL_SUCCESS;
}
