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
	return 0;
}

int main()
{
	time_t rawtime;
	struct tm* timeinfo;

	time(&rawtime);
	timeinfo = localtime(&rawtime);
	printf("Current local time and date: %s", asctime(timeinfo));
}