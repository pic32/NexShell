#include "NexShellCallbacks.h"

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

void ShellPowerOffCallback(void)
{
	
}

void ShellPowerDownSleepCallback(void)
{
	
}

void ShellPowerResetCallback(void)
{
	
}
