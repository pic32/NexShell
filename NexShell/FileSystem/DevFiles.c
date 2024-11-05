#include <stdlib.h>

#include "DevFiles.h"
#include "VirtualFile.h"

#if (USE_DEV_ZERO_VIRTUAL_FILE == 1)
	VIRTUAL_FILE gZeroFile;
	const BYTE gZeroFileDescription[] = { "Contains an infinite sequence of zeros" };

	static SHELL_RESULT ZeroReadFileData(GENERIC_BUFFER* OutputStream)
	{
		BYTE Data = 0;

		if (GenericBufferWrite(OutputStream, 1, &Data) != 1)
			return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

		return SHELL_SUCCESS;
	}
#endif // end of #if (USE_DEV_ZERO_VIRTUAL_FILE == 1)

#if (USE_DEV_NULL_VIRTUAL_FILE == 1)
	VIRTUAL_FILE gNullFile;
	const BYTE gNullFileDescription[] = { "Is a black hole for data written to it" };

	static SHELL_RESULT NullWriteFileData(char* Args[], UINT32 NumberOfArgs, GENERIC_BUFFER* OutputStream)
	{
		// do nothing but return success
		return SHELL_SUCCESS;
	}
#endif // end of #if (USE_DEV_NULL_VIRTUAL_FILE == 1)

#if(USE_DEV_RANDOM_VIRTUAL_FILE == 1)
	VIRTUAL_FILE gRandomFile;
	const BYTE gRandomFileDescription[] = { "Outputs a random number on each read" };

	static SHELL_RESULT RandomReadFileData(GENERIC_BUFFER* OutputStream)
	{
		UINT16 RandomNumber = rand();

		if (GenericBufferWrite(OutputStream, 2, &RandomNumber) != 2)
			return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

		return SHELL_SUCCESS;
	}
#endif // end of #if(USE_DEV_RANDOM_VIRTUAL_FILE == 1)

SHELL_RESULT CreateDevFiles(char RootVolume)
{
	SHELL_RESULT Result;
	char TempBuffer[16];

	Shell_sprintf(TempBuffer, "%c:/" DEV_FOLDER_NAME, RootVolume);

	#if (USE_DEV_ZERO_VIRTUAL_FILE == 1)
		// now create the default dev files
		Result = CreateVirtualFile(&gZeroFile, "zero", ZeroReadFileData, NULL, NULL, gZeroFileDescription, NULL);

		if (Result != SHELL_SUCCESS)
			return Result;

		Result = VirtualFileAddToVirtualFileSystem(&gZeroFile, TempBuffer);

		if (Result != SHELL_SUCCESS)
			return Result;
	#endif // end of #if (USE_DEV_ZERO_VIRTUAL_FILE == 1)

	#if (USE_DEV_NULL_VIRTUAL_FILE == 1)
		Result = CreateVirtualFile(&gNullFile, "null", NULL, NullWriteFileData, NULL, gNullFileDescription, NULL);

		if (Result != SHELL_SUCCESS)
			return Result;

		Result = VirtualFileAddToVirtualFileSystem(&gNullFile, TempBuffer);

		if (Result != SHELL_SUCCESS)
			return Result;
	#endif // end of #if (USE_DEV_NULL_VIRTUAL_FILE == 1)

	#if(USE_DEV_RANDOM_VIRTUAL_FILE == 1)
		Result = CreateVirtualFile(&gRandomFile, "random", RandomReadFileData, NULL, NULL, gRandomFileDescription, NULL);

		if (Result != SHELL_SUCCESS)
			return Result;

		Result = VirtualFileAddToVirtualFileSystem(&gRandomFile, TempBuffer);

		if (Result != SHELL_SUCCESS)
			return Result;
	#endif // end of #if(USE_DEV_RANDOM_VIRTUAL_FILE == 1)

	return SHELL_SUCCESS;
}