#include <string.h>

#include "NexShellConfig.h"

#include "cd_Command.h"

extern char gCurrentWorkingDirectory[];

#if (EXTENDED_CD_SUPPORT == 1)
	char gPriorDirectory[SHELL_MAX_DIRECTORY_SIZE_IN_BYTES + 1];

	SHELL_RESULT cdInit(void)
	{
		memset(gPriorDirectory, 0, sizeof(gPriorDirectory));

		gPriorDirectory[0] = gCurrentWorkingDirectory[0];
		gPriorDirectory[1] = gCurrentWorkingDirectory[1];
		gPriorDirectory[2] = gCurrentWorkingDirectory[2];

		return SHELL_SUCCESS;
	}
#endif // end of #if (EXTENDED_CD_SUPPORT == 1)

SHELL_RESULT cdCommandExecuteMethod(char* Args[], UINT32 NumberOfArgs, PIPE* OutputStream)
{
	SHELL_RESULT Result;
	char CurrentWorkingDirectory[SHELL_MAX_DIRECTORY_SIZE_IN_BYTES + 1];
	char* WorkingDirectory = NULL;

	if (NumberOfArgs == 0)
		return SHELL_INSUFFICIENT_ARGUMENTS_FOR_FILE;

	// output help if they asked
	if (strcmp(Args[0], "--help") == 0)
	{
		if (PipeWrite(OutputStream, CD_HELP_TEXT, strlen(CD_HELP_TEXT), NULL) != OS_SUCCESS)
			return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

		return SHELL_SUCCESS;
	}

	#if (EXTENDED_CD_SUPPORT == 1)
		if (strlen(Args[0]) == 1)
		{
			if (*Args[0] == '-')
			{
				WorkingDirectory = gPriorDirectory;
			}
			else
			{
				// is it the home directory?
				if (*Args[0] == SHELL_HOME_DIRECTORY_CHARACTER)
				{
					WorkingDirectory = SHELL_HOME_DIRECTORY;

					strcpy(CurrentWorkingDirectory, WorkingDirectory);

					WorkingDirectory = CurrentWorkingDirectory;
				}
				else
				{
					WorkingDirectory = Args[0];
				}
			}
		}
		else
		{
			WorkingDirectory = Args[0];
		}
	#else
		WorkingDirectory = Args[0];

		strcat(CurrentWorkingDirectory, "/");
		strcat(CurrentWorkingDirectory, WorkingDirectory);

		WorkingDirectory = CurrentWorkingDirectory;
	#endif // end of #if (EXTENDED_CD_SUPPORT == 1)

	// change it to the one the user specified
	Result = f_chdir(WorkingDirectory);

	if (Result != SHELL_SUCCESS)
		return Result;

	Result = f_getcwd(CurrentWorkingDirectory, sizeof(CurrentWorkingDirectory));

	if (Result != SHELL_SUCCESS)
		return Result;

	#if (EXTENDED_CD_SUPPORT == 1)
		strcpy(gPriorDirectory, gCurrentWorkingDirectory);
	#endif // end of #if (EXTENDED_CD_SUPPORT == 1)

	// Now copy it over to the global one
	strcpy(gCurrentWorkingDirectory, CurrentWorkingDirectory);

	return SHELL_SUCCESS;
}
