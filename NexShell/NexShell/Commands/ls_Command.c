#include <string.h>

#include "NexShellConfig.h"
#include "NexShell.h"
#include "VirtualFile.h"
#include "ff.h"
#include "ls_Command.h"
#include "NexShellTime.h"

#define INVALID_FILE_SIZE					0xFFFFFFFF

static SHELL_RESULT OutputDirectoryInfo(char* DirectoryName, UINT32 FileSize, UINT16 Date, UINT16 Time, PIPE* OutputStream)
{
	char FileAttributes[6];

	// clear it out first
	memset(FileAttributes, 0, sizeof(FileAttributes));

	FileAttributes[0] = 'd';
	FileAttributes[1] = '-';
	FileAttributes[2] = '-';
	FileAttributes[3] = '-';
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

		if (PipeWrite(OutputStream, Buffer, strlen(Buffer), NULL) != OS_SUCCESS)
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

static SHELL_RESULT OutputFileInfo(char *FileName, UINT32 FileSize, UINT16 Date, UINT16 Time, BOOL Virtual, BOOL Read, BOOL Write, BOOL Execute, char *Description, char *Help, PIPE* OutputStream)
{
	char FileAttributes[6];

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

	if (Help != NULL)
		FileAttributes[4] = 'h';
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
	Result = OutputDirectoryInfo(".", INVALID_FILE_SIZE, 0, 0, OutuputStream);

	if (Result != SHELL_SUCCESS)
		return Result;

	Result = OutputDirectoryInfo("..", INVALID_FILE_SIZE, 0, 0, OutuputStream);

	if (Result != SHELL_SUCCESS)
		return Result;

	return SHELL_SUCCESS;
}

static SHELL_RESULT OutputDirectoryContents(DIR *Directory, BOOL LongFormat, PIPE* OutputStream)
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
			Result = OutputDirectoryInfo(FileInfo.fname, Size, Date, Time, OutputStream);

			if (Result != SHELL_SUCCESS)
				return Result;
		}
		else
		{
			// it was a file
			Result = OutputFileInfo(FileInfo.fname, Size, Date, Time, FALSE, TRUE, !(FileInfo.fattrib & AM_RDO), FALSE, NULL, NULL, OutputStream);

			if (Result != SHELL_SUCCESS)
				return Result;
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
	char CurrentWorkingDirectory[SHELL_MAX_DIRECTORY_SIZE_IN_BYTES + 1];
	BOOL LongFormat = FALSE;

	ArgIndex = 0;

	if (NumberOfArgs != 0)
	{
		// output help if they asked
		if (strcmp(Args[0], "--help") == 0)
		{
			if (PipeWrite(OutputStream, LS_HELP_TEXT, strlen(LS_HELP_TEXT), NULL) != OS_SUCCESS)
				return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

			return SHELL_SUCCESS;
		}

		if (strcmp(Args[ArgIndex], "-l") == 0)
		{
			LongFormat = TRUE;

			ArgIndex++;
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
		Result = OutputDirectoryContents(&Directory, LongFormat, OutputStream);

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