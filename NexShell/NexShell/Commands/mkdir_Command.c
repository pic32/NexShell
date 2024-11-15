#include <string.h>

#include "NexShell.h"
#include "mkdir_Command.h"

static BOOL ProcessOptions(char* OptionsString, MKDIR_OPTIONS_DATA* Options)
{
	if (*OptionsString++ != '-')
		return FALSE;

	while (*OptionsString)
	{
		switch (*OptionsString++)
		{
			case 'm':
			{
				Options->Options.Bits.Mode = 1;

				// now get the mode

				break;
			}

			case 'p':
			{
				Options->Options.Bits.AllowParentDirectoryCreation = 1;

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

SHELL_RESULT mkdirCommandExecuteMethod(char* Args[], UINT32 NumberOfArgs, PIPE* OutputStream)
{
	SHELL_RESULT Result;
	UINT32 ArgsProcessed;
	MKDIR_OPTIONS_DATA Options;
	char CurrentWorkingDirectory[SHELL_MAX_DIRECTORY_SIZE_IN_BYTES + 1];
	char* DirectoryCutOff;

	if (NumberOfArgs == 0)
		return SHELL_INSUFFICIENT_ARGUMENTS_FOR_FILE;

	// output help if they asked
	if (strcmp(Args[0], "--help") == 0)
	{
		if (PipeWrite(OutputStream, MKDIR_HELP_TEXT, (UINT32)strlen(MKDIR_HELP_TEXT), NULL) != OS_SUCCESS)
			return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

		return SHELL_SUCCESS;
	}

	// clear it out first
	memset(&Options, 0, sizeof(MKDIR_OPTIONS_DATA));

	// zero this out at the start
	ArgsProcessed = 0;

	// get any potential options
	while (*(Args[ArgsProcessed]) == '-' && NumberOfArgs != 0)
	{
		if ((Result = ProcessOptions(Args[ArgsProcessed], &Options)) == TRUE)
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

	Result = f_getcwd(CurrentWorkingDirectory, sizeof(CurrentWorkingDirectory));

	if (Result != SHELL_SUCCESS)
		return Result;

	DirectoryCutOff = &CurrentWorkingDirectory[strlen(CurrentWorkingDirectory)];

	while (NumberOfArgs--)
	{
		char* CurrentDir;
		char* DirectoryName = Args[ArgsProcessed];

		// do we have a valid length?
		if (strlen(DirectoryName) == 0)
			return SHELL_INVALID_DIRECTORY_NAME;

		if (*DirectoryName == '.')
			DirectoryName++;
		
		// is it valid?
		if(*DirectoryName == 0)
			return SHELL_INVALID_DIRECTORY_NAME;

		if (*DirectoryName == '/')
			DirectoryName++;

		// is it valid?
		if (*DirectoryName == 0)
			return SHELL_INVALID_DIRECTORY_NAME;

		// now if we have a '/' we need to have p option enabled
		if (strchr(DirectoryName, '/') != NULL)
		{
			// do we have p?
			if (Options.Options.Bits.AllowParentDirectoryCreation == 0)
			{
				return SHELL_CANNOT_CREATE_DIRECTORY;
			}
		}

		// point to our current sopt for use later
		CurrentDir = DirectoryName;

		// we need to null this out always
		*DirectoryCutOff = 0;

		Result = f_chdir(CurrentWorkingDirectory);

		if (Result != SHELL_SUCCESS)
			return Result;

		do
		{
			DirectoryName = strchr(DirectoryName, '/');

			// null out the / if it is there
			if (DirectoryName != NULL)
				*DirectoryName = 0;

			// now try and create the directory
			Result = f_mkdir(CurrentDir);

			if (Result != SHELL_SUCCESS)
				return Result;

			Result = f_chdir(CurrentDir);

			if (Result != SHELL_SUCCESS)
				return Result;

			// point to after the null
			if (DirectoryName != NULL)
			{
				DirectoryName++;

				CurrentDir = DirectoryName;
			}
		} 
		while (DirectoryName);

		ArgsProcessed++;
	}

	return SHELL_SUCCESS;
}
