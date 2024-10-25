#include <string.h>

#include "help_Command.h"
#include "NexShellConfig.h"

extern COMMAND_INFO gCommandList[];

SHELL_RESULT helpCommandExecuteMethod(char* Args[], UINT32 NumberOfArgs, GENERIC_BUFFER* OutputStream)
{
	SHELL_FILE* File;

	// if there are zero arguments just print out all the global 
	// files that are available
	switch (NumberOfArgs)
	{
		case 0:
		{
			char SpaceBuffer[HELP_COMMAND_NAME_SIZE_IN_BYTES];
			UINT32 i = 0;

			do
			{
				// clear out the buffer with all spaces
				memset(SpaceBuffer, ' ', HELP_COMMAND_NAME_SIZE_IN_BYTES - 2);

				// put a - for the description
				SpaceBuffer[HELP_COMMAND_NAME_SIZE_IN_BYTES - 2] = '-';

				// now null terminate
				SpaceBuffer[HELP_COMMAND_NAME_SIZE_IN_BYTES - 1] = 0;

				// now copy over the command name into the buffer
				memcpy(SpaceBuffer, gCommandList[i].CommandName, strlen(gCommandList[i].CommandName));

				// write the command name with spaces to the buffer
				if (GenericBufferWrite(OutputStream, (UINT32)strlen(SpaceBuffer), SpaceBuffer) != (UINT32)strlen(SpaceBuffer))
					return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

				// now output the description
				if (GenericBufferWrite(OutputStream, (UINT32)strlen(gCommandList[i].Description), gCommandList[i].Description) != (UINT32)strlen(gCommandList[i].Description))
					return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

				if (GenericBufferWrite(OutputStream, SHELL_END_OF_LINE_SEQUENCE_SIZE_IN_BYTES, SHELL_DEFAULT_END_OF_LINE_SEQUENCE) != SHELL_END_OF_LINE_SEQUENCE_SIZE_IN_BYTES)
					return SHELL_GENERIC_BUFFER_WRITE_FAILURE;
			} 
			while (gCommandList[++i].CommandName != NULL);

			return SHELL_SUCCESS;
		}

		default:
		{
			SHELL_RESULT Result;
			UINT32 ArgsProcessed = 0;
			UINT32 i;

			#if (USE_FILE_DESCRIPTION == 1)
				BOOL DescriptionOnly = FALSE;

				// get the options first
				if (strcmp(Args[0], "-d") == 0)
				{
					DescriptionOnly = TRUE;

					ArgsProcessed++;
				}
			#endif // end of #if (USE_FILE_DESCRIPTION == 1)

			// look up a file with the name
			char* Help;
			UINT32 j;

			for (i = 0; i < NumberOfArgs; i++)
			{
				j = 0;

				while (gCommandList[j].CommandName != NULL)
				{
					// look for a match
					if (strcmp(gCommandList[j].CommandName, Args[i]) == 0)
					{
						// we have a match
						if (GenericBufferWrite(OutputStream, (UINT32)strlen(gCommandList[j].CommandName), gCommandList[j].CommandName) != (UINT32)strlen(gCommandList[j].CommandName))
							return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

						if (GenericBufferWrite(OutputStream, (UINT32)strlen(": "), ": ") != (UINT32)2)
							return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

						Result = NexShellProcessOutgoingData(gCommandList[j].Description, OutputStream, strlen(gCommandList[j].Description), SHELL_HAL_MAX_TRANSFER_SIZE_IN_BYTES, NexShellWriteTasks);

						if (Result != SHELL_SUCCESS)
							return Result;

						// write out a new line
						if (GenericBufferWrite(OutputStream, SHELL_END_OF_LINE_SEQUENCE_SIZE_IN_BYTES, SHELL_DEFAULT_END_OF_LINE_SEQUENCE) != SHELL_END_OF_LINE_SEQUENCE_SIZE_IN_BYTES)
							return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

						Result = NexShellProcessOutgoingData(gCommandList[j].Help, OutputStream, strlen(gCommandList[j].Help), SHELL_HAL_MAX_TRANSFER_SIZE_IN_BYTES, NexShellWriteTasks);

						if (Result != SHELL_SUCCESS)
							return Result;
					}

					j++;
				}
			}

			return SHELL_SUCCESS;
		}
	}
}
