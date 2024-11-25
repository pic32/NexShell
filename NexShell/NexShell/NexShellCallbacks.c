#include "NexShellCallbacks.h"
#include "NexShellTime.h"
#include "NexShell.h"
#include "ff.h"
#include "ioctl.h"
#include "DevFiles.h"

extern BYTE gDataRead;

// this is the callback that the shell calls to send data out
UINT32 StreamWriteDataHALCallback(BYTE* DataBuffer, UINT32 DataBufferSizeInBytes)
{
	UINT32 i;

	for (i = 0; i < DataBufferSizeInBytes; i++)
		printf("%c", DataBuffer[i]);

	return DataBufferSizeInBytes;
}

// this is the callback that the shell calls to read data in
UINT32 StreamReaderDataHALCallback(BYTE* DataBuffer, UINT32 DataBufferSizeInBytes)
{
	if (gDataRead != 0)
	{
		*DataBuffer = gDataRead;

		gDataRead = 0;

		if (*DataBuffer++ == '\r')
		{
			*DataBuffer = '\n';

			return 2;
		}

		return 1;
	}
	else
	{
		return 0;
	}
}

/*
	Current local time shall be returned as bit-fields packed into a DWORD value. The bit fields are as follows:

	bit 31:25
	Year origin from the 1980 (0..127, e.g. 37 for 2017)

	bit 24:21
	Month (1..12)

	bit 20:16
	Day of the month (1..31)

	bit 15:11
	Hour (0..23)

	bit 10:5
	Minute (0..59)

	bit 4:0
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

void ShellPowerOffCallback(void)
{
	
}

void ShellPowerDownSleepCallback(void)
{
	
}

void ShellPowerResetCallback(void)
{
	
}
