#include "NexShellTime.h"

const char* const gMonth[] =
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

const char* const gWeekday[] =
{
	"Saturday",
	"Sunday",
	"Monday",
	"Tuesday",
	"Wednesday",
	"Thursday",
	"Friday"
};

const char* const MonthToString(BYTE Month)
{
	if (Month > 11)
		return NULL;

	return gMonth[Month];
}

const char* const WeekdayToString(BYTE Weekday)
{
	if (Weekday > 6)
		return NULL;

	return gWeekday[Weekday];
}

BYTE CalculateDayOfWeek(BYTE Day, BYTE Month, UINT16 Year)
{
	if (Month < 3)
	{
		Month += 12;
		Year -= 1;
	}

	int k = Year % 100;
	int j = Year / 100;

	BYTE DayOfWeek = (Day + 13 * (Month + 1) / 5 + k + k / 4 + j / 4 + 5 * j) % 7;

	return DayOfWeek;
}

BYTE GetNexShellPackedDateTimeSeconds(UINT16 Time)
{
	PACKED_DATE_TIME DateTime;

	DateTime.Value = (UINT32)Time;

	return DateTime.BITS.Seconds * 2;
}

BYTE GetNexShellPackedDateTimeMinutes(UINT16 Time)
{
	PACKED_DATE_TIME DateTime;

	DateTime.Value = (UINT32)Time;

	return DateTime.BITS.Minutes;
}

BYTE GetNexShellPackedDateTimeHours(UINT16 Time)
{
	PACKED_DATE_TIME DateTime;

	DateTime.Value = (UINT32)Time;

	return DateTime.BITS.Hours;
}

BYTE GetNexShellPackedDateTimeDay(UINT16 Date)
{
	PACKED_DATE_TIME DateTime;

	DateTime.Value = (UINT32)Date << 16;

	return DateTime.BITS.Day;
}

BYTE GetNexShellPackedDateTimeMonth(UINT16 Date)
{
	PACKED_DATE_TIME DateTime;

	DateTime.Value = (UINT32)Date << 16;

	return DateTime.BITS.Month;
}

UINT16 GetNexShellPackedDateTimeYear(UINT16 Date)
{
	PACKED_DATE_TIME DateTime;

	DateTime.Value = (UINT32)Date << 16;

	return DateTime.BITS.Year + 1980;
}
