 #include <string.h>
#include <ctype.h>

#include "NexShell.h"
#include "VirtualFile.h"
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

							if (ReadInfo->ReadOptions.Bits.SupressRepeativeEmptyLines == 0 || (ReadInfo->ReadOptions.Bits.SupressRepeativeEmptyLines == 1 && ReadInfo->NumberOfEmptyLines < SHELL_NEW_LINE_SUPPRESS_THRESHOLD))
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
						// if they have line count on, output a new line
						if (ReadInfo->NumberOfEmptyLines >= SHELL_NEW_LINE_SUPPRESS_THRESHOLD)
						{
							Shell_sprintf(LineNumberBuffer, "% 6i ", ReadInfo->LineNumber);

							Result = NexShellProcessOutgoingData(LineNumberBuffer, (GENERIC_BUFFER*)OutputStream, strlen(LineNumberBuffer), SHELL_HAL_MAX_TRANSFER_SIZE_IN_BYTES, NexShellWriteTasks);

							if (Result != SHELL_SUCCESS)
								return 0;
						}

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
						// if they have line count on, output a new line
						if (ReadInfo->NumberOfEmptyLines >= SHELL_NEW_LINE_SUPPRESS_THRESHOLD)
						{
							Shell_sprintf(LineNumberBuffer, "% 6i ", ReadInfo->LineNumber);

							Result = NexShellProcessOutgoingData(LineNumberBuffer, (GENERIC_BUFFER*)OutputStream, strlen(LineNumberBuffer), SHELL_HAL_MAX_TRANSFER_SIZE_IN_BYTES, NexShellWriteTasks);

							if (Result != SHELL_SUCCESS)
								return 0;
						}

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

SHELL_RESULT catCommandExecuteMethod(char* Args[], UINT32 NumberOfArgs, GENERIC_BUFFER* OutputStream)
{
	UINT DataRead;
	SHELL_RESULT Result;
	UINT32 ArgsProcessed;
	READ_INFO ReadInfo;
	char CurrentWorkingDirectory[SHELL_MAX_DIRECTORY_SIZE_IN_BYTES + 1];
	char* DirectoryPtr;
	VIRTUAL_FILE* VirtualFile;

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
		FIL File;

		// copy in the path so we can modify it
		strcpy(CurrentWorkingDirectory, Args[ArgsProcessed]);

		// now point to the end and try to find the /
		{
			DirectoryPtr = &CurrentWorkingDirectory[strlen(CurrentWorkingDirectory) - 1];
			BOOL FileNameFound = FALSE;

			do
			{
				if (*DirectoryPtr == '/')
				{
					*DirectoryPtr = 0;

					FileNameFound = TRUE;

					break;
				}
			} 
			while (DirectoryPtr-- != CurrentWorkingDirectory);

			if (FileNameFound == TRUE)
			{
				// set the directory
				Result = f_chdir(CurrentWorkingDirectory);

				if (Result != SHELL_SUCCESS)
					return Result;
			}
			else
			{
				// we didn't find the name, so the user argument is just a file name
				// for the local directory
				DirectoryPtr = Args[ArgsProcessed];

				// now get the current working directory
				// get the current directory
				Result = f_getcwd(CurrentWorkingDirectory, sizeof(CurrentWorkingDirectory));

				if (Result != SHELL_SUCCESS)
					return Result;
			}
		}

		// now is the current directory request virtual?
		VirtualFile = GetVirtualFile(CurrentWorkingDirectory, DirectoryPtr);

		if (VirtualFile == NULL)
		{
			Result = f_open(&File, Args[ArgsProcessed], FA_OPEN_EXISTING | FA_READ);

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
		else
		{
			// we're virtual

			if (VirtualFile->ReadFileData == NULL)
				return SHELL_FILE_NOT_READABLE;

			Result = VirtualFile->ReadFileData(OutputStream);

			if (Result != SHELL_SUCCESS)
				return Result;
		}

		if (GenericBufferWrite(OutputStream, SHELL_END_OF_LINE_SEQUENCE_SIZE_IN_BYTES, SHELL_DEFAULT_END_OF_LINE_SEQUENCE) != SHELL_END_OF_LINE_SEQUENCE_SIZE_IN_BYTES)
			return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

		ArgsProcessed++;

		// clear out the status of the read info
		ReadInfo.LineNumber = 0;
		ReadInfo.NumberOfEmptyLines = 0;
		ReadInfo.ReadOptions.Bits.NewLinePrior = ReadInfo.ReadOptions.Bits.OutputNewLine = 0;
	}

	return SHELL_SUCCESS;
}