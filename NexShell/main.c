#include <stdio.h>
#include <string.h>

#include "NexShell.h"
#include "NexShellCommands.h"

SHELL_RESULT gShellResult;

void ShellPowerDownCallback(void)
{
	// this is called at power down
}

void StartThreads(void);

FIL gFile;
const char gTestTest[] = "This is to test the help file text.\r\nMaybe this works.\r\n\tMaybe not...\r\n";
const char gHelpText[] = "R:/help.txt";
const char gADescription[] = "A description to test with";
const char gACommandName[] = "Test_Command";
UINT gBytesWritten;

SHELL_RESULT CommandExecuteFile(char* Args[], UINT32 NumberOfArgs, PIPE *OutputStream)
{
	return SHELL_SUCCESS;
}

int main()
{
	// start up the thread that'll read in the data input
	StartThreads();

	gShellResult = NexShellInit('R');

	if (gShellResult != SHELL_SUCCESS)
	{
		printf("NexShellInit() Failed\r\n");

		exit(0);
	}

	gShellResult = f_open(&gFile, "R:/Test-a-very-long-file-name.txt", FA_READ | FA_WRITE | FA_CREATE_ALWAYS);

	if (gShellResult != SHELL_SUCCESS)
	{
		printf("f_open() Failed\r\n");

		exit(0);
	}

	gShellResult = f_close(&gFile);

	gShellResult = f_open(&gFile, gHelpText, FA_READ | FA_WRITE | FA_CREATE_ALWAYS);

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

	gShellResult = AddUserCommand(gACommandName, gADescription, gHelpText, CommandExecuteFile);

	// did it work?
	if (gShellResult != SHELL_SUCCESS)
	{
		printf("AddUserCommand() Failed\r\n");

		exit(0);
	}

	while (1)
		NexShellTasks();
}