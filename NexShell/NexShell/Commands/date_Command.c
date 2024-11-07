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
								BYTE Hour;

								switch (CurrentDateTime.tm_hour)
								{
									case 0:
									{
										Hour = 12;

										break;
									}

									case 1:
									case 2:
									case 3:
									case 4:
									case 5:
									case 6:
									case 7:
									case 8:
									case 9:
									case 10:
									case 11:
									{
										Hour = CurrentDateTime.tm_hour;

										break;
									}

									case 12:
									{
										Hour = 12;

										break;
									}

									default:
									{
										Hour = CurrentDateTime.tm_hour - 12;

										break;
									}
								}

								Shell_sprintf(TempBuffer, "%02i", Hour);

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
								BYTE Hour;

								switch (CurrentDateTime.tm_hour)
								{
									case 0:
									{
										Hour = 12;

										break;
									}

									case 1:
									case 2:
									case 3:
									case 4:
									case 5:
									case 6:
									case 7:
									case 8:
									case 9:
									case 10:
									case 11:
									{
										Hour = CurrentDateTime.tm_hour;

										break;
									}

									case 12:
									{
										Hour = 12;

										break;
									}

									default:
									{
										Hour = CurrentDateTime.tm_hour - 12;

										break;
									}
								}

								Shell_sprintf(TempBuffer, "%i", Hour);

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
								strcat(TempBuffer, "\n");

								break;
							}

							// %N nanoseconds (000000000..999999999)
							// %s seconds since 1970 - 01 - 01 00 : 00 : 00 UTC
							// %U week number of year, with Sunday as first day of week(00..53)
							// %V ISO week number, with Monday as first day of week(01..53)
							// %W week number of year, with Monday as first day of week(00..53)
							// %z + hhmm numeric timezone(e.g., -0400)
							// %Z alphabetic time zone abbreviation(e.g., EDT)
							case 'N':
							case 's':
							case 'U':
							case 'V':
							case 'W':
							case 'z':
							case 'Z':
							{
								Shell_sprintf(TempBuffer, "unsupported");

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

							// locale's 12-hour clock time (e.g., 11:11:04 PM)
							case 'r':
							{
								int Hour;
								char* AM_PM;

								switch (CurrentDateTime.tm_hour)
								{
									case 0:
									{
										AM_PM = "AM";

										Hour = 12;

										break;
									}

									case 1:
									case 2:
									case 3:
									case 4:
									case 5:
									case 6:
									case 7:
									case 8:
									case 9:
									case 10:
									case 11:
									{
										Hour = CurrentDateTime.tm_hour;

										AM_PM = "AM";

										break;
									}

									case 12:
									{
										Hour = 12;

										AM_PM = "PM";

										break;
									}

									default:
									{
										Hour = CurrentDateTime.tm_hour - 12;

										AM_PM = "PM";

										break;
									}
								}

								// 12-hour clock time (e.g., 11:11:04 PM)
								Shell_sprintf(TempBuffer, "%i:%02i:%02i %s", Hour, CurrentDateTime.tm_min, CurrentDateTime.tm_sec, AM_PM);

								break;
							}

							// 24 - hour hour and minute; same as %H:%M
							case 'R':
							{
								Shell_sprintf(TempBuffer, "%02i:%02i", CurrentDateTime.tm_hour, CurrentDateTime.tm_min);

								break;
							}

							// second(00..60)
							case 'S':
							{
								Shell_sprintf(TempBuffer, "%02i", CurrentDateTime.tm_sec);

								break;
							}

							// a tab
							case 't':
							{
								strcpy(TempBuffer, "\t");

								break;
							}

							// time; same as time; same as %H:%M:%S
							case 'T':
							{
								Shell_sprintf(TempBuffer, "%02i:%02i:%02i", CurrentDateTime.tm_hour, CurrentDateTime.tm_min, CurrentDateTime.tm_sec);

								break;
							}

							// day of week(1..7); 1 is Monday
							case 'u':
							{
								if (CurrentDateTime.tm_wday == 0)
								{
									strcpy(TempBuffer, "7");
								}
								else
								{
									char Day[2];

									Day[0] = '0' + CurrentDateTime.tm_wday;
									Day[1] = 0;

									strcpy(TempBuffer, Day);
								}

								break;
							}

							// day of week(0..6); 0 is Sunday
							case 'w':
							{
								TempBuffer[0] = '0' + CurrentDateTime.tm_wday;

								break;
							}

							// locale's date representation (e.g., 12/31/99)
							case 'x':
							{
								Shell_sprintf(TempBuffer, "%02i:%02i:%02i", CurrentDateTime.tm_mon + 1, CurrentDateTime.tm_mday, CurrentDateTime.tm_year + 1900 - 2000);

								break;
							}

							// locale's time representation (e.g., 23:13:48)
							case 'X':
							{
								Shell_sprintf(TempBuffer, "%02i:%02i:%02i", CurrentDateTime.tm_hour, CurrentDateTime.tm_min, CurrentDateTime.tm_sec);

								break;
							}

							// last two digits of year(00..99)
							case 'y':
							{
								Shell_sprintf(TempBuffer, "%i", CurrentDateTime.tm_year + 1900 - 2000);

								break;
							}

							// last two digits of year(00..99)
							case 'Y':
							{
								Shell_sprintf(TempBuffer, "%i", CurrentDateTime.tm_year + 1900);

								break;
							}

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
