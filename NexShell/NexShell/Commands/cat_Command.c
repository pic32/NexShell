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
				Options->BITS.ShowLineEnds = 1;

				break;
			}

			case 'n':
			{
				Options->BITS.NumberAllLines = 1;

				break;
			}

			case 's':
			{
				Options->BITS.SupressRepeativeEmptyLines = 1;

				break;
			}

			case 'T':
			{
				Options->BITS.ShowTabs = 1;

				break;
			}

			case 'v':
			{
				Options->BITS.ShowControlCharacters = 1;

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
	char LineNumberBuffer[8];
	READ_INFO* ReadInfo = (READ_INFO*)Options;
	UINT btfCopy = btf;

	// this is to find out if ready for transfer
	if (btf == 0)
	{
		if (ReadInfo->ReadOptions.BITS.NumberAllLines == 1)
		{
			ReadInfo->LineNumber++;

			Shell_sprintf(LineNumberBuffer, "% 6i ", ReadInfo->LineNumber++);

			if (PipeWrite((PIPE*)OutputStream, LineNumberBuffer, (UINT32)strlen(LineNumberBuffer), NULL) != OS_SUCCESS)
				return 0;
		}

		return 1;
	}

	if (ReadInfo->ReadOptions.Value == 0)
	{
		if (PipeWrite((PIPE*)OutputStream, (BYTE*)DataToWrite, (UINT32)strlen(DataToWrite), NULL) != OS_SUCCESS)
			return 0;

		return btf;
	}
	else
	{
		// now we have to iterate through and see if we found anything
		UINT i;
		char* DataStart = (char*)DataToWrite;

		for(i = 0; i < btf; i++)
		{
			switch (DataToWrite[i])
			{
				case '\r':
				{
					// send out what we have currently
					if (DataStart != &DataToWrite[i])
						if (PipeWrite((PIPE*)OutputStream, DataStart, (&DataToWrite[i] - DataStart), NULL) != OS_SUCCESS)
							return 0;

					// just set the flag and move on, we don't want to send anything yet
					ReadInfo->ReadOptions.BITS.CarriageReturnPresent = 1;

					// point to our new spot
					DataStart = &DataToWrite[i + 1];

					break;
				}

				case '\n':
				{
					// send out what we have currently
					if (DataStart != &DataToWrite[i])
						if (PipeWrite((PIPE*)OutputStream, DataStart, (&DataToWrite[i] - DataStart), NULL) != OS_SUCCESS)
							return 0;

					// show the line end
					if (ReadInfo->ReadOptions.BITS.ShowLineEnds == 1)
					{
						if (PipeWrite((PIPE*)OutputStream, "$", 1, NULL) != OS_SUCCESS)
							return 0;
					}

					// did they have a '\r'?
					if (ReadInfo->ReadOptions.BITS.CarriageReturnPresent == 1)
					{
						// now clear this
						ReadInfo->ReadOptions.BITS.CarriageReturnPresent = 0;

						if (PipeWrite((PIPE*)OutputStream, "\r", 1, NULL) != OS_SUCCESS)
							return 0;
					}

					// now output the new line
					if (PipeWrite((PIPE*)OutputStream, "\n", 1, NULL) != OS_SUCCESS)
						return 0;

					// if this was set, output the new line and clear the bit
					if (ReadInfo->ReadOptions.BITS.NumberAllLines == 1)
					{
						Shell_sprintf(LineNumberBuffer, "% 6i ", ReadInfo->LineNumber++);

						if (PipeWrite((PIPE*)OutputStream, LineNumberBuffer, (UINT32)strlen(LineNumberBuffer), NULL) != OS_SUCCESS)
							return 0;
					}

					// point to our new spot
					DataStart = &DataToWrite[i + 1];

					break;
				}

				case '\t':
				{
					// did they have a '\r'?
					if (ReadInfo->ReadOptions.BITS.CarriageReturnPresent == 1)
					{
						// now clear this
						ReadInfo->ReadOptions.BITS.CarriageReturnPresent = 0;

						if (PipeWrite((PIPE*)OutputStream, "\r", 1, NULL) != OS_SUCCESS)
							return 0;
					}

					// send out what we have currently
					if(DataStart != &DataToWrite[i])
						if (PipeWrite((PIPE*)OutputStream, DataStart, (&DataToWrite[i] - DataStart), NULL) != OS_SUCCESS)
							return 0;

					if (ReadInfo->ReadOptions.BITS.ShowTabs == 1)
					{
						if (PipeWrite((PIPE*)OutputStream, "^I", 2, NULL) != OS_SUCCESS)
							return 0;
					}
					else
					{
						if (PipeWrite((PIPE*)OutputStream, SHELL_TAB, (UINT32)SHELL_TAB_STRING_LENGTH_IN_BYTES, NULL) != OS_SUCCESS)
							return 0;
					}

					// point to our new spot
					DataStart = &DataToWrite[i + 1];

					break;
				}

				default:
				{
					// did they have a '\r'?
					if (ReadInfo->ReadOptions.BITS.CarriageReturnPresent == 1)
					{
						// now clear this
						ReadInfo->ReadOptions.BITS.CarriageReturnPresent = 0;

						if (PipeWrite((PIPE*)OutputStream, "\r", 1, NULL) != OS_SUCCESS)
							return 0;
					}

					// if the character is print, print it out
					if (isprint(DataToWrite[i]) == FALSE && isspace(DataToWrite[i]) == FALSE)
					{
						// the charcter is not print

						// send out what we have currently
						if (DataStart != &DataToWrite[i])
							if (PipeWrite((PIPE*)OutputStream, DataStart, (&DataToWrite[i] - DataStart), NULL) != OS_SUCCESS)
								return 0;

						if (ReadInfo->ReadOptions.BITS.ShowControlCharacters == 1)
						{
							// do they have the option set
							if (DataToWrite[i] < 128)
							{
								// we are under 128, use ^@
								char TempBuffer[2];

								TempBuffer[0] = '^';
								TempBuffer[1] = '@' + DataToWrite[i];

								// now write the data
								if (PipeWrite((PIPE*)OutputStream, TempBuffer, 2, NULL) != OS_SUCCESS)
									return 0;
							}
							else
							{
								// we are at or over 128, use M-^@
								char TempBuffer[4];

								TempBuffer[0] = 'M';
								TempBuffer[1] = '-';
								TempBuffer[2] = '^';
								TempBuffer[3] = '@' + (DataToWrite[i]) - 128;

								// now write the data
								if (PipeWrite((PIPE*)OutputStream, TempBuffer, 4, NULL) != OS_SUCCESS)
									return 0;
							}
						}

						// point to our new spot
						DataStart = &DataToWrite[i + 1];
					}
					else
					{
						// do nothing
					}

					break;
				}
			}
		}

		// send out what we have currently
		if (DataStart != &DataToWrite[i])
			if (PipeWrite((PIPE*)OutputStream, DataStart, (&DataToWrite[i] - DataStart), NULL) != OS_SUCCESS)
				return 0;
	}

	return btf;
}

SHELL_RESULT catCommandExecuteMethod(char* Args[], UINT32 NumberOfArgs, PIPE* OutputStream)
{
	UINT DataRead;
	SHELL_RESULT Result;
	UINT32 ArgsProcessed;
	READ_INFO ReadInfo;
	char CurrentWorkingDirectory[SHELL_MAX_DIRECTORY_SIZE_IN_BYTES + 1];
	VIRTUAL_FILE* VirtualFile;

	if (NumberOfArgs == 0)
		return SHELL_INSUFFICIENT_ARGUMENTS_FOR_FILE;

	// output help if they asked
	if (strcmp(Args[0], "--help") == 0)
	{
		if (PipeWrite(OutputStream, CAT_HELP_TEXT, (UINT32)strlen(CAT_HELP_TEXT), NULL) != OS_SUCCESS)
			return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

		return SHELL_SUCCESS;
	}

	Result = f_getcwd(CurrentWorkingDirectory, sizeof(CurrentWorkingDirectory));

	if (Result != SHELL_SUCCESS)
		return Result;

	// clear it out first
	memset(&ReadInfo, 0, sizeof(ReadInfo));

	// zero this out at the start
	ArgsProcessed = 0;

	// get any potential options
	while (*(Args[ArgsProcessed]) == '-' && NumberOfArgs != 0)
	{
		if ((Result = ProcessOptions(Args[ArgsProcessed], &ReadInfo.ReadOptions)) == TRUE)
		{
			// if we only have one set of arguments, that's an issue
			if (NumberOfArgs == 1)
				return SHELL_MISSING_OPERANDS;

			ArgsProcessed++;
			NumberOfArgs--;
		}
		else
		{
			return SHELL_INVALID_OPTION;
		}
	}

	while (NumberOfArgs--)
	{
		FIL File;

		// now is the current directory request virtual?
		VirtualFile = GetVirtualFile(Args[ArgsProcessed]);

		if (VirtualFile == NULL)
		{
			// open the file on disk
			Result = f_open(&File, Args[ArgsProcessed], FA_OPEN_EXISTING | FA_READ);

			// did we find it?
			if (Result != SHELL_SUCCESS)
				return Result;

			// we did, now start reading the contents
			while (Result == SHELL_SUCCESS && !f_eof(&File))
			{
				// forward more data
				Result = f_forward(&File, cat_ForwardData, PipeGetRemainingBytes(OutputStream), &DataRead, (void*)OutputStream, (void*)&ReadInfo);

				// did we read ok?
				if (Result != SHELL_SUCCESS)
				{
					f_close(&File);

					return Result;
				}
			}

			// output a new line
			if (PipeWrite(OutputStream, SHELL_DEFAULT_END_OF_LINE_SEQUENCE, SHELL_END_OF_LINE_SEQUENCE_SIZE_IN_BYTES, NULL) != OS_SUCCESS)
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

		ArgsProcessed++;

		// clear out the status of the read info
		ReadInfo.LineNumber = 0;
	}

	// output a new line
	if (PipeWrite(OutputStream, SHELL_DEFAULT_END_OF_LINE_SEQUENCE, SHELL_END_OF_LINE_SEQUENCE_SIZE_IN_BYTES, NULL) != OS_SUCCESS)
		return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

	return SHELL_SUCCESS;
}