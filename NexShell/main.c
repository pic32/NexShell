// NexShell.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <time.h>
#include <stdio.h>

#include "ff.h"

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
	DWORD Time;
	time_t rawtime;
	struct tm* timeinfo;

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	Time = 0;

	// get the seconds
	Time |= timeinfo->tm_sec / 2;

	// get the minutes
	Time |= (timeinfo->tm_min) << 4;

	// get the hours
	Time |= (timeinfo->tm_hour) << 10;

	// get the day
	Time |= (timeinfo->tm_mday) << 15;

	// get the month
	Time |= (timeinfo->tm_mon + 1) << 20;

	// get the year
	Time |= (timeinfo->tm_year - 80) << 24;

	return Time;
}

FATFS gFatFs;
FRESULT gResult;
FIL gFile;
MKFS_PARM gParams;
unsigned char gBuffer[4096];

int main()
{
	time_t rawtime;
	struct tm* timeinfo;

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	printf("Current local time and date: %s", asctime(timeinfo));

	gResult = f_mount(&gFatFs, "", 1);

	if (gResult != FR_OK)
		printf("Fail value %i", gResult);

	//gResult = f_mkfs("", NULL, gBuffer, sizeof(gBuffer));

	//if (gResult != FR_OK)
		//printf("Fail value %i", gResult);

	//gResult = f_mount(&gFatFs, "", 1);

	//if (gResult != FR_OK)
		//printf("Fail value %i", gResult);

	gResult = f_open(&gFile, "", FA_CREATE_ALWAYS);

	if (gResult != FR_OK)
		printf("Fail value %i", gResult);

	// now initialize the fatfs

}