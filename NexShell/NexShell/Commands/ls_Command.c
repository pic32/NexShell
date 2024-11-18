#include <string.h>

#include "NexShellConfig.h"
#include "NexShell.h"
#include "VirtualFile.h"
#include "ff.h"
#include "ls_Command.h"
#include "NexShellTime.h"

#define INVALID_FILE_SIZE					0xFFFFFFFF

static BOOL ProcessOptions(char* OptionsString, LS_OPTIONS* Options)
{
	if (*OptionsString++ != '-')
		return FALSE;

	while (*OptionsString)
	{
		switch (*OptionsString++)
		{
			case 'l':
			{
				Options->BITS.LongListing = 1;

				break;
			}

			case 'a':
			{
				Options->BITS.ListAll = 1;

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

static SHELL_RESULT OutputDirectoryInfo(char* DirectoryName, UINT32 FileSize, UINT16 Date, UINT16 Time, BOOL ReadOnly, BOOL Hidden, BOOL System, BOOL Archive, PIPE* OutputStream)
{
	char FileAttributes[6];

	// clear it out first
	memset(FileAttributes, 0, sizeof(FileAttributes));

	FileAttributes[0] = 'd';

	if (ReadOnly == TRUE)
		FileAttributes[1] = 'r';
	else
		FileAttributes[1] = '-';

	if (Hidden == TRUE)
		FileAttributes[2] = 'h';
	else
		FileAttributes[2] = '-';

	if (System == TRUE)
		FileAttributes[3] = 's';
	else
		FileAttributes[3] = '-';

	if (Archive != NULL)
		FileAttributes[4] = 'a';
	else
		FileAttributes[4] = '-';

	FileAttributes[5] = ' ';

	if (PipeWrite(OutputStream, FileAttributes, sizeof(FileAttributes), NULL) != OS_SUCCESS)
		return SHELL_GENERIC_BUFFER_WRITE_FAILURE;
	
	if (FileSize != INVALID_FILE_SIZE)
	{
		BYTE Buffer[16];

		Shell_sprintf(Buffer, "% 10i ", FileSize);

		if (PipeWrite(OutputStream, Buffer, (UINT32)strlen(Buffer), NULL) != OS_SUCCESS)
			return SHELL_GENERIC_BUFFER_WRITE_FAILURE;
	}

	if (Date != 0)
	{
		BYTE Buffer[32];

		Shell_sprintf(Buffer, "%.3s %.3s % 2i %i ", WeekdayToString(CalculateDayOfWeek(GetNexShellFileInfoDay(Date), GetNexShellFileInfoMonth(Date), GetNexShellFileInfoYear(Date))), MonthToString(GetNexShellFileInfoMonth(Date) - 1), GetNexShellFileInfoDay(Date), GetNexShellFileInfoYear(Date));

		if (PipeWrite(OutputStream, Buffer, (UINT32)strlen(Buffer), NULL) != OS_SUCCESS)
			return SHELL_GENERIC_BUFFER_WRITE_FAILURE;
	}

	if (Time != 0)
	{
		BYTE Buffer[32];

		Shell_sprintf(Buffer, "%02i:%02i:%02i ", GetNexShellFileInfoHours(Time), GetNexShellFileInfoMinutes(Time), GetNexShellFileInfoSeconds(Time));

		if (PipeWrite(OutputStream, Buffer, (UINT32)strlen(Buffer), NULL) != OS_SUCCESS)
			return SHELL_GENERIC_BUFFER_WRITE_FAILURE;
	}

	if (strlen(DirectoryName) > SHELL_NUMBER_OF_FILE_CHARACTERS_TO_DISPLAY)
	{
		// display a poriton
		if (PipeWrite(OutputStream, DirectoryName, SHELL_NUMBER_OF_FILE_CHARACTERS_TO_DISPLAY, NULL) != OS_SUCCESS)
			return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

		if (PipeWrite(OutputStream, "...", 3, NULL) != OS_SUCCESS)
			return SHELL_GENERIC_BUFFER_WRITE_FAILURE;
	}
	else
	{
		// display them all
		if (PipeWrite(OutputStream, DirectoryName, (UINT32)strlen(DirectoryName), NULL) != OS_SUCCESS)
			return SHELL_GENERIC_BUFFER_WRITE_FAILURE;
	}

	if (PipeWrite(OutputStream, "/"  SHELL_DEFAULT_END_OF_LINE_SEQUENCE, 1 + SHELL_END_OF_LINE_SEQUENCE_SIZE_IN_BYTES, NULL) != OS_SUCCESS)
		return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

	return SHELL_SUCCESS;
}

static SHELL_RESULT OutputFileInfo(char *FileName, UINT32 FileSize, UINT16 Date, UINT16 Time, BOOL Virtual, BOOL Read_ReadOnly, BOOL Write_Hidden, BOOL Execute_System, char *Description, char *Help_Archive, PIPE* OutputStream)
{
	char FileAttributes[6];

	// clear it out first
	memset(FileAttributes, 0, sizeof(FileAttributes));

	if (Virtual == TRUE)
		FileAttributes[0] = 'v';
	else
		FileAttributes[0] = 'f';

	if (Virtual == TRUE)
	{
		if (Read_ReadOnly == TRUE)
			FileAttributes[1] = 'r';
		else
			FileAttributes[1] = '-';

		if (Write_Hidden == TRUE)
			FileAttributes[2] = 'w';
		else
			FileAttributes[2] = '-';

		if (Execute_System == TRUE)
			FileAttributes[3] = 'x';
		else
			FileAttributes[3] = '-';

		if (Help_Archive != NULL)
			FileAttributes[4] = 'h';
		else
			FileAttributes[4] = '-';
	}
	else
	{
		// we are a disk file, not virtual
		if (Read_ReadOnly == TRUE)
			FileAttributes[1] = 'r';
		else
			FileAttributes[1] = '-';

		if (Write_Hidden == TRUE)
			FileAttributes[2] = 'h';
		else
			FileAttributes[2] = '-';

		if (Execute_System == TRUE)
			FileAttributes[3] = 's';
		else
			FileAttributes[3] = '-';

		if (Help_Archive != NULL)
			FileAttributes[4] = 'a';
		else
			FileAttributes[4] = '-';
	}

	FileAttributes[5] = ' ';

	if (PipeWrite(OutputStream, FileAttributes, sizeof(FileAttributes), NULL) != OS_SUCCESS)
		return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

	if (FileSize != INVALID_FILE_SIZE)
	{
		BYTE Buffer[16];

		Shell_sprintf(Buffer, "% 10i ", FileSize);

		if (PipeWrite(OutputStream, Buffer, (UINT32)strlen(Buffer), NULL) != OS_SUCCESS)
			return SHELL_GENERIC_BUFFER_WRITE_FAILURE;
	}

	if (Date != 0)
	{
		BYTE Buffer[32];

		Shell_sprintf(Buffer, "%.3s %.3s % 2i %i ", WeekdayToString(CalculateDayOfWeek(GetNexShellFileInfoDay(Date), GetNexShellFileInfoMonth(Date), GetNexShellFileInfoYear(Date))), MonthToString(GetNexShellFileInfoMonth(Date) - 1), GetNexShellFileInfoDay(Date), GetNexShellFileInfoYear(Date));

		if (PipeWrite(OutputStream, Buffer, (UINT32)strlen(Buffer), NULL) != OS_SUCCESS)
			return SHELL_GENERIC_BUFFER_WRITE_FAILURE;
	}

	if (Time != 0)
	{
		BYTE Buffer[32];

		Shell_sprintf(Buffer, "%02i:%02i:%02i ", GetNexShellFileInfoHours(Time), GetNexShellFileInfoMinutes(Time), GetNexShellFileInfoSeconds(Time));

		if (PipeWrite(OutputStream, Buffer, (UINT32)strlen(Buffer), NULL) != OS_SUCCESS)
			return SHELL_GENERIC_BUFFER_WRITE_FAILURE;
	}

	if (strlen(FileName) > SHELL_NUMBER_OF_FILE_CHARACTERS_TO_DISPLAY)
	{
		// display a poriton
		if (PipeWrite(OutputStream, FileName, SHELL_NUMBER_OF_FILE_CHARACTERS_TO_DISPLAY, NULL) != OS_SUCCESS)
			return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

		if (PipeWrite(OutputStream, "...", 3, NULL) != OS_SUCCESS)
			return SHELL_GENERIC_BUFFER_WRITE_FAILURE;
	}
	else
	{
		// display them all
		if (PipeWrite(OutputStream, FileName, (UINT32)strlen(FileName), NULL) != OS_SUCCESS)
			return SHELL_GENERIC_BUFFER_WRITE_FAILURE;
	}

	if (Description != NULL)
	{
		if (strlen(FileName) > SHELL_NUMBER_OF_FILE_CHARACTERS_TO_DISPLAY)
		{
			// this is the max space, add one space for the description seperation
			if (PipeWrite(OutputStream, " ", 1, NULL) != OS_SUCCESS)
				return SHELL_GENERIC_BUFFER_WRITE_FAILURE;
		}
		else
		{
			UINT32 i;

			for (i = 0; i < SHELL_NUMBER_OF_FILE_CHARACTERS_TO_DISPLAY - (UINT32)strlen(FileName) + 3; i++)
				if (PipeWrite(OutputStream, " ", 1, NULL) != OS_SUCCESS)
					return SHELL_GENERIC_BUFFER_WRITE_FAILURE;
		}

		if (PipeWrite(OutputStream, " -", 2, NULL) != OS_SUCCESS)
			return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

		if (PipeWrite(OutputStream, Description, (UINT32)strlen(Description), NULL) != OS_SUCCESS)
			return SHELL_GENERIC_BUFFER_WRITE_FAILURE;
	}

	if (PipeWrite(OutputStream, SHELL_DEFAULT_END_OF_LINE_SEQUENCE, SHELL_END_OF_LINE_SEQUENCE_SIZE_IN_BYTES, NULL) != OS_SUCCESS)
		return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

	return SHELL_SUCCESS;
}

static SHELL_RESULT OutputRelativeDirectories(PIPE* OutuputStream)
{
	SHELL_RESULT Result;

	// output the relative directories since we're not in root
	Result = OutputDirectoryInfo(".", INVALID_FILE_SIZE, 0, 0, FALSE, FALSE, FALSE, FALSE, OutuputStream);

	if (Result != SHELL_SUCCESS)
		return Result;

	Result = OutputDirectoryInfo("..", INVALID_FILE_SIZE, 0, 0, FALSE, FALSE, FALSE, FALSE, OutuputStream);

	if (Result != SHELL_SUCCESS)
		return Result;

	return SHELL_SUCCESS;
}

static SHELL_RESULT OutputDirectoryContents(DIR *Directory, BOOL LongFormat, BOOL ShowHidden, PIPE* OutputStream)
{
	SHELL_RESULT Result;
	FILINFO FileInfo;
	UINT16 Time, Date;
	UINT32 Size;

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

		// are we outputting the date?
		if (LongFormat == TRUE)
		{
			Date = FileInfo.fdate;
			Time = FileInfo.ftime;
			Size = FileInfo.fsize;
		}
		else
		{
			Date = 0;
			Time = 0;
			Size = INVALID_FILE_SIZE;
		}

		// was it a directory?
		if (FileInfo.fattrib & AM_DIR)
		{
			// it was a directory
			if (((FileInfo.fattrib & AM_HID) == 0) || (ShowHidden == TRUE))
			{
				Result = OutputDirectoryInfo(FileInfo.fname, Size, Date, Time, (BOOL)((FileInfo.fattrib & AM_RDO) != 0), (BOOL)((FileInfo.fattrib & AM_HID) != 0), (BOOL)((FileInfo.fattrib & AM_SYS) != 0), (BOOL)((FileInfo.fattrib & AM_ARC) != 0), OutputStream);

				if (Result != SHELL_SUCCESS)
					return Result;
			}
		}
		else
		{
			// it was a file
			if (((FileInfo.fattrib & AM_HID) == 0) || (ShowHidden == TRUE))
			{
				Result = OutputFileInfo(FileInfo.fname, Size, Date, Time, FALSE, (BOOL)(FileInfo.fattrib & AM_RDO), (BOOL)(FileInfo.fattrib & AM_HID), (BOOL)(FileInfo.fattrib & AM_SYS), NULL, (char*)(FileInfo.fattrib & AM_ARC), OutputStream);

				if (Result != SHELL_SUCCESS)
					return Result;
			}
		}
	}

	return SHELL_SUCCESS;
}

static SHELL_RESULT OutputVirtualFileList(LINKED_LIST *VirtualFileList, PIPE* OutputStream)
{
	UINT32 Size, i;
	VIRTUAL_FILE* File;
	SHELL_RESULT Result;

	if (VirtualFileList == NULL)
		return SHELL_SUCCESS;

	// now output all the file present
	Size = LinkedListGetSize(VirtualFileList);

	for (i = 1; i <= Size; i++)
	{
		File = LinkedListGetData(VirtualFileList, i);

		Result = OutputFileInfo(File->FileName, INVALID_FILE_SIZE, 0, 0, TRUE, (BOOL)(File->ReadFileData != NULL), (BOOL)(File->WriteFileData != NULL), (BOOL)(File->ExecuteFile != NULL), File->FileDescription, File->FileHelp, OutputStream);

		if (Result != SHELL_SUCCESS)
			return Result;
	}

	return SHELL_SUCCESS;
}

SHELL_RESULT lsCommandExecuteMethod(char* Args[], UINT32 NumberOfArgs, PIPE* OutputStream)
{
	UINT32 ArgIndex;
	SHELL_RESULT Result;
	char* WorkingDirectoryPath;
	DIR Directory;
	LS_OPTIONS Options;
	char CurrentWorkingDirectory[SHELL_MAX_DIRECTORY_SIZE_IN_BYTES + 1];

	// clear it out first
	memset(&Options, 0, sizeof(Options));

	// zero this out at the start
	ArgIndex = 0;

	if (NumberOfArgs != 0)
	{
		// output help if they asked
		if (strcmp(Args[0], "--help") == 0)
		{
			if (PipeWrite(OutputStream, LS_HELP_TEXT, (UINT32)strlen(LS_HELP_TEXT), NULL) != OS_SUCCESS)
				return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

			return SHELL_SUCCESS;
		}

		// get any potential options
		while (*(Args[ArgIndex]) == '-' && NumberOfArgs != 0)
		{
			if ((Result = ProcessOptions(Args[ArgIndex], &Options)) == TRUE)
			{
				ArgIndex++;
				NumberOfArgs--;

				// if we only have one set of arguments, just leave
				if (NumberOfArgs == 0)
					break;
			}
			else
			{
				return SHELL_INVALID_OPTION;
			}
		}
	}

	do
	{
		if (Args[ArgIndex] == NULL)
			WorkingDirectoryPath = "./";
		else
			WorkingDirectoryPath = Args[ArgIndex];

		Result = f_chdir(WorkingDirectoryPath);

		if (Result != SHELL_SUCCESS)
			return Result;

		// get the current directory
		Result = f_getcwd(CurrentWorkingDirectory, sizeof(CurrentWorkingDirectory));

		if (Result != SHELL_SUCCESS)
			return Result;

		// open the directory
		Result = f_opendir(&Directory, CurrentWorkingDirectory);

		if (Result != SHELL_SUCCESS)
			return Result;

		// output the directories
		Result = OutputDirectoryContents(&Directory, (BOOL)Options.BITS.LongListing, (BOOL)Options.BITS.ListAll, OutputStream);

		if (Result != SHELL_SUCCESS)
			return Result;

		// output the virtual files
		Result = OutputVirtualFileList(GetVirtualFileList(CurrentWorkingDirectory), OutputStream);

		if (Result != SHELL_SUCCESS)
			return Result;
	} 
	while (++ArgIndex < NumberOfArgs);

	return SHELL_SUCCESS;
}