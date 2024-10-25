#include <string.h>

#include "pwd_Command.h"

SHELL_RESULT pwdCommandExecuteMethod(char* Args[], UINT32 NumberOfArgs, GENERIC_BUFFER* OutputStream)
{
	SHELL_RESULT Result;
	char CurrentWorkingDirectory[SHELL_MAX_DIRECTORY_SIZE_IN_BYTES + 1];

	if (NumberOfArgs != 0)
		return SHELL_INVALID_ARGUMENT;

	Result = f_getcwd(CurrentWorkingDirectory, sizeof(CurrentWorkingDirectory));

	if (Result != SHELL_SUCCESS)
		return Result;

	Result = NexShellProcessOutgoingData(CurrentWorkingDirectory, OutputStream, strlen(CurrentWorkingDirectory), SHELL_HAL_MAX_TRANSFER_SIZE_IN_BYTES, NexShellWriteTasks);

	if (Result != SHELL_SUCCESS)
		return Result;

	if (GenericBufferWrite(OutputStream, SHELL_END_OF_LINE_SEQUENCE_SIZE_IN_BYTES, SHELL_DEFAULT_END_OF_LINE_SEQUENCE) != SHELL_END_OF_LINE_SEQUENCE_SIZE_IN_BYTES)
		return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

	return SHELL_SUCCESS;
}