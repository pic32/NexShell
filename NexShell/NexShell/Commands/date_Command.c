#include <string.h>
#include <ctype.h>

#include "ioctl.h"
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
	BYTE Filename[32];
	rtc_time CurrentDateTime;

	Shell_sprintf(Filename, "%c:/" DEV_FOLDER_NAME "/" RTC_0_FILENAME, NexShellGetRootDriveVolume());

	if (ioctl(Filename, GET_DATE_TIME_CMD, (void*)&CurrentDateTime) != 0)
		return SHELL_IO_CTL_FAILED;

	// now to string it into the buffer
	if (NumberOfArgs == 0)
	{
		// just output the current date and time
		Shell_sprintf(Filename, "%s %s % 2i %02i:%02i:%02i %i" SHELL_DEFAULT_END_OF_LINE_SEQUENCE, 
			gWeekday[CalculateDayOfWeek(CurrentDateTime.tm_mday, CurrentDateTime.tm_mon + 1, CurrentDateTime.tm_year + 1900)],
			gMonth[CurrentDateTime.tm_mon],
			CurrentDateTime.tm_mday,
			CurrentDateTime.tm_hour,
			CurrentDateTime.tm_min,
			CurrentDateTime.tm_sec,
			CurrentDateTime.tm_year + 1900);
	}
	else
	{
		// they supplied arguments

	}

	// write our answer out the stream they passed in
	if(GenericBufferWrite(OutputStream, strlen(Filename), Filename) != strlen(Filename))
		return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

	return SHELL_SUCCESS;
}
