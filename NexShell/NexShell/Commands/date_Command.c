#include <string.h>
#include <ctype.h>

#include "ioctl.h"
#include "date_Command.h"
#include "VirtualFile.h"
#include "DevFiles.h"
#include "NexShellTime.h"

SHELL_RESULT dateCommandExecuteMethod(char* Args[], UINT32 NumberOfArgs, GENERIC_BUFFER* OutputStream)
{
	SHELL_RESULT Result;
	UINT32 ArgumentIndex;
	BYTE Filename[32];
	rtc_time CurrentDateTime;

	ArgumentIndex = 0;

	Shell_sprintf(Filename, "%c:/" DEV_FOLDER_NAME "/" RTC_0_FILENAME, NexShellGetRootDriveVolume());

	if (ioctl(Filename, GET_DATE_TIME_CMD, (void*)&CurrentDateTime) != 0)
		return SHELL_IO_CTL_FAILED;

	// now to string it into the buffer
	if (NumberOfArgs == 0)
	{
		// just output the current date and time
		Shell_sprintf(Filename, "%.3s %.3s % 2i %02i:%02i:%02i %i" SHELL_DEFAULT_END_OF_LINE_SEQUENCE, 
			WeekdayToString(CalculateDayOfWeek(CurrentDateTime.tm_mday, CurrentDateTime.tm_mon + 1, CurrentDateTime.tm_year + 1900)),
			MonthToString(CurrentDateTime.tm_mon),
			CurrentDateTime.tm_mday,
			CurrentDateTime.tm_hour,
			CurrentDateTime.tm_min,
			CurrentDateTime.tm_sec,
			CurrentDateTime.tm_year + 1900);

		// write our answer out the stream they passed in
		if (GenericBufferWrite(OutputStream, strlen(Filename), Filename) != strlen(Filename))
			return SHELL_GENERIC_BUFFER_WRITE_FAILURE;
	}
	else
	{
		// they supplied arguments
		if (*Args[ArgumentIndex] == '-')
		{
			// it needs to be 
			if(strlen(Args[ArgumentIndex]) != 2)
				return SHELL_INVALID_INPUT_PARAMETER;

			// they're supplying arguments
			switch (Args[ArgumentIndex++][1])
			{
				case 'd':
				{
					break;
				}

				case 'r':
				{
					break;
				}

				case 's':
				{
					break;
				}

				default:
				{
					// unknown option
					return SHELL_INVALID_INPUT_PARAMETER;
				}
			}

			NumberOfArgs--;
		}

		// are they supplying format specifiers?
		while (NumberOfArgs-- != 0)
		{
			if (Args[ArgumentIndex][0] == '+')
			{
				// they are, lets see what they have done
				char* Ptr = &Args[ArgumentIndex][1];

				while (*Ptr)
				{
					if (*Ptr == '%')
					{
						char TempBuffer[32];

						memset(TempBuffer, 0, sizeof(TempBuffer));

						Ptr++;

						switch (*Ptr)
						{
							// a literal %
							case '%':
							{
								TempBuffer[0] = '%';

								break;
							}

							// locale's abbreviated weekday name (e.g., Sun)
							case 'a':
							{
								Shell_sprintf(TempBuffer, "%.3s", WeekdayToString(CalculateDayOfWeek(CurrentDateTime.tm_mday, CurrentDateTime.tm_mon + 1, CurrentDateTime.tm_year + 1900)));

								break;
							}

							// locale's full weekday name (e.g., Sunday)
							case 'A':
							{
								Shell_sprintf(TempBuffer, "%s", WeekdayToString(CalculateDayOfWeek(CurrentDateTime.tm_mday, CurrentDateTime.tm_mon + 1, CurrentDateTime.tm_year + 1900)));

								break;
							}

							// locale's abbreviated month name (e.g., Jan)
							case 'b':
							case 'h':
							{
								Shell_sprintf(TempBuffer, "%.3s", MonthToString(CurrentDateTime.tm_mon));

								break;
							}

							// locale's full month name (e.g., January)
							case 'B':
							{
								Shell_sprintf(TempBuffer, "%s", MonthToString(CurrentDateTime.tm_mon));

								break;
							}

							// locale's date and time (e.g., Thu Mar 3 23:05:25 2005)
							case 'c':
							{
								Shell_sprintf(TempBuffer, "%.3s %.3s % 2i %02i:%02i:%02i %i",
									WeekdayToString(CalculateDayOfWeek(CurrentDateTime.tm_mday, CurrentDateTime.tm_mon + 1, CurrentDateTime.tm_year + 1900)),
									MonthToString(CurrentDateTime.tm_mon),
									CurrentDateTime.tm_mday,
									CurrentDateTime.tm_hour,
									CurrentDateTime.tm_min,
									CurrentDateTime.tm_sec,
									CurrentDateTime.tm_year + 1900);

								break;
							}

							// century; like% Y, except omit last two digits(e.g., 20)
							case 'C':
							{
								char CentryBuffer[8];

								Shell_sprintf(CentryBuffer, "%i", CurrentDateTime.tm_year + 1900);

								Shell_sprintf(TempBuffer, "%.2s", CentryBuffer);

								break;
							}

							// day of month (e.g, 01)
							case 'd':
							{
								Shell_sprintf(TempBuffer, "%02i", CurrentDateTime.tm_mday);

								break;
							}

							// date; same as %m/%d/%y
							case 'D':
							{
								Shell_sprintf(TempBuffer, "%02i/%02i/%i", CurrentDateTime.tm_mon + 1, CurrentDateTime.tm_mday, CurrentDateTime.tm_year + 1900);

								break;
							}

							// day of month, space padded; same as %_d
							case 'e':
							{
								Shell_sprintf(TempBuffer, "% 2i", CurrentDateTime.tm_mday);

								break;
							}

							// full date; same as %Y-%m-%d
							case 'F':
							{
								Shell_sprintf(TempBuffer, "%i-%02i-%02i", CurrentDateTime.tm_year + 1900, CurrentDateTime.tm_mon + 1, CurrentDateTime.tm_mday);

								break;
							}

							// hour (00..23)
							case 'H':
							{
								Shell_sprintf(TempBuffer, "%02i", CurrentDateTime.tm_hour);

								break;
							}

							// hour (01..12)
							case 'I':
							{
								

								break;
							}

							// day of year (001..366)
							case 'j':
							{
								Shell_sprintf(TempBuffer, "%03i", CurrentDateTime.tm_yday + 1);

								break;
							}

							// hour ( 0..23)
							case 'k':
							{
								Shell_sprintf(TempBuffer, "%i", CurrentDateTime.tm_hour);

								break;
							}

							// hour ( 1..12)
							case 'l':
							{
								

								break;
							}

							// month (01..12)
							case 'm':
							{
								Shell_sprintf(TempBuffer, "%02i", CurrentDateTime.tm_mon + 1);

								break;
							}

							// minute (00..59)
							case 'M':
							{
								Shell_sprintf(TempBuffer, "%02i", CurrentDateTime.tm_min);

								break;
							}

							// a newline
							case 'n':
							{
								Shell_sprintf(TempBuffer, SHELL_DEFAULT_END_OF_LINE_SEQUENCE);

								break;
							}

							// nanoseconds (000000000..999999999)
							case 'N':
							{
								Shell_sprintf(TempBuffer, "%N unsupported");

								break;
							}

							// locale's equivalent of either AM or PM; blank if not known
							case 'p':
							{
								if (CurrentDateTime.tm_hour < 12)
								{
									// it is definetly AM
									strcpy(TempBuffer, "AM");
								}
								else
								{
									strcpy(TempBuffer, "PM");
								}

								break;
							}

							// like %p, but lower case
							case 'P':
							{
								if (CurrentDateTime.tm_hour < 12)
								{
									// it is definetly AM
									strcpy(TempBuffer, "am");
								}
								else
								{
									strcpy(TempBuffer, "pm");
								}

								break;
							}

#if 0
								% r
								locale's 12-hour clock time (e.g., 11:11:04 PM)
								% R
								24 - hour hour and minute; same as% H:% M
								% s
								seconds since 1970 - 01 - 01 00 : 00 : 00 UTC
								% S
								second(00..60)
								% t
								a tab
								% T
								time; same as% H:% M : % S
								% u
								day of week(1..7); 1 is Monday
								% U
								week number of year, with Sunday as first day of week(00..53)
								% V
								ISO week number, with Monday as first day of week(01..53)
								% w
								day of week(0..6); 0 is Sunday
								% W
								week number of year, with Monday as first day of week(00..53)
								% x
								locale's date representation (e.g., 12/31/99)
								% X
								locale's time representation (e.g., 23:13:48)
								% y
								last two digits of year(00..99)
								% Y
								year
								% z
								+ hhmm numeric timezone(e.g., -0400)
								% :z
								+ hh : mm numeric timezone(e.g., -04:00)
								% ::z
								+ hh : mm : ss numeric time zone(e.g., -04:00 : 00)
								% :: : z
								numeric time zone with : to necessary precision(e.g., -04, +05:30)
								% Z
								alphabetic time zone abbreviation(e.g., EDT)
								By default, date pads numeric fields with zeroes.The following optional flags may follow '%' :

								-
								(hyphen) do not pad the field
								_
								(underscore) pad with spaces
								(zero) pad with zeros
								^
								use upper case if possible
#
								use opposite case if possible
								After any flags comes an optional field width, as a decimal number; then an optional modifier, which is either E to use the locale's alternate representations if available, or O to use the locale's alternate numeric symbols if available.

								Date String
								The --date = STRING is a mostly free format human readable date string such as "Sun, 29 Feb 2004 16:21:42 -0800" or "2004-02-29 16:21:42" or even "next Thursday".A date string may contain items indicating calendar date, time of day, time zone, day of week, relative time, relative date, and numbers.An empty string indicates the beginning of the day.The date string format is more complex than is easily documented here but is fully described in the info documentation.
								Environment
								TZ
								Specifies the timezone, unless overridden by command line parameters.If neither is specified, the setting from / etc / localtime is used.
#endif

							default:
							{
								// unknown character
								return SHELL_INVALID_ARGUMENT;
							}
						}

						if (GenericBufferWrite(OutputStream, (UINT32)strlen(TempBuffer), (BYTE*)TempBuffer) != (UINT32)strlen(TempBuffer))
							return SHELL_GENERIC_BUFFER_WRITE_FAILURE;
					}
					else
					{
						if (GenericBufferWrite(OutputStream, 1, (BYTE*)Ptr) != 1)
							return SHELL_GENERIC_BUFFER_WRITE_FAILURE;
					}

					Ptr++;
				}

				if (GenericBufferWrite(OutputStream, SHELL_END_OF_LINE_SEQUENCE_SIZE_IN_BYTES, SHELL_DEFAULT_END_OF_LINE_SEQUENCE) != SHELL_END_OF_LINE_SEQUENCE_SIZE_IN_BYTES)
					return SHELL_GENERIC_BUFFER_WRITE_FAILURE;
			}
		}
	}

	return SHELL_SUCCESS;
}
