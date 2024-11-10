#include <string.h>

#include "pwd_Command.h"

SHELL_RESULT pwdCommandExecuteMethod(char* Args[], UINT32 NumberOfArgs, PIPE* OutputStream)
{
	SHELL_RESULT Result;
	char CurrentWorkingDirectory[SHELL_MAX_DIRECTORY_SIZE_IN_BYTES + 1];

	// output help if they asked
	if (NumberOfArgs != 0)
	{
		if (strcmp(Args[0], "--help") == 0)
		{
			if (PipeWrite(OutputStream, PWD_HELP_TEXT, strlen(PWD_HELP_TEXT), NULL) != OS_SUCCESS)
				return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

			return SHELL_SUCCESS;
		}
		else
		{
			return SHELL_INVALID_ARGUMENT;
		}
	}

	Result = f_getcwd(CurrentWorkingDirectory, sizeof(CurrentWorkingDirectory));

	if (Result != SHELL_SUCCESS)
		return Result;

	if (PipeWrite(OutputStream, CurrentWorkingDirectory, strlen(CurrentWorkingDirectory), NULL) != OS_SUCCESS)
		return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

	if (PipeWrite(OutputStream, SHELL_DEFAULT_END_OF_LINE_SEQUENCE, SHELL_END_OF_LINE_SEQUENCE_SIZE_IN_BYTES, NULL) != OS_SUCCESS)
		return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

	return SHELL_SUCCESS;
}