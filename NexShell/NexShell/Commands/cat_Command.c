#include <string.h>
#include <ctype.h>

#include "NexShell.h"
#include "cat_Command.h"

#if (EXTENDED_CAT_SUPPORT == 1)
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
#endif // end of #if (EXTENDED_CAT_SUPPORT == 1)

UINT cat_ForwardData(   /* Returns number of bytes sent or stream status */
	const BYTE* DataToWrite,  /* Pointer to the data block to be sent */
	UINT btf,        /* >0: Transfer call (Number of bytes to be sent). 0: Sense call */
	void *OutputStream
)
{
	SHELL_RESULT Result;

	// this is to find out if ready for transfer
	if (btf == 0)
		return 1;

	Result = NexShellProcessOutgoingData(DataToWrite, (GENERIC_BUFFER*)OutputStream, strlen(DataToWrite), SHELL_HAL_MAX_TRANSFER_SIZE_IN_BYTES, NexShellWriteTasks);

	if (Result != SHELL_SUCCESS)
		return 0;

	return btf;
}

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
		char CurrentWorkingDirectory[SHELL_MAX_DIRECTORY_SIZE_IN_BYTES + 1];

		if (NumberOfArgs != 1)
			return SHELL_INVALID_INPUT_PARAMETER;

		Result = f_getcwd(CurrentWorkingDirectory, sizeof(CurrentWorkingDirectory));

		if (Result != SHELL_SUCCESS)
			return Result;

		Result = f_open(&File, Args[0], FA_OPEN_EXISTING | FA_READ);

		// did we find it?
		if (Result != SHELL_SUCCESS)
			return Result;

		// we did, now start reading the contents
		while (Result == SHELL_SUCCESS && !f_eof(&File))
		{
			// forward more data
			Result = f_forward(&File, cat_ForwardData, GenericBufferGetRemainingBytes(OutputStream), &DataRead, (void*)OutputStream);

			// did we read ok?
			if (Result != SHELL_SUCCESS)
			{
				f_close(&File);

				return Result;
			}
		}

		if (GenericBufferWrite(OutputStream, SHELL_END_OF_LINE_SEQUENCE_SIZE_IN_BYTES, SHELL_DEFAULT_END_OF_LINE_SEQUENCE) != SHELL_END_OF_LINE_SEQUENCE_SIZE_IN_BYTES)
			return SHELL_GENERIC_BUFFER_WRITE_FAILURE;
	#endif // end of #if (EXTENDED_CAT_SUPPORT == 1)

	return (SHELL_RESULT)f_close(&File);
}