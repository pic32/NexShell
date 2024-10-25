// NexShell.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <stdio.h>

#include "NexShell.h"

SHELL_RESULT gShellResult;

void ShellPowerDownCallback(void)
{
	// this is called at power down
}

void StartThread(void);

FIL gFile;
const char gTestTest[] = "This is some test text!\r\n\r\n\r\n\r\nHopefully it works.\r\n\t\t\r\n";
UINT gBytesWritten;

int main()
{
	// start up the thread that'll read in the data input
	StartThread();

	gShellResult = NexShellInit('R');

	if (gShellResult != SHELL_SUCCESS)
	{
		printf("NexShellInit() Failed\r\n");

		exit(0);
	}

	gShellResult = f_open(&gFile, "R:\\Test-a-very-long-file-name.txt", FA_READ | FA_WRITE | FA_CREATE_ALWAYS);

	if (gShellResult != SHELL_SUCCESS)
	{
		printf("f_open() Failed\r\n");

		exit(0);
	}

	gShellResult = f_close(&gFile);

	gShellResult = f_open(&gFile, "R:\\File.txt", FA_READ | FA_WRITE | FA_CREATE_ALWAYS);

	if (gShellResult != SHELL_SUCCESS)
	{
		printf("f_open() Failed\r\n");

		exit(0);
	}

	gShellResult = f_write(&gFile, gTestTest, strlen(gTestTest), &gBytesWritten);

	if (gShellResult != SHELL_SUCCESS)
	{
		printf("f_write() Failed\r\n");

		exit(0);
	}

	gShellResult = f_close(&gFile);

	if (gShellResult != SHELL_SUCCESS)
	{
		printf("f_close() Failed\r\n");

		exit(0);
	}

	gShellResult = f_mkdir("R:/folder1");

	// did it work?
	if (gShellResult != SHELL_SUCCESS)
	{
		printf("f_mkdir() Failed\r\n");

		exit(0);
	}

	gShellResult = f_mkdir("R:/folder2");

	// did it work?
	if (gShellResult != SHELL_SUCCESS)
	{
		printf("f_mkdir() Failed\r\n");

		exit(0);
	}

	gShellResult = f_mkdir("R:/folder2/folder3");

	// did it work?
	if (gShellResult != SHELL_SUCCESS)
	{
		printf("f_mkdir() Failed\r\n");

		exit(0);
	}

	while (1)
		NexShellTasks();
}