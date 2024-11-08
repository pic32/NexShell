#ifndef NEXSHELL_TIME_H
	#define NEXSHELL_TIME_H

#include "GenericTypes.h"
#include "ff.h"

typedef struct 
{
	int tm_sec;   // seconds after the minute - [0, 60] including leap second
	int tm_min;   // minutes after the hour - [0, 59]
	int tm_hour;  // hours since midnight - [0, 23]
	int tm_mday;  // day of the month - [1, 31]
	int tm_mon;   // months since January - [0, 11]
	int tm_year;  // years since 1900
	int tm_wday;  // days since Sunday - [0, 6]
	int tm_yday;  // days since January 1 - [0, 365]
	int tm_isdst; // daylight savings time flag
}rtc_time;

typedef union
{
	UINT32 Value;

	struct
	{
		unsigned int Seconds : 5; // LSB 0 - 30 (it's seconds divided by 2)
		unsigned int Minutes : 6; // 0 - 59
		unsigned int Hours : 5; // 0 - 23
		unsigned int Day : 5; // 1 - 31
		unsigned int Month : 4; // 1 - 12
		unsigned int Year : 6; // MSB (year since 1980, so 1981 is 1)
	}BITS;

	struct
	{
		UINT16 Time;
		UINT16 Date;
	}DATE_TIME;
}PACKED_DATE_TIME;


const char* MonthToString(BYTE Month);

const char* WeekdayToString(BYTE Weekday);

int CalculateDayOfWeek(int day, int month, int year);

DWORD get_fattime(void);

BYTE GetNexShellFileInfoSeconds(UINT16 Time);

BYTE GetNexShellFileInfoMinutes(UINT16 Time);

BYTE GetNexShellFileInfoHours(UINT16 Time);

BYTE GetNexShellFileInfoDay(UINT16 Date);

BYTE GetNexShellFileInfoMonth(UINT16 Date);

UINT16 GetNexShellFileInfoYear(UINT16 Date);

#endif // end of #ifndef NEXSHELL_TIME_H
