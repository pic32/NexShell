// NexShell.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <time.h>
#include <stdio.h>

#include "ff.h"

typedef union
{
	DWORD Value;

	struct
	{
		unsigned int Seconds: 5; // LSB 0 - 30 (it's seconds divided by 2)
		unsigned int Minutes : 6; // 0 - 59
		unsigned int Hours : 5; // 0 - 23
		unsigned int Day : 5; // 1 - 31
		unsigned int Month : 4; // 1 - 12
		unsigned int Year : 6; // MSB (year since 1980, so 1981 is 1)
	}BITS;
}PACKED_DATE_TIME;

/*
Currnet local time shall be returned as bit-fields packed into a DWORD value. The bit fields are as follows:

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
	time_t rawtime;
	struct tm* timeinfo;
	PACKED_DATE_TIME DateTime;

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	DateTime.Value = 0;

	// get the seconds
	DateTime.BITS.Seconds = timeinfo->tm_sec / 2;

	// get the minutes
	DateTime.BITS.Minutes = timeinfo->tm_min;

	// get the hours
	DateTime.BITS.Hours = timeinfo->tm_hour;

	// get the day
	DateTime.BITS.Day = timeinfo->tm_mday;

	// get the month
	DateTime.BITS.Month = timeinfo->tm_mon + 1;

	// get the year
	DateTime.BITS.Year = ((DWORD)(timeinfo->tm_year + 1900) - 1980);

	return DateTime.Value;
}

BYTE GetFATSeconds(WORD Time)
{
	PACKED_DATE_TIME DateTime;

	DateTime.Value = (DWORD)Time;

	return DateTime.BITS.Seconds * 2;
}

BYTE GetFATMinutes(WORD Time)
{
	PACKED_DATE_TIME DateTime;

	DateTime.Value = (DWORD)Time;

	return DateTime.BITS.Minutes;
}

BYTE GetFATHours(WORD Time)
{
	PACKED_DATE_TIME DateTime;

	DateTime.Value = (DWORD)Time;

	return DateTime.BITS.Hours;
}

BYTE GetFATDay(WORD Date)
{
	PACKED_DATE_TIME DateTime;

	DateTime.Value = (DWORD)Date << 16;

	return DateTime.BITS.Day;
}

BYTE GetFATMonth(WORD Date)
{
	PACKED_DATE_TIME DateTime;

	DateTime.Value = (DWORD)Date << 16;

	return DateTime.BITS.Month;
}

unsigned int GetFATYear(WORD Date)
{
	PACKED_DATE_TIME DateTime;

	DateTime.Value = (DWORD)Date << 16;

	return DateTime.BITS.Year + 1980;
}

FATFS gFatFs;
FRESULT gResult;
FIL gFile;
MKFS_PARM gParams;
unsigned char gBuffer[4096];
FILINFO gFileInfo;

int main()
{
	time_t rawtime;
	struct tm* timeinfo;

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	printf("Current local time and date: %s", asctime(timeinfo));

	gResult = f_mount(&gFatFs, "R:", 1);

	if (gResult != FR_OK)
		printf("Fail value %i\r\n", gResult);

	gResult = f_mkfs("R:", NULL, gBuffer, sizeof(gBuffer));

	if (gResult != FR_OK)
		printf("Fail value %i\r\n", gResult);

	gResult = f_mount(&gFatFs, "R:", 1);

	if (gResult != FR_OK)
		printf("Fail value %i\r\n", gResult);

	gResult = f_open(&gFile, "R:Test.txt", FA_CREATE_ALWAYS);

	if (gResult != FR_OK)
		printf("Fail value %i\r\n", gResult);

	gResult = f_close(&gFile);

	if (gResult != FR_OK)
		printf("Fail value %i\r\n", gResult);

	gResult = f_stat("R:Test.txt", &gFileInfo);

	if (gResult != FR_OK)
		printf("Fail value %i\r\n", gResult);

	printf("File Time: %i\r\nFile Date: %i\r\n", gFileInfo.ftime, gFileInfo.fdate);

	printf("Seconds: %i\r\n", GetFATSeconds(gFileInfo.ftime));
	printf("Minutes: %i\r\n", GetFATMinutes(gFileInfo.ftime));
	printf("Hours: %i\r\n", GetFATHours(gFileInfo.ftime));
	printf("Day: %i\r\n", GetFATDay(gFileInfo.fdate));
	printf("Month: %i\r\n", GetFATMonth(gFileInfo.fdate));
	printf("Year: %i\r\n", GetFATYear(gFileInfo.fdate));


}