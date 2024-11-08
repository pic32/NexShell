#include <string.h>

#include "help_Command.h"
#include "NexShellConfig.h"
#include "NexShellCommands.h"
#include "LinkedList.h"

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
	if (GenericBufferWrite(OutputStream, (UINT32)strlen(SpaceBuffer), SpaceBuffer) != (UINT32)strlen(SpaceBuffer))
		return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

	// now output the description
	if (GenericBufferWrite(OutputStream, (UINT32)strlen(CommandInfo->Description), CommandInfo->Description) != (UINT32)strlen(CommandInfo->Description))
		return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

	if (GenericBufferWrite(OutputStream, SHELL_END_OF_LINE_SEQUENCE_SIZE_IN_BYTES, SHELL_DEFAULT_END_OF_LINE_SEQUENCE) != SHELL_END_OF_LINE_SEQUENCE_SIZE_IN_BYTES)
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
	SHELL_RESULT Result;

	// this is to find out if ready for transfer
	if (btf == 0)
	{
		return 1;
	}

	Result = NexShellProcessOutgoingData(DataToWrite, (PIPE*)OutputStream, strlen(DataToWrite), SHELL_HAL_MAX_TRANSFER_SIZE_IN_BYTES, NexShellWriteTasks);

	if (Result != SHELL_SUCCESS)
		return 0;

	return btf;
}

static SHELL_RESULT OutputHelpFile(COMMAND_INFO* CommandInfo, BOOL DescriptionOnly, PIPE* OutputStream)
{
	SHELL_RESULT Result;

	if (GenericBufferWrite(OutputStream, (UINT32)strlen(CommandInfo->CommandName), CommandInfo->CommandName) != (UINT32)strlen(CommandInfo->CommandName))
		return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

	if (GenericBufferWrite(OutputStream, (UINT32)strlen(": "), ": ") != (UINT32)2)
		return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

	Result = NexShellProcessOutgoingData(CommandInfo->Description, OutputStream, (UINT32)strlen(CommandInfo->Description), SHELL_HAL_MAX_TRANSFER_SIZE_IN_BYTES, NexShellWriteTasks);

	if (Result != SHELL_SUCCESS)
		return Result;

	// write out a new line
	if (GenericBufferWrite(OutputStream, SHELL_END_OF_LINE_SEQUENCE_SIZE_IN_BYTES, SHELL_DEFAULT_END_OF_LINE_SEQUENCE) != SHELL_END_OF_LINE_SEQUENCE_SIZE_IN_BYTES)
		return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

	if (DescriptionOnly == FALSE)
	{
		FIL File;

		if (f_open(&File, CommandInfo->Help, FA_OPEN_EXISTING | FA_READ) == SHELL_SUCCESS)
		{
			// now we must read the data out and send it
			UINT DataRead;

			// we did, now start reading the contents
			while (Result == SHELL_SUCCESS && !f_eof(&File))
			{
				// forward more data
				Result = f_forward(&File, help_ForwardData, GenericBufferGetRemainingBytes(OutputStream), &DataRead, (void*)OutputStream, NULL);

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
			Result = NexShellProcessOutgoingData(CommandInfo->Help, OutputStream, (UINT32)strlen(CommandInfo->Help), SHELL_HAL_MAX_TRANSFER_SIZE_IN_BYTES, NexShellWriteTasks);

			if (Result != SHELL_SUCCESS)
				return Result;
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
			if (GenericBufferWrite(OutputStream, strlen(HELP_HELP_TEXT), HELP_HELP_TEXT) != strlen(HELP_HELP_TEXT))
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

				Result = OutputHelpFile(CommandInfo, DescriptionOnly, OutputStream);

				if (Result != SHELL_SUCCESS)
					return Result;
			}

			return SHELL_SUCCESS;
		}
	}
}
