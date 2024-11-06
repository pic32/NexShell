#include <string.h>

#include "ioctl.h"
#include "NexShellTime.h"
#include "VirtualFile.h"

long ioctl(char *File, unsigned int cmd, void *arg)
{
	BYTE Filename[SHELL_MAX_DIRECTORY_SIZE_IN_BYTES + 1];
	char* ActualFilename;
	VIRTUAL_FILE* VirtualFile;

	if(File == NULL)
		return -ENOENT;

	// copy over the file name so we can modify it
	strcpy(Filename, File);

	// get the file name
	ActualFilename = strrchr(Filename, '/');

	if(ActualFilename == NULL)
		return -ENOENT;

	// null out the / and iterate beyond it
	*ActualFilename++ = 0;

	// get a handle on the virtual file 
	VirtualFile = GetVirtualFile(Filename, ActualFilename);

	// did we get it?
	if(VirtualFile == NULL)
		return -ENOENT;

	switch (cmd)
	{
		case GET_DATE_TIME_CMD:
		{
			SHELL_RESULT Result;
			GENERIC_BUFFER Stream;
			BYTE Buffer[sizeof(rtc_time)];
			rtc_time *UserTime;

			UserTime = (rtc_time*)arg;

			// create a temporary buffer for communicating with the rtc0 file
			if (CreateGenericBuffer(&Stream, sizeof(Buffer), Buffer) == NULL)
				return -EIO;

			// now get the current time
			Result = VirtualFile->ReadFileData(&Stream);

			if (Result != SHELL_SUCCESS)
				return -EIO;

			// read our answer from the ioctl inside the file read
			if (GenericBufferRead(&Stream, sizeof(rtc_time), (BYTE*)UserTime, sizeof(rtc_time), FALSE) != sizeof(rtc_time))
				return -EIO;

			return 0;
		}

		default:
		{
			return ESRCH;
		}
	}
}