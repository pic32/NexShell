#include <string.h>

#include "clear_Command.h"

SHELL_RESULT clearCommandExecuteMethod(char* Args[], UINT32 NumberOfArgs, GENERIC_BUFFER* OutputStream)
{
	// output help if they asked
	if (NumberOfArgs != 0)
	{
		if (strcmp(Args[0], "--help") == 0)
		{
			if (GenericBufferWrite(OutputStream, strlen(CLEAR_HELP_TEXT), CLEAR_HELP_TEXT) != strlen(CLEAR_HELP_TEXT))
				return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

			return SHELL_SUCCESS;
		}
		else
		{
			return SHELL_INVALID_INPUT_PARAMETER;
		}
	}
	
	// this clears the screen
	if (GenericBufferWrite(OutputStream, (UINT32)strlen(SHELL_CLEAR_SCREEN_COMMAND), SHELL_CLEAR_SCREEN_COMMAND) != (UINT32)strlen(SHELL_CLEAR_SCREEN_COMMAND))
		return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

	// this puts the cursor in the upper left hand corner of the screen
	if (GenericBufferWrite(OutputStream, (UINT32)strlen(SHELL_HOME_CURSOR_COMMAND), SHELL_HOME_CURSOR_COMMAND) != (UINT32)strlen(SHELL_HOME_CURSOR_COMMAND))
		return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

	return SHELL_SUCCESS;
}
