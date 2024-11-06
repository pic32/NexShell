#include <string.h>
#include <ctype.h>

#include "date_Command.h"
#include "VirtualFile.h"
#include "DevFiles.h"
#include "NexShellTime.h"

const char* gMonth[] =
{
	"Jan",
	"Feb",
	"Mar",
	"Apr",
	"May",
	"Jun",
	"Jul",
	"Aug",
	"Sep",
	"Oct",
	"Nov",
	"Dec"
};

const char* gWeekday[] =
{
	"Sat",
	"Sun",
	"Mon",
	"Tue",
	"Wed",
	"Thu",
	"Fri"
};

int CalculateDayOfWeek(int day, int month, int year)
{
	if (month < 3) 
	{
		month += 12;
		year -= 1;
	}

	int k = year % 100;
	int j = year / 100;

	int DayOfWeek = (day + 13 * (month + 1) / 5 + k + k / 4 + j / 4 + 5 * j) % 7;

	return DayOfWeek;
}

SHELL_RESULT dateCommandExecuteMethod(char* Args[], UINT32 NumberOfArgs, GENERIC_BUFFER* OutputStream)
{
	SHELL_RESULT Result;
	GENERIC_BUFFER Stream;
	VIRTUAL_FILE* rtc0VirtualFile;
	BYTE Buffer[16];
	BYTE Filename[SIZE_OF_SHELL_STACK_BUFFER_IN_BYTES];
	PACKED_DATE_TIME CurrentDateTime;

	if (CreateGenericBuffer(&Stream, sizeof(Buffer), Buffer) == NULL)
		return SHELL_GENERIC_BUFFER_CREATE_FAILURE;

	Shell_sprintf(Filename, "%c:/" DEV_FOLDER_NAME, NexShellGetRootDriveVolume());

	// now read the date time
	rtc0VirtualFile = GetVirtualFile(Filename, RTC_0_FILENAME);

	// did we get it?
	if (rtc0VirtualFile == NULL)
		return SHELL_FILE_NOT_FOUND;

	// now get the current time
	Result = rtc0VirtualFile->ReadFileData(&Stream);

	if (Result != SHELL_SUCCESS)
		return Result;

	if (GenericBufferRead(&Stream, sizeof(PACKED_DATE_TIME), (BYTE*)&CurrentDateTime, sizeof(CurrentDateTime), FALSE) != sizeof(PACKED_DATE_TIME))
		return SHELL_GENERIC_BUFFER_READ_FAILURE;

	// now to string it into the buffer
	if (NumberOfArgs == 0)
	{
		// just output the current date and time
		Shell_sprintf(Filename, "%s %s % 2i %02i:%02i:%02i %i" SHELL_DEFAULT_END_OF_LINE_SEQUENCE, 
			gWeekday[CalculateDayOfWeek(GetNexShellFileInfoDay(CurrentDateTime.DATE_TIME.Date), 
			GetNexShellFileInfoMonth(CurrentDateTime.DATE_TIME.Date), 
			GetNexShellFileInfoYear(CurrentDateTime.DATE_TIME.Date))], 
			gMonth[GetNexShellFileInfoMonth(CurrentDateTime.DATE_TIME.Date) - 1], 
			GetNexShellFileInfoDay(CurrentDateTime.DATE_TIME.Date),
			GetNexShellFileInfoHours(CurrentDateTime.DATE_TIME.Time), 
			GetNexShellFileInfoMinutes(CurrentDateTime.DATE_TIME.Time), 
			GetNexShellFileInfoSeconds(CurrentDateTime.DATE_TIME.Time), 
			GetNexShellFileInfoYear(CurrentDateTime.DATE_TIME.Date));
	}
	else
	{
		// they supplied arguments

	}

	if(GenericBufferWrite(OutputStream, strlen(Filename), Filename) != strlen(Filename))
		return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

	return SHELL_SUCCESS;
}
