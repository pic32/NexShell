#include <string.h>

#include "shutdown_Command.h"

void ShellPowerDownCallback(void);

SHELL_RESULT shutdownCommandExecuteMethod(char* Args[], UINT32 NumberOfArgs, GENERIC_BUFFER* OutputStream)
{
	char *Options;

	if (NumberOfArgs == 0)
		ShellPowerOff();

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

			ShellPowerDownSleep();

			// if we wake back up, return this value
			return SHELL_SUCCESS;
		}

		case 'P':
		{
			ShellPowerDownCallback();

			ShellPowerOff();
		}

		case 'r':
		{
			ShellPowerDownCallback();

			ShellPowerReset();
		}

		default:
		{
			return SHELL_INVALID_INPUT_PARAMETER;
		}
	}
}
