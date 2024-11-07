#include <time.h>

#include "NexShellTime.h"
#include "ff.h"
#include "ioctl.h"
#include "NexShell.h"
#include "DevFiles.h"

const char* gMonth[] =
{
	"January",
	"February",
	"March",
	"April",
	"May",
	"June",
	"July",
	"August",
	"September",
	"October",
	"November",
	"December"
};

const char* gWeekday[] =
{
	"Saturday",
	"Sunday",
	"Monday",
	"Tuesday",
	"Wednesday",
	"Thursday",
	"Friday"
};

const char* MonthToString(BYTE Month)
{
	if (Month > 11)
		return NULL;

	return gMonth[Month];
}

const char* WeekdayToString(BYTE Weekday)
{
	if (Weekday > 6)
		return NULL;

	return gWeekday[Weekday];
}

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

/*
	Current local time shall be returned as bit-fields packed into a DWORD value. The bit fields are as follows:

	bit31:25
	Year origin from the 1980 (0..127, e.g. 37 for 2017)

	bit24:21
	Month (1..12)

	bit20:16
	Day of the month (1..31)

	bit15:11
	Hour (0..23)

	bit10:5
	Minute (0..59)

	bit4:0
	Second / 2 (0..29, e.g. 25 for 50)
*/
DWORD get_fattime(void)
{
	PACKED_DATE_TIME DateTime;
	rtc_time TimeInfo;
	BYTE Filename[32];

	Shell_sprintf(Filename, "%c:/" DEV_FOLDER_NAME "/" RTC_0_FILENAME, NexShellGetRootDriveVolume());

	if (ioctl(Filename, GET_DATE_TIME_CMD, (void*)&TimeInfo) != 0)
		return 0;

	DateTime.Value = 0;

	// get the seconds
	DateTime.BITS.Seconds = TimeInfo.tm_sec / 2;

	// get the minutes
	DateTime.BITS.Minutes = TimeInfo.tm_min;

	// get the hours
	DateTime.BITS.Hours = TimeInfo.tm_hour;

	// get the day
	DateTime.BITS.Day = TimeInfo.tm_mday;

	// get the month
	DateTime.BITS.Month = TimeInfo.tm_mon + 1;

	// get the year
	DateTime.BITS.Year = ((DWORD)(TimeInfo.tm_year + 1900) - 1980);

	return DateTime.Value;
}

BYTE GetNexShellFileInfoSeconds(UINT16 Time)
{
	PACKED_DATE_TIME DateTime;

	DateTime.Value = (UINT32)Time;

	return DateTime.BITS.Seconds * 2;
}

BYTE GetNexShellFileInfoMinutes(UINT16 Time)
{
	PACKED_DATE_TIME DateTime;

	DateTime.Value = (UINT32)Time;

	return DateTime.BITS.Minutes;
}

BYTE GetNexShellFileInfoHours(UINT16 Time)
{
	PACKED_DATE_TIME DateTime;

	DateTime.Value = (UINT32)Time;

	return DateTime.BITS.Hours;
}

BYTE GetNexShellFileInfoDay(UINT16 Date)
{
	PACKED_DATE_TIME DateTime;

	DateTime.Value = (UINT32)Date << 16;

	return DateTime.BITS.Day;
}

BYTE GetNexShellFileInfoMonth(UINT16 Date)
{
	PACKED_DATE_TIME DateTime;

	DateTime.Value = (UINT32)Date << 16;

	return DateTime.BITS.Month;
}

UINT16 GetNexShellFileInfoYear(UINT16 Date)
{
	PACKED_DATE_TIME DateTime;

	DateTime.Value = (UINT32)Date << 16;

	return DateTime.BITS.Year + 1980;
}
