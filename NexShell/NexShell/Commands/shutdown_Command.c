#include <string.h>

#include "shutdown_Command.h"
#include "ioctl.h"

void ShellPowerDownCallback(void);

SHELL_RESULT shutdownCommandExecuteMethod(char* Args[], UINT32 NumberOfArgs, PIPE* OutputStream)
{
	char *Options;

	if (NumberOfArgs == 0)
	{
		ShellPowerDownCallback();

		ShellPowerOff();
	}
	else
	{
		// output help if they asked
		if (strcmp(Args[0], "--help") == 0)
		{
			if (PipeWrite(OutputStream, SHUTDOWN_HELP_TEXT, (UINT32)strlen(SHUTDOWN_HELP_TEXT), NULL) != OS_SUCCESS)
				return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

			return SHELL_SUCCESS;
		}
	}

	if (NumberOfArgs != 1)
		return SHELL_INVALID_INPUT_PARAMETER;

	Options = Args[0];

	// are we the correct length?
	if(strlen(Options) != 2)
		return SHELL_INVALID_INPUT_PARAMETER;

	if(*Options++ != '-')
		return SHELL_INVALID_INPUT_PARAMETER;

	switch (*Options)
	{
		case 'H':
		{
			ShellPowerDownCallback();

			// initiate the sleep
			if (ioctl(NULL, SYSTEM_SHUTDOWN_SLEEP, NULL) != 0)
				return SHELL_IO_CTL_FAILED;

			// if we wake back up, return this value
			return SHELL_SUCCESS;
		}

		case 'P':
		{
			ShellPowerDownCallback();

			if (ioctl(NULL, SYSTEM_SHUTDOWN_POWER_OFF, NULL) != 0)
				return SHELL_IO_CTL_FAILED;

			// if we wake back up, return this value
			return SHELL_SUCCESS;
		}

		case 'r':
		{
			ShellPowerDownCallback();

			if (ioctl(NULL, SYSTEM_SHUTDOWN_RESET, NULL) != 0)
				return SHELL_IO_CTL_FAILED;

			// if we wake back up, return this value
			return SHELL_SUCCESS;
		}

		default:
		{
			return SHELL_INVALID_INPUT_PARAMETER;
		}
	}
}
