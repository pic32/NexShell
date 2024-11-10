#include <string.h>

#include "ioctl.h"
#include "NexShellTime.h"
#include "VirtualFile.h"

long ioctl(char *File, unsigned int cmd, void *arg)
{
	BYTE Filename[SHELL_MAX_DIRECTORY_SIZE_IN_BYTES + 1];
	char* ActualFilename = NULL;
	VIRTUAL_FILE* VirtualFile = NULL;

	if (File != NULL)
	{
		// copy over the file name so we can modify it
		strcpy(Filename, File);

		// get the file name
		ActualFilename = strrchr(Filename, '/');

		if (ActualFilename == NULL)
			return -ENOENT;

		// null out the / and iterate beyond it
		*ActualFilename++ = 0;

		// get a handle on the virtual file 
		VirtualFile = GetVirtualFile(Filename, ActualFilename);

		// did we get it?
		if (VirtualFile == NULL)
			return -ENOENT;
	}

	switch (cmd)
	{
		case GET_DATE_TIME_CMD:
		{
			SHELL_RESULT Result;
			PIPE Stream;
			BYTE Buffer[sizeof(rtc_time)];
			rtc_time *UserTime;

			if(VirtualFile == NULL)
				return -ENOENT;

			UserTime = (rtc_time*)arg;

			// create a temporary buffer for communicating with the rtc0 file
			if (CreateGenericBuffer(&Stream, sizeof(Buffer), Buffer) == NULL)
				return -EIO;

			// now get the current time
			Result = VirtualFile->ReadFileData(&Stream);

			if (Result != SHELL_SUCCESS)
				return -EIO;

			// read our answer from the ioctl inside the file read
			if (PipeRead(&Stream, (BYTE*)UserTime, sizeof(rtc_time), sizeof(rtc_time), NULL) != OS_SUCCESS)
				return -EIO;

			return 0;
		}

		case SYSTEM_SHUTDOWN_SLEEP:
		{
			ShellPowerDownSleep();

			return 0;
		}

		case SYSTEM_SHUTDOWN_RESET:
		{
			ShellPowerReset();

			return 0;
		}

		case SYSTEM_SHUTDOWN_POWER_OFF:
		{
			ShellPowerOff();

			return 0;
		}

		default:
		{
			return ESRCH;
		}
	}
}