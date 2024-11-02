#include <string.h>

#include "NexShellConfig.h"
#include "NexShell.h"
#include "ff.h"
#include "ls_Command.h"
#include "VirtualDirectory.h"

extern char gCurrentWorkingDirectory[];
extern VIRTUAL_DIRECTORY* gCurrentWorkingVirtualDirectory;
extern VIRTUAL_DIRECTORY gRootVirtualDirectory;

static SHELL_RESULT OutputDirectoryInfo(char* DirectoryName, GENERIC_BUFFER* OutputStream)
{
	#if (USE_VIRTUAL_FILE_HELP == 1)
		char FileAttributes[6];
	#else
		char FileAttributes[5];
	#endif // end of #if (USE_VIRTUAL_FILE_HELP == 1)

	// clear it out first
	memset(FileAttributes, 0, sizeof(FileAttributes));

	FileAttributes[0] = 'd';
	FileAttributes[1] = '-';
	FileAttributes[2] = '-';
	FileAttributes[3] = '-';

	#if (USE_VIRTUAL_FILE_HELP == 1)
		FileAttributes[4] = '-';
		FileAttributes[5] = ' ';
	#else
		FileAttributes[4] = ' ';
	#endif // end of #if (USE_VIRTUAL_FILE_HELP == 1)

	if (GenericBufferWrite(OutputStream, sizeof(FileAttributes), FileAttributes) != sizeof(FileAttributes))
		return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

	if (strlen(DirectoryName) > SHELL_NUMBER_OF_FILE_CHARACTERS_TO_DISPLAY)
	{
		// display a poriton
		if (GenericBufferWrite(OutputStream, SHELL_NUMBER_OF_FILE_CHARACTERS_TO_DISPLAY, DirectoryName) != SHELL_NUMBER_OF_FILE_CHARACTERS_TO_DISPLAY)
			return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

		if (GenericBufferWrite(OutputStream, 3, "...") != 3)
			return SHELL_GENERIC_BUFFER_WRITE_FAILURE;
	}
	else
	{
		// display them all
		if (GenericBufferWrite(OutputStream, (UINT32)strlen(DirectoryName), DirectoryName) != (UINT32)strlen(DirectoryName))
			return SHELL_GENERIC_BUFFER_WRITE_FAILURE;
	}

	if (GenericBufferWrite(OutputStream, 1 + SHELL_END_OF_LINE_SEQUENCE_SIZE_IN_BYTES, "/"  SHELL_DEFAULT_END_OF_LINE_SEQUENCE) != 1 + SHELL_END_OF_LINE_SEQUENCE_SIZE_IN_BYTES)
		return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

	return SHELL_SUCCESS;
}

static SHELL_RESULT OutputFileInfo(char *FileName, BOOL Virtual, BOOL Read, BOOL Write, BOOL Execute, char *Description, char *Help, GENERIC_BUFFER* OutputStream)
{
		#if (USE_VIRTUAL_FILE_HELP == 1)
			char FileAttributes[6];
		#else
			char FileAttributes[5];
		#endif // end of #if (USE_VIRTUAL_FILE_HELP == 1)

		// clear it out first
		memset(FileAttributes, 0, sizeof(FileAttributes));

		if (Virtual == TRUE)
			FileAttributes[0] = 'v';
		else
			FileAttributes[0] = '-';

		if (Read == TRUE)
			FileAttributes[1] = 'r';
		else
			FileAttributes[1] = '-';

		if (Write == TRUE)
			FileAttributes[2] = 'w';
		else
			FileAttributes[2] = '-';

		if (Execute  == TRUE)
			FileAttributes[3] = 'x';
		else
			FileAttributes[3] = '-';

	#if (USE_VIRTUAL_FILE_HELP == 1)
		if (Help != NULL)
			FileAttributes[4] = 'h';
		else
			FileAttributes[4] = '-';

		FileAttributes[5] = ' ';
	#else
		FileAttributes[4] = ' ';
	#endif // end of #if (USE_VIRTUAL_FILE_HELP == 1)

	if (GenericBufferWrite(OutputStream, sizeof(FileAttributes), FileAttributes) != sizeof(FileAttributes))
		return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

	if (strlen(FileName) > SHELL_NUMBER_OF_FILE_CHARACTERS_TO_DISPLAY)
	{
		// display a poriton
		if (GenericBufferWrite(OutputStream, SHELL_NUMBER_OF_FILE_CHARACTERS_TO_DISPLAY, FileName) != SHELL_NUMBER_OF_FILE_CHARACTERS_TO_DISPLAY)
			return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

		if (GenericBufferWrite(OutputStream, 3, "...") != 3)
			return SHELL_GENERIC_BUFFER_WRITE_FAILURE;
	}
	else
	{
		// display them all
		if (GenericBufferWrite(OutputStream, (UINT32)strlen(FileName), FileName) != (UINT32)strlen(FileName))
			return SHELL_GENERIC_BUFFER_WRITE_FAILURE;
	}

	#if (USE_VIRTUAL_FILE_DESCRIPTION == 1)
		if (Description != NULL)
		{
			if (strlen(FileName) > SHELL_NUMBER_OF_FILE_CHARACTERS_TO_DISPLAY)
			{
				// this is the max space, add one space for the description seperation
				if (GenericBufferWrite(OutputStream, 1, " ") != 1)
					return SHELL_GENERIC_BUFFER_WRITE_FAILURE;
			}
			else
			{
				UINT32 i;

				for (i = 0; i < SHELL_NUMBER_OF_FILE_CHARACTERS_TO_DISPLAY - (UINT32)strlen(FileName) + 3; i++)
					if (GenericBufferWrite(OutputStream, 1, " ") != 1)
						return SHELL_GENERIC_BUFFER_WRITE_FAILURE;
			}

			if (GenericBufferWrite(OutputStream, 2, " -") != 2)
				return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

			if (GenericBufferWrite(OutputStream, (UINT32)strlen(Description), Description) != (UINT32)strlen(Description))
				return SHELL_GENERIC_BUFFER_WRITE_FAILURE;
		}
	#endif // end of #if (USE_VIRTUAL_FILE_DESCRIPTION == 1)

	if (GenericBufferWrite(OutputStream, SHELL_END_OF_LINE_SEQUENCE_SIZE_IN_BYTES, SHELL_DEFAULT_END_OF_LINE_SEQUENCE) != SHELL_END_OF_LINE_SEQUENCE_SIZE_IN_BYTES)
		return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

	return SHELL_SUCCESS;
}

static SHELL_RESULT OutputRelativeDirectories(GENERIC_BUFFER* OutuputStream)
{
	SHELL_RESULT Result;

	// output the relative directories since we're not in root
	Result = OutputDirectoryInfo(".", OutuputStream);

	if (Result != SHELL_SUCCESS)
		return Result;

	Result = OutputDirectoryInfo("..", OutuputStream);

	if (Result != SHELL_SUCCESS)
		return Result;

	return SHELL_SUCCESS;
}

static SHELL_RESULT OutputDirectoryContents(DIR *Directory, GENERIC_BUFFER* OutputStream)
{
	UINT32 Size, i;
	SHELL_RESULT Result;
	FILINFO FileInfo;

	Result = OutputRelativeDirectories(OutputStream);

	if (Result != SHELL_SUCCESS)
		return Result;

	// now output all the directories present
	while(1)
	{
		Result = f_readdir(Directory, &FileInfo);

		if (Result != SHELL_SUCCESS)
			return Result;

		// was there anything?
		if (FileInfo.fname[0] == 0)
			return SHELL_SUCCESS;

		// was it a directory?
		if (FileInfo.fattrib & AM_DIR)
		{
			// it was a directory
			Result = OutputDirectoryInfo(FileInfo.fname, OutputStream);

			if (Result != SHELL_SUCCESS)
				return Result;
		}
		else
		{
			// it was a file
			Result = OutputFileInfo(FileInfo.fname, FALSE, TRUE, !(FileInfo.fattrib & AM_RDO), FALSE, NULL, NULL, OutputStream);

			if (Result != SHELL_SUCCESS)
				return Result;
		}
	}

	return SHELL_SUCCESS;
}

static SHELL_RESULT OutputVirtualFiles(char *Path, GENERIC_BUFFER* OutputStream)
{
	return SHELL_SUCCESS;
}

static SHELL_RESULT OutputVirtualDirectoryDirectories(VIRTUAL_DIRECTORY* LocalWorkingDirectory, GENERIC_BUFFER* OutputStream)
{
	VIRTUAL_DIRECTORY* Directory;
	UINT32 Size, i;
	SHELL_RESULT Result;

	// now output all the directories present
	Size = VirtualDirectoryGetNumberOfDirectories(LocalWorkingDirectory);

	for (i = 1; i <= Size; i++)
	{
		Directory = VirtualDirectoryGetDirectory(LocalWorkingDirectory, i);

		Result = OutputDirectoryInfo(Directory->DirectoryName, OutputStream

			#if (USE_DIRECTORY_DESCRIPTION == 1)
				, Directory->DirectoryDescription
			#endif // end of #ifdef USE_DIRECTORY_DESCRIPTION

		);

		if (Result != SHELL_SUCCESS)
			return Result;
	}

	return SHELL_SUCCESS;
}

static SHELL_RESULT OutputVirtualDirectoryFiles(VIRTUAL_DIRECTORY* LocalWorkingDirectory, GENERIC_BUFFER* OutputStream)
{
	UINT32 Size, i;
	VIRTUAL_FILE* File;
	SHELL_RESULT Result;

	// now output all the file present
	Size = VirtualDirectoryGetNumberOfFiles(LocalWorkingDirectory);

	for (i = 1; i <= Size; i++)
	{
		File = VirtualDirectoryGetFile(LocalWorkingDirectory, i);

		Result = OutputFileInfo(File->FileName, TRUE, (BOOL)(File->ReadFileData != NULL), (BOOL)(File->WriteFileData != NULL), (BOOL)(File->ExecuteFile != NULL), File->FileDescription, File->FileHelp, OutputStream);

		if (Result != SHELL_SUCCESS)
			return Result;
	}

	return SHELL_SUCCESS;
}

SHELL_RESULT VirtuallsExecuteMethod(char* Args[], UINT32 NumberOfArgs, GENERIC_BUFFER* OutputStream)
{
	SHELL_RESULT Result;
	VIRTUAL_DIRECTORY *LocalWorkingDirectory;
	BOOL Outcome;

	// what is the directory
	LocalWorkingDirectory = FollowVirtualDirectory(Args[0], &gRootVirtualDirectory, gCurrentWorkingVirtualDirectory, &Outcome);

	// is it valid?
	if (LocalWorkingDirectory == NULL)
	{
		if (NumberOfArgs != 0)
			return SHELL_INVALID_INPUT_PARAMETER;

		// we are just 1, and need to output the current directory
		LocalWorkingDirectory = gCurrentWorkingVirtualDirectory;
	}

	// first output the current and upper directory shorthand symbols "." and ".." if we're not in root
	if (LocalWorkingDirectory != &gRootVirtualDirectory)
	{
		Result = OutputRelativeDirectories(OutputStream);

		if (Result != SHELL_SUCCESS)
			return Result;
	}

	// output the directories
	Result = OutputVirtualDirectoryDirectories(LocalWorkingDirectory, OutputStream);

	if (Result != SHELL_SUCCESS)
		return Result;

	// output the files
	Result = OutputVirtualDirectoryFiles(LocalWorkingDirectory, OutputStream);

	if (Result != SHELL_SUCCESS)
		return Result;

	return SHELL_SUCCESS;
}

SHELL_RESULT lsCommandExecuteMethod(char* Args[], UINT32 NumberOfArgs, GENERIC_BUFFER* OutputStream)
{
	UINT32 ArgIndex;
	SHELL_RESULT Result;
	char* WorkingDirectoryPath;
	DIR Directory;
	char CurrentWorkingDirectory[SHELL_MAX_DIRECTORY_SIZE_IN_BYTES + 1];

	ArgIndex = 0;

	// Args[0] holds the directory that we will be getting the list files of

	Result = f_getcwd(CurrentWorkingDirectory, sizeof(CurrentWorkingDirectory));

	if (Result != SHELL_SUCCESS)
		return Result;

	do
	{
		// get our next working directory
		// get the current directory
		Result = f_getcwd(CurrentWorkingDirectory, sizeof(CurrentWorkingDirectory));

		if (Result != SHELL_SUCCESS)
			return Result;

		if (Args[ArgIndex] == NULL)
			WorkingDirectoryPath = "./";
		else
			WorkingDirectoryPath = Args[ArgIndex];

		// now get the full path, but this may include some .. which are resolved on the fly
		GetFullDirectoryPath(CurrentWorkingDirectory, WorkingDirectoryPath, NexShellGetRootDriveVolume());
		
		if (IsDirectoryVirtual(CurrentWorkingDirectory) == TRUE || IsDirectoryAtVirtualTransition(CurrentWorkingDirectory) == TRUE)
		{
			char* TempPath = CurrentWorkingDirectory;

			// iterate to the root area of the path
			while (*TempPath != '/')
				TempPath++;

			TempPath++;

			while (*TempPath != '/')
				TempPath++;

			TempPath++;

			return VirtuallsExecuteMethod(&TempPath, NumberOfArgs, OutputStream);
		}

		// open the directory
		Result = f_opendir(&Directory, CurrentWorkingDirectory);

		if (Result != SHELL_SUCCESS)
			return Result;

		// output the directories
		Result = OutputDirectoryContents(&Directory, OutputStream);

		if (Result != SHELL_SUCCESS)
			return Result;
	} 
	while (++ArgIndex < NumberOfArgs);

	return SHELL_SUCCESS;
}