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

	while (1)
		NexShellTasks();
}