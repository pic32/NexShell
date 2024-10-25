#include <string.h>
#include <ctype.h>

#include "NexShell.h"
#include "cat_Command.h"

static BOOL ProcessOptions(char* OptionsString, READ_OPTIONS* Options)
{
	if (*OptionsString++ != '-')
		return FALSE;

	while (*OptionsString)
	{
		switch (*OptionsString++)
		{
			case 'E':
			{
				Options->Bits.ShowLineEnds = 1;

				break;
			}

			case 'n':
			{
				Options->Bits.NumberAllLines = 1;

				break;
			}

			case 's':
			{
				Options->Bits.SupressRepeativeEmptyLines = 1;

				break;
			}

			case 'T':
			{
				Options->Bits.ShowTabs = 1;

				break;
			}

			case 'v':
			{
				Options->Bits.ShowControlCharacters = 1;

				break;
			}

			default:
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}

UINT cat_ForwardData(   /* Returns number of bytes sent or stream status */
	const BYTE* DataToWrite,  /* Pointer to the data block to be sent */
	UINT btf,        /* >0: Transfer call (Number of bytes to be sent). 0: Sense call */
	void *OutputStream,
	void *Options
)
{
	SHELL_RESULT Result;
	char LineNumberBuffer[8];
	READ_INFO* ReadInfo = (READ_INFO*)Options;

	// this is to find out if ready for transfer
	if (btf == 0)
	{
		if (ReadInfo->ReadOptions.Bits.NumberAllLines == 1)
		{
			ReadInfo->LineNumber++;

			Shell_sprintf(LineNumberBuffer, "% 6i ", ReadInfo->LineNumber);

			Result = NexShellProcessOutgoingData(LineNumberBuffer, (GENERIC_BUFFER*)OutputStream, strlen(LineNumberBuffer), SHELL_HAL_MAX_TRANSFER_SIZE_IN_BYTES, NexShellWriteTasks);

			if (Result != SHELL_SUCCESS)
				return 0;
		}

		return 1;
	}

	// if this was set, output the new line and clear the bit
	if (ReadInfo->ReadOptions.Bits.OutputNewLine == 1)
	{
		ReadInfo->ReadOptions.Bits.OutputNewLine = 0;

		if (ReadInfo->ReadOptions.Bits.SupressRepeativeEmptyLines == 0 || (ReadInfo->ReadOptions.Bits.SupressRepeativeEmptyLines == 1 && ReadInfo->NumberOfEmptyLines <= SHELL_NEW_LINE_SUPPRESS_THRESHOLD))
		{
			Shell_sprintf(LineNumberBuffer, "% 6i ", ReadInfo->LineNumber);

			Result = NexShellProcessOutgoingData(LineNumberBuffer, (GENERIC_BUFFER*)OutputStream, strlen(LineNumberBuffer), SHELL_HAL_MAX_TRANSFER_SIZE_IN_BYTES, NexShellWriteTasks);

			if (Result != SHELL_SUCCESS)
				return 0;
		}
	}

	if (ReadInfo->ReadOptions.Value == 0)
	{
		Result = NexShellProcessOutgoingData(DataToWrite, (GENERIC_BUFFER*)OutputStream, strlen(DataToWrite), SHELL_HAL_MAX_TRANSFER_SIZE_IN_BYTES, NexShellWriteTasks);
	}
	else
	{
		// now we have to iterate through and see if we found anything
		UINT32 i, LineNumber;
		char* DataPtr = DataToWrite;
		char* DataStart = DataToWrite;

		LineNumber = 0;

		for (i = 0; i < strlen(DataToWrite); i++)
		{
			switch (*DataPtr++)
			{
				case '\r':
				case '\n':
				{
					// send out what we have currently
					Result = NexShellProcessOutgoingData(DataStart, (GENERIC_BUFFER*)OutputStream, (DataPtr - DataStart) - ReadInfo->ReadOptions.Bits.ShowLineEnds, SHELL_HAL_MAX_TRANSFER_SIZE_IN_BYTES, NexShellWriteTasks);

					if (Result != SHELL_SUCCESS)
						return 0;

					if (*DataPtr == '\n')
					{
						if (ReadInfo->ReadOptions.Bits.NewLinePrior == 1)
						{
							if (ReadInfo->ReadOptions.Bits.SupressRepeativeEmptyLines == 1)
								ReadInfo->NumberOfEmptyLines++;
						}
						else
						{
							ReadInfo->ReadOptions.Bits.NewLinePrior = 1;

							ReadInfo->NumberOfEmptyLines++;
						}

						DataPtr++;
					}

					// point to our new spot
					DataStart = DataPtr;

					// are we outputting end of line characters?
					if (ReadInfo->ReadOptions.Bits.ShowLineEnds == 1 && (ReadInfo->ReadOptions.Bits.SupressRepeativeEmptyLines == 0 || (ReadInfo->ReadOptions.Bits.SupressRepeativeEmptyLines == 1 && ReadInfo->NumberOfEmptyLines <= SHELL_NEW_LINE_SUPPRESS_THRESHOLD)))
					{
						// output a $
						Result = NexShellProcessOutgoingData(END_OF_LINE_CAT_PRINT_CHARACTER SHELL_DEFAULT_END_OF_LINE_SEQUENCE, (GENERIC_BUFFER*)OutputStream, 1 + SHELL_END_OF_LINE_SEQUENCE_SIZE_IN_BYTES, SHELL_HAL_MAX_TRANSFER_SIZE_IN_BYTES, NexShellWriteTasks);

						if (Result != SHELL_SUCCESS)
							return 0;
					}

					// are we numbering lines?
					if (ReadInfo->ReadOptions.Bits.NumberAllLines == 1)
					{
						// increment our line number
						ReadInfo->LineNumber++;

						if (*DataStart == 0)
						{
							// this could be EOF, or just end of the current write, we don't know.
							// mark this so that if the file reenters, it outputs the new line
							ReadInfo->ReadOptions.Bits.OutputNewLine = 1;
						}
						else
						{
							// output the new line if required

							if (ReadInfo->ReadOptions.Bits.SupressRepeativeEmptyLines == 0 || (ReadInfo->ReadOptions.Bits.SupressRepeativeEmptyLines == 1 && ReadInfo->NumberOfEmptyLines <= SHELL_NEW_LINE_SUPPRESS_THRESHOLD))
							{
								Shell_sprintf(LineNumberBuffer, "% 6i ", ReadInfo->LineNumber);

								Result = NexShellProcessOutgoingData(LineNumberBuffer, (GENERIC_BUFFER*)OutputStream, strlen(LineNumberBuffer), SHELL_HAL_MAX_TRANSFER_SIZE_IN_BYTES, NexShellWriteTasks);

								if (Result != SHELL_SUCCESS)
									return 0;
							}
						}
					}

					break;
				}

				case '\t':
				{
					if (ReadInfo->ReadOptions.Bits.SupressRepeativeEmptyLines == 1)
					{
						ReadInfo->NumberOfEmptyLines = 0;
						ReadInfo->ReadOptions.Bits.NewLinePrior = 0;
					}

					// send out what we have currently
					Result = NexShellProcessOutgoingData(DataStart, (GENERIC_BUFFER*)OutputStream, (DataPtr - DataStart) - 1, SHELL_HAL_MAX_TRANSFER_SIZE_IN_BYTES, NexShellWriteTasks);

					if (Result != SHELL_SUCCESS)
						return 0;

					Result = NexShellProcessOutgoingData("^I", (GENERIC_BUFFER*)OutputStream, 2, SHELL_HAL_MAX_TRANSFER_SIZE_IN_BYTES, NexShellWriteTasks);

					if (Result != SHELL_SUCCESS)
						return 0;

					// point to our new spot
					DataStart = DataPtr;

					break;
				}

				default:
				{
					DataPtr--;

					if (ReadInfo->ReadOptions.Bits.SupressRepeativeEmptyLines == 1)
					{
						ReadInfo->NumberOfEmptyLines = 0;
						ReadInfo->ReadOptions.Bits.NewLinePrior = 0;
					}

					// if the character is print, print it out
					if (isprint(*DataPtr) == FALSE && isspace(*DataPtr) == FALSE)
					{
						// its not print

						// do they have the option set for printing non printable characters?
						if (ReadInfo->ReadOptions.Bits.ShowControlCharacters == 1)
						{
							// send out what we have currently
							Result = NexShellProcessOutgoingData(DataStart, (GENERIC_BUFFER*)OutputStream, (DataPtr - DataStart), SHELL_HAL_MAX_TRANSFER_SIZE_IN_BYTES, NexShellWriteTasks);

							if (Result != SHELL_SUCCESS)
								return 0;

							// do they have the option set
							if (*DataPtr < 128)
							{
								// we are under 128, use ^@
								char TempBuffer[2];

								TempBuffer[0] = '^';
								TempBuffer[1] = '@' + *DataPtr;

								// now write the data
								Result = NexShellProcessOutgoingData(TempBuffer, (GENERIC_BUFFER*)OutputStream, 2, SHELL_HAL_MAX_TRANSFER_SIZE_IN_BYTES, NexShellWriteTasks);
								
								if (Result != SHELL_SUCCESS)
									return 0;
							}
							else
							{
								// we are at or over 128, use M-^@
								char TempBuffer[4];

								TempBuffer[0] = 'M';
								TempBuffer[1] = '-';
								TempBuffer[2] = '^';
								TempBuffer[3] = '@' + (*DataPtr) - 128;

								// now write the data
								Result = NexShellProcessOutgoingData(TempBuffer, (GENERIC_BUFFER*)OutputStream, 4, SHELL_HAL_MAX_TRANSFER_SIZE_IN_BYTES, NexShellWriteTasks);

								if (Result != SHELL_SUCCESS)
									return 0;
							}

							// point to our new spot
							DataStart = ++DataPtr;
						}
						else
						{
							DataPtr++;
						}
					}
					else
					{
						DataPtr++;
					}

					break;
				}
			}
		}

		// output any residual data
		if (DataPtr != DataStart)
		{
			// send out what we have currently
			Result = NexShellProcessOutgoingData(DataStart, (GENERIC_BUFFER*)OutputStream, (DataPtr - DataStart), SHELL_HAL_MAX_TRANSFER_SIZE_IN_BYTES, NexShellWriteTasks);

			if (Result != SHELL_SUCCESS)
				return 0;
		}
	}

	return btf;
}
#if(0)
void test(READ_OPTIONS Options, GENERIC_BUFFER* Buffer, char* DataRead, UINT32 NumberOfBytesRead)
{
	char LineNumberBuffer[6];
	UINT32 LineNumber = 0;
	UINT32 NewLinePrior = 0;
	BOOL PrintOutLineNumber = FALSE;

	if (Buffer == NULL)
		return SHELL_INVALID_INPUT_PARAMETER;

	if (DataRead == NULL)
		return SHELL_INVALID_INPUT_PARAMETER;

	if (NumberOfBytesRead == 0)
		return SHELL_SUCCESS;

	// do they even have any options on?
	if (Options.Value == 0)
	{
		// they don't, just write the data out
		return NanoShellProcessOutgoingData(DataRead, Buffer, NumberOfBytesRead, SHELL_HAL_MAX_TRANSFER_SIZE_IN_BYTES, NexShellWriteTasks);
	}

	// we know they have at least 1 option on.
	if (Options.Bits.NumberAllLines == TRUE)
	{
		// initialize the line buffer with all spaces and null terminate it
		LineNumber = 1;

		Shell_sprintf(LineNumberBuffer, "% 4i ", LineNumber);

		if (GenericBufferWrite(Buffer, strlen(LineNumberBuffer), LineNumberBuffer) != strlen(LineNumberBuffer))
			return SHELL_OUTPUT_GENERIC_BUFFER_OVERFLOW;
	}

	while (NumberOfBytesRead)
	{
		// we have enough data for a new line sequence, does it exist?
		if (NumberOfBytesRead >= SHELL_END_OF_LINE_SEQUENCE_SIZE_IN_BYTES)
		{
			// find it if it exists
			if (strncmp(DataRead, SHELL_DEFAULT_END_OF_LINE_SEQUENCE, SHELL_END_OF_LINE_SEQUENCE_SIZE_IN_BYTES) == 0)
			{
				// it does exist, so now what are our options?
				NumberOfBytesRead -= SHELL_END_OF_LINE_SEQUENCE_SIZE_IN_BYTES;

				// increment our line number too
				LineNumber++;

				// do they want to suppress the new lines?
				if (NewLinePrior == SHELL_NEW_LINE_SUPPRESS_THRESHOLD && Options.Bits.SupressRepeativeEmptyLines == 1)
				{
					// increment our pointer
					DataRead += SHELL_END_OF_LINE_SEQUENCE_SIZE_IN_BYTES;
}
				else
				{
					// increment this so when it gets to 2, we don't display if the option
					// is set
					NewLinePrior++;

					// do they want line ends?
					if (Options.Bits.ShowLineEnds == TRUE)
					{
						// output $ before the new line
						if (GenericBufferWrite(Buffer, 1, "$") != 1)
							return SHELL_OUTPUT_GENERIC_BUFFER_OVERFLOW;
					}

					// output the new line sequence
					if (GenericBufferWrite(Buffer, SHELL_END_OF_LINE_SEQUENCE_SIZE_IN_BYTES, DataRead) != SHELL_END_OF_LINE_SEQUENCE_SIZE_IN_BYTES)
						return SHELL_OUTPUT_GENERIC_BUFFER_OVERFLOW;

					// increment our pointer
					DataRead += SHELL_END_OF_LINE_SEQUENCE_SIZE_IN_BYTES;

					// do we number lines?
					if (Options.Bits.NumberAllLines == TRUE && NewLinePrior == SHELL_NEW_LINE_SUPPRESS_THRESHOLD && Options.Bits.SupressRepeativeEmptyLines == 1)
					{
						PrintOutLineNumber = TRUE;
					}
					else
					{
						PrintOutLineNumber = FALSE;

						// update the line number and output the value
						Shell_sprintf(LineNumberBuffer, "% 4i ", LineNumber);

						if (GenericBufferWrite(Buffer, strlen(LineNumberBuffer), LineNumberBuffer) != strlen(LineNumberBuffer))
							return SHELL_OUTPUT_GENERIC_BUFFER_OVERFLOW;
					}
				}

				// don't execute anything else in the loop
				continue;
				}
			}

		NewLinePrior = 0;

		if (PrintOutLineNumber == TRUE)
		{
			PrintOutLineNumber = FALSE;

			// update the line number and output the value
			Shell_sprintf(LineNumberBuffer, "% 4i ", LineNumber);

			if (GenericBufferWrite(Buffer, strlen(LineNumberBuffer), LineNumberBuffer) != strlen(LineNumberBuffer))
				return SHELL_OUTPUT_GENERIC_BUFFER_OVERFLOW;
		}

		// do they want to show tabs?
		if (Options.Bits.ShowTabs == 1)
		{
			// they do, do we have a tab
			if (*DataRead == '\t')
			{
				if (GenericBufferWrite(Buffer, 2, "^I") != 2)
					return SHELL_OUTPUT_GENERIC_BUFFER_OVERFLOW;

				// update our size and pointer
				NumberOfBytesRead -= 1;
				DataRead++;

				// do not execute anymore of the loop
				continue;
			}
		}

		// if the character is print, print it out
		if (isprint(*DataRead) == FALSE && isspace(*DataRead) == FALSE)
		{
			// its not print

			// do they have the option set for printing non printable characters?
			if (Options.Bits.ShowControlCharacters == 1)
			{
				// do they have the option set
				if (*DataRead < 128)
				{
					// we are under 128, use ^@
					char TempBuffer[2];

					TempBuffer[0] = '^';
					TempBuffer[1] = '@' + *DataRead;

					// now write the data
					if (GenericBufferWrite(Buffer, 2, TempBuffer) != 2)
						return SHELL_OUTPUT_GENERIC_BUFFER_OVERFLOW;
				}
				else
				{
					// we are at or over 128, use M-^@
					char TempBuffer[4];

					TempBuffer[0] = 'M';
					TempBuffer[1] = '-';
					TempBuffer[2] = '^';
					TempBuffer[3] = '@' + (*DataRead) - 128;

					// now write the data
					if (GenericBufferWrite(Buffer, 4, TempBuffer) != 4)
						return SHELL_OUTPUT_GENERIC_BUFFER_OVERFLOW;
				}
			}
		}
		else
		{
			// write the data out
			if (GenericBufferWrite(Buffer, 1, DataRead) != 1)
				return SHELL_OUTPUT_GENERIC_BUFFER_OVERFLOW;
		}

		// update our size and pointer
		NumberOfBytesRead -= 1;
		DataRead++;
		}

	return SHELL_SUCCESS;
}
#endif
SHELL_RESULT catCommandExecuteMethod(char* Args[], UINT32 NumberOfArgs, GENERIC_BUFFER* OutputStream)
{
	#if (EXTENDED_CAT_SUPPORT == 1)
		SHELL_FILE* File;
		SHELL_RESULT Result;
		READ_OPTIONS ReadOptions;
		UINT32 ArgsProcessed;

		if (NumberOfArgs == 0)
			return SHELL_INVALID_PARAMETERS;

		// clear it out first
		ReadOptions.Value = 0;

		// zero this out at the start
		ArgsProcessed = 0;

		// get any potential options
		if ((Result = ProcessOptions(Args[0], &ReadOptions)) == TRUE)
		{
			// if we only have one set of arguments, that's an issue
			if (NumberOfArgs == 1)
				return SHELL_INVALID_PARAMETERS;

			ArgsProcessed++;
		}

		do
		{
			// find the file
			File = NanoShellGetWorkingFile(Args[ArgsProcessed], gRootNanoDirectory, gCurrentWorkingNanoDirectory);

			// did we find it?
			if (File == NULL)
				return SHELL_FILE_NOT_FOUND;

			if (File->ReadFileData == NULL)
				return SHELL_FILE_NOT_READABLE;

			Result = File->ReadFileData(Buffer, ReadOptions);

			if (Result != SHELL_SUCCESS)
				return Result;

			if (GenericBufferWrite(Buffer, SHELL_END_OF_LINE_SEQUENCE_SIZE_IN_BYTES, SHELL_DEFAULT_END_OF_LINE_SEQUENCE) != SHELL_END_OF_LINE_SEQUENCE_SIZE_IN_BYTES)
				return SHELL_GENERIC_BUFFER_LIBRARY_FAILED;
		}
		while (++ArgsProcessed != NumberOfArgs);
	#else
		FIL File;
		UINT DataRead;
		SHELL_RESULT Result;
		UINT32 ArgsProcessed;
		READ_INFO ReadInfo;
		char CurrentWorkingDirectory[SHELL_MAX_DIRECTORY_SIZE_IN_BYTES + 1];

		if (NumberOfArgs == 0)
			return SHELL_INSUFFICIENT_ARGUMENTS_FOR_FILE;

		Result = f_getcwd(CurrentWorkingDirectory, sizeof(CurrentWorkingDirectory));

		if (Result != SHELL_SUCCESS)
			return Result;

		// clear it out first
		memset(&ReadInfo, 0, sizeof(ReadInfo));

		// zero this out at the start
		ArgsProcessed = 0;

		// get any potential options
		if ((Result = ProcessOptions(Args[0], &ReadInfo.ReadOptions)) == TRUE)
		{
			// if we only have one set of arguments, that's an issue
			if (NumberOfArgs == 1)
				return SHELL_INVALID_INPUT_PARAMETER;

			ArgsProcessed++;
			NumberOfArgs--;
		}

		while (NumberOfArgs--)
		{
			Result = f_open(&File, Args[ArgsProcessed++], FA_OPEN_EXISTING | FA_READ);

			// did we find it?
			if (Result != SHELL_SUCCESS)
				return Result;

			// we did, now start reading the contents
			while (Result == SHELL_SUCCESS && !f_eof(&File))
			{
				// forward more data
				Result = f_forward(&File, cat_ForwardData, GenericBufferGetRemainingBytes(OutputStream), &DataRead, (void*)OutputStream, (void*)&ReadInfo);

				// did we read ok?
				if (Result != SHELL_SUCCESS)
				{
					f_close(&File);

					return Result;
				}
			}

			// output a new line
			if (GenericBufferWrite(OutputStream, SHELL_END_OF_LINE_SEQUENCE_SIZE_IN_BYTES, SHELL_DEFAULT_END_OF_LINE_SEQUENCE) != SHELL_END_OF_LINE_SEQUENCE_SIZE_IN_BYTES)
				return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

			Result = f_close(&File);

			// did it close?
			if (Result != SHELL_SUCCESS)
				return Result;
		}
	#endif // end of #if (EXTENDED_CAT_SUPPORT == 1)

	return SHELL_SUCCESS;
}