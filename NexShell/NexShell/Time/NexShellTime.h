#ifndef NEXSHELL_TIME_H
	#define NEXSHELL_TIME_H

/** \file
 * This file contains support methods for date and time functions.  There is a design issue
 * with the system that comes down to time.  The file system used (FatFs) wants the date/time
 * data structure in a different format than conventional Linux systems.  So, when getting
 * the data and time of a file it is returned by the FatFs in a format that is unconventional.
 * This file has support methods to deal with that and abstract away the inconvenience of it. 
 */

#include "GenericTypes.h"
#include "ff.h"

 /*!
 * @struct rtc_time
 *
 * @brief This struct holds the date time data for the system.  This structure is what is used everywhere 
 * but the file system when it comes to dates and times.
 *
 * @param int tm_sec - Seconds after the minute - [0, 60] including leap second.
 * 
 * @param int tm_min - Minutes after the hour - [0, 59].
 * 
 * @param int tm_hour - Hours since midnight - [0, 23].
 * 
 * @param int tm_mday - Day of the month - [1, 31].
 * 
 * @param int tm_mon - Months since January - [0, 11].
 * 
 * @param int tm_year - Years since 1900.
 * 
 * @param int tm_wday - Days since Sunday - [0, 6].
 * 
 * @param int tm_yday - Days since January 1 - [0, 365].
 * 
 * @param int tm_isdst - Is daylight savings time.  This is unsupported.
 */
typedef struct 
{
	int tm_sec;
	int tm_min;
	int tm_hour;
	int tm_mday;
	int tm_mon;
	int tm_year;
	int tm_wday;
	int tm_yday;
	int tm_isdst;
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

/*!
 * @brief To strings the month.
 *
 * @param[in] BYTE Month - The 0 based month.  January is 0, February is 1, etc.
 *
 * @return const char* const - The to stringed representation of the month.  This is not an
 * abbreviation of the month, but the full month name.  If the function fails NULL is returned.
 *
 * @details
 * This function is meant to be used with a month that is had from the FatFs file system.
 * For traditional date and time functions refer to the date command or dateCommandExecuteMethod().
 * 
 * @sa dateCommandExecuteMethod()
 */
const char* const MonthToString(BYTE Month);

/*!
 * @brief To strings the weekday.
 *
 * @param[in] BYTE Weekday - The 0 based weekday.  Monday is 0, Tuesday is 1, etc.
 *
 * @return const char* const - The to stringed representation of the weekday.  This is not an
 * abbreviation of the weekday, but the full weekday name.  If the function fails NULL is returned.
 *
 * @details
 * This function is meant to be used with a weekday that is had from the FatFs file system.
 * For traditional date and time functions refer to the date command or dateCommandExecuteMethod().
 *
 * @sa dateCommandExecuteMethod()
 */
const char* const WeekdayToString(BYTE Weekday);

/*!
 * @brief Calcualtes the date of the week.
 *
 * @param[in] BYTE Weekday - Day of the month starting at 1 to 31.
 * @param[in] BYTE Month - The month starting at 1 for January, 2 for February.
 * @param[in] UINT16 Year - The full year in no other format.  If the year is 2024, this should be 2024.
 *
 * @return BYTE - The weekday which is zero based on Sunday.  Sunday is 0, Monday is 1, etc.
 */
BYTE CalculateDayOfWeek(BYTE Day, BYTE Month, UINT16 Year);

DWORD get_fattime(void);

/*!
 * @brief Get the seconds from a \ref PACKED_DATE_TIME.
 *
 * @param[in] UINT16 Time - 
 *
 * @return BYTE - 
 *
 * @details
 * 
 *
 * @sa dateCommandExecuteMethod()
 */
BYTE GetNexShellFileInfoSeconds(UINT16 Time);

BYTE GetNexShellFileInfoMinutes(UINT16 Time);

BYTE GetNexShellFileInfoHours(UINT16 Time);

BYTE GetNexShellFileInfoDay(UINT16 Date);

BYTE GetNexShellFileInfoMonth(UINT16 Date);

UINT16 GetNexShellFileInfoYear(UINT16 Date);

#endif // end of #ifndef NEXSHELL_TIME_H
