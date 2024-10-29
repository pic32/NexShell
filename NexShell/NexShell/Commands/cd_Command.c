#include <string.h>

#include "NexShellConfig.h"

#include "cd_Command.h"
#include "VirtualDirectory.h"

extern char gCurrentWorkingDirectory[];
extern VIRTUAL_DIRECTORY* gCurrentWorkingVirtualDirectory;
extern VIRTUAL_DIRECTORY gRootVirtualDirectory;

#if (EXTENDED_CD_SUPPORT == 1)
	char gPriorDirectory[SHELL_MAX_DIRECTORY_SIZE_IN_BYTES + 1];
	VIRTUAL_DIRECTORY* gPriorVirtualDirectory = NULL;

	SHELL_RESULT cdInit(void)
	{
		memset(gPriorDirectory, 0, sizeof(gPriorDirectory));

		gPriorDirectory[0] = gCurrentWorkingDirectory[0];
		gPriorDirectory[1] = gCurrentWorkingDirectory[1];
		gPriorDirectory[2] = gCurrentWorkingDirectory[2];

		return SHELL_SUCCESS;
	}
#endif // end of #if (EXTENDED_CD_SUPPORT == 1)

SHELL_RESULT VirtualcdExecuteMethod(char *Args[], UINT32 NumberOfArgs, GENERIC_BUFFER* OutputStream)
{
	BOOL Outcome;
	VIRTUAL_DIRECTORY* Directory;
	SHELL_RESULT Result;

	Directory = FollowVirtualDirectory(Args, &gRootVirtualDirectory, gCurrentWorkingVirtualDirectory, &Outcome);

	if (Directory == NULL || Outcome == FALSE)
		return SHELL_INVALID_DIRECTORY_NAME;

	Result = VirtualDirectoryGetPath(Directory, Args);

	if (Result != SHELL_SUCCESS)
		return Result;

	#if (EXTENDED_CD_SUPPORT == 1)
		strcpy(gPriorDirectory, gCurrentWorkingDirectory);
	#endif // end of #if (EXTENDED_CD_SUPPORT == 1)

	strcpy(gCurrentWorkingDirectory, Args);
}

SHELL_RESULT cdCommandExecuteMethod(char* Args[], UINT32 NumberOfArgs, GENERIC_BUFFER* OutputStream)
{
	if (NumberOfArgs == 0)
		return SHELL_INSUFFICIENT_ARGUMENTS_FOR_FILE;

	// now we check if it is virtual, if it is, it is a different process
	{
		SHELL_RESULT Result;
		char CurrentWorkingDirectory[SHELL_MAX_DIRECTORY_SIZE_IN_BYTES + 1];
		char* WorkingDirectory = NULL;

		Result = f_getcwd(CurrentWorkingDirectory, sizeof(CurrentWorkingDirectory));

		if (Result != SHELL_SUCCESS)
			return Result;

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
				}
			}

			//char TempPath = CurrentWorkingDirectory;
		
			if(WorkingDirectory == NULL)
			{
				WorkingDirectory = Args[0];

				// is it relative?
				if (WorkingDirectory[0] == '.')
				{
					// it is relative
					strcat(CurrentWorkingDirectory, "/");
					strcat(CurrentWorkingDirectory, WorkingDirectory);
				}
				else
				{
					// it is absolute
					strcpy(CurrentWorkingDirectory, WorkingDirectory);
				}

				WorkingDirectory = CurrentWorkingDirectory;
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

		if (IsDirectoryVirtual(CurrentWorkingDirectory) == TRUE)
		{
			Args[0] = CurrentWorkingDirectory;

			return VirtualcdExecuteMethod(CurrentWorkingDirectory, NumberOfArgs, OutputStream);
		}

		if (Result != SHELL_SUCCESS)
			return Result;

		#if (EXTENDED_CD_SUPPORT == 1)
			strcpy(gPriorDirectory, gCurrentWorkingDirectory);
		#endif // end of #if (EXTENDED_CD_SUPPORT == 1)

		// Now copy it over to the global one
		strcpy(gCurrentWorkingDirectory, CurrentWorkingDirectory);

		return SHELL_SUCCESS;
	}
}
