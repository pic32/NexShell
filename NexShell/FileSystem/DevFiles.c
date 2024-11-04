#include "DevFiles.h"
#include "VirtualFile.h"

VIRTUAL_FILE gZeroFile, gNullFile, gRandomFile;

const BYTE gZeroFileDescription[] = { "Contains an infinite sequence of zeros" };
const BYTE gNullFileDescription[] = { "Is a black hole for data written to it" };

static SHELL_RESULT ZeroReadFileData(GENERIC_BUFFER* OutputStream)
{
	BYTE Data = 0;

	if (GenericBufferWrite(OutputStream, 1, &Data) != 1)
		return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

	return SHELL_SUCCESS;
}

static SHELL_RESULT NullWriteFileData(char* Args[], UINT32 NumberOfArgs, GENERIC_BUFFER* OutputStream)
{
	// do nothing but return success
	return SHELL_SUCCESS;
}

SHELL_RESULT CreateDevFiles(void)
{
	SHELL_RESULT Result;

	// now create the default dev files
	Result = CreateVirtualFile(&gZeroFile, "zero", ZeroReadFileData, NULL, NULL, gZeroFileDescription, NULL);

	if (Result != SHELL_SUCCESS)
		return Result;

	Result = CreateVirtualFile(&gNullFile, "null", NULL, NullWriteFileData, NULL, gNullFileDescription, NULL);

	if (Result != SHELL_SUCCESS)
		return Result;

	return SHELL_SUCCESS;
}