#include <string.h>

#include "help_Command.h"
#include "NexShellConfig.h"
#include "NexShellCommands.h"
#include "LinkedList.h"
#include "VirtualFile.h"

static OutputCommandHelp(COMMAND_INFO* CommandInfo, PIPE* OutputStream)
{
	char SpaceBuffer[HELP_COMMAND_NAME_SIZE_IN_BYTES];

	// clear out the buffer with all spaces
	memset(SpaceBuffer, ' ', HELP_COMMAND_NAME_SIZE_IN_BYTES - 2);

	// put a - for the description
	SpaceBuffer[HELP_COMMAND_NAME_SIZE_IN_BYTES - 2] = '-';

	// now null terminate
	SpaceBuffer[HELP_COMMAND_NAME_SIZE_IN_BYTES - 1] = 0;

	// now copy over the command name into the buffer
	memcpy(SpaceBuffer, CommandInfo->CommandName, strlen(CommandInfo->CommandName));

	// write the command name with spaces to the buffer
	if (PipeWrite(OutputStream, SpaceBuffer, (UINT32)strlen(SpaceBuffer), NULL) != OS_SUCCESS)
		return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

	// now output the description
	if (PipeWrite(OutputStream, CommandInfo->Description, (UINT32)strlen(CommandInfo->Description), NULL) != OS_SUCCESS)
		return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

	if (PipeWrite(OutputStream, SHELL_DEFAULT_END_OF_LINE_SEQUENCE, SHELL_END_OF_LINE_SEQUENCE_SIZE_IN_BYTES, NULL) != OS_SUCCESS)
		return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

	return SHELL_SUCCESS;
}

static UINT help_ForwardData(   /* Returns number of bytes sent or stream status */
	const BYTE* DataToWrite,  /* Pointer to the data block to be sent */
	UINT btf,        /* >0: Transfer call (Number of bytes to be sent). 0: Sense call */
	void* OutputStream,
	void* Options
)
{
	// this is to find out if ready for transfer
	if (btf == 0)
	{
		return 1;
	}

	// write the description
	if (PipeWrite((PIPE*)OutputStream, (BYTE*)DataToWrite, (UINT32)btf, NULL) != OS_SUCCESS)
		return 0;

	return btf;
}

static SHELL_RESULT OutputHelpFile(char *Name, char *Description, char *Help, BOOL DescriptionOnly, PIPE* OutputStream)
{
	SHELL_RESULT Result;

	if (Name == NULL)
		return SHELL_SUCCESS;

	// write the command name
	if (PipeWrite(OutputStream, Name,(UINT32)strlen(Name), NULL) != OS_SUCCESS)
		return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

	if (PipeWrite(OutputStream, ": ", (UINT32)strlen(": "), NULL) != OS_SUCCESS)
		return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

	// write the description
	if(Description != NULL)
		if (PipeWrite(OutputStream, Description, (UINT32)strlen(Description), NULL) != OS_SUCCESS)
			return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

	// write out a new line
	if (PipeWrite(OutputStream, SHELL_DEFAULT_END_OF_LINE_SEQUENCE, SHELL_END_OF_LINE_SEQUENCE_SIZE_IN_BYTES, NULL) != OS_SUCCESS)
		return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

	if (DescriptionOnly == FALSE && Help != NULL)
	{
		FIL File;

		Result = f_open(&File, Help, FA_OPEN_EXISTING | FA_READ);

		if (Result == SHELL_SUCCESS)
		{
			// now we must read the data out and send it
			UINT DataRead;

			// we did, now start reading the contents
			while (Result == SHELL_SUCCESS && !f_eof(&File))
			{
				// forward more data
				Result = f_forward(&File, help_ForwardData, PipeGetRemainingBytes(OutputStream), &DataRead, (void*)OutputStream, NULL);

				// did we read ok?
				if (Result != SHELL_SUCCESS)
				{
					f_close(&File);

					return Result;
				}
			}

			Result = f_close(&File);

			// did it close?
			if (Result != SHELL_SUCCESS)
				return Result;
		}
		else
		{
			// write the help file
			if(Help != NULL)
				if (PipeWrite(OutputStream, Help, (UINT32)strlen(Help), NULL) != OS_SUCCESS)
					return SHELL_GENERIC_BUFFER_WRITE_FAILURE;
		}
	}

	return SHELL_SUCCESS;
}

SHELL_RESULT helpCommandExecuteMethod(char* Args[], UINT32 NumberOfArgs, PIPE* OutputStream)
{
	// output help if they asked
	if (NumberOfArgs != 0)
	{
		if (strcmp(Args[0], "--help") == 0)
		{
			if (PipeWrite(OutputStream, HELP_HELP_TEXT, (UINT32)strlen(HELP_HELP_TEXT), NULL) != OS_SUCCESS)
				return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

			return SHELL_SUCCESS;
		}
	}

	// if there are zero arguments just print out all the global 
	// files that are available
	switch (NumberOfArgs)
	{
		case 0:
		{
			// if zero arguments are had, output all commands

			SHELL_RESULT Result;
			UINT32 i = 0;

			do
			{
				COMMAND_INFO* CommandInfo = GetNativeCommandInfo(i);

				Result = OutputCommandHelp(CommandInfo, OutputStream);

				if (Result != SHELL_SUCCESS)
					return Result;
			} 
			while (GetNativeCommandInfo(++i) != NULL);


			#if (USE_USER_COMMANDS == 1)
				i = 1;

				// now the user commands
				do
				{
					COMMAND_INFO* CommandInfo = GetUserCommandByIndex(i++);

					if (CommandInfo == NULL)
						break;

					Result = OutputCommandHelp(CommandInfo, OutputStream);

					if (Result != SHELL_SUCCESS)
						return Result;
				} 
				while (1);
			#endif // end of #if (USE_USER_COMMANDS == 1)

			return SHELL_SUCCESS;
		}

		default:
		{
			SHELL_RESULT Result;
			UINT32 i, j;
			COMMAND_INFO* CommandInfo;
			BOOL DescriptionOnly = FALSE;
			UINT32 ArgsProcessed = 0;

			// get the options first
			if (strcmp(Args[0], "-d") == 0)
			{
				DescriptionOnly = TRUE;

				ArgsProcessed++;

				i = 1;
			}
			else
			{
				i = 0;
			}

			for (; i < NumberOfArgs; i++)
			{
				CommandInfo = NULL;

				// first search for a virtual file help
				{
					VIRTUAL_FILE* VirtualFile;

					// now is the current directory request virtual?
					VirtualFile = GetVirtualFile(Args[i]);

					if (VirtualFile != NULL)
					{
						// we got a match
						Result = OutputHelpFile(VirtualFile->FileName, VirtualFile->FileDescription, VirtualFile->FileHelp, DescriptionOnly, OutputStream);

						if (Result != SHELL_SUCCESS)
							return Result;

						continue;
					}
				}

				// search for a native command match
				for(j = 0; j < GetNumberOfNextShellNativeCommands(); j++)
				{
					CommandInfo = GetNativeCommandInfo(j);

					// look for a match
					if (strcmp(CommandInfo->CommandName, Args[i]) == 0)
					{
						break;
					}
					else
					{
						CommandInfo = NULL;
					}
				}

				#if (USE_USER_COMMANDS == 1)
					// look for a user command since we didn't find one in the native list
					if (CommandInfo == NULL)
					{
						CommandInfo = GetUserCommandByName(Args[i]);
					}
				#endif // end of #if (USE_USER_COMMANDS == 1)

				if (CommandInfo == NULL)
					return SHELL_COMMAND_NOT_FOUND;

				Result = OutputHelpFile(CommandInfo->CommandName, CommandInfo->Description, CommandInfo->Help, DescriptionOnly, OutputStream);

				if (Result != SHELL_SUCCESS)
					return Result;
			}

			return SHELL_SUCCESS;
		}
	}
}
