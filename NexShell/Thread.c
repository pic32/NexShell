// NexShell.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <stdio.h>
#include <windows.h>

HANDLE gThread;

BYTE gDataRead = 0;

// this is the ascii value for the backspace character
#define BACKSPACE_ASCII_VALUE									0x08
#define ESCAPE_ASCII_VALUE										0x1B
#define UP_ARROW_ASCII_VALUE									41
#define DOWN_ARROW_ASCII_VALUE									42
#define RIGHT_ARROW_ASCII_VALUE									43
#define LEFT_ARROW_ASCII_VALUE									44


DWORD WINAPI ThreadFunc(void* data)
{
	BYTE LocalData;

	// Do stuff.  This will be the first function called on the new thread.
	// When this function returns, the thread goes away.  See MSDN for more details.
	while (1)
	{
		LocalData = getch();

		// 224 comes in when it is a special character
		if (LocalData == 0xE0)
		{
			while (gDataRead != 0)
				Sleep(50);

			gDataRead = ESCAPE_ASCII_VALUE;

			LocalData = getch();

			switch (LocalData)
			{
			case 0x48:
			{
				// this is the up arrow
				LocalData = 0x5B;

				while (gDataRead != 0)
					Sleep(50);

				gDataRead = LocalData;

				while (gDataRead != 0)
					Sleep(50);

				LocalData = UP_ARROW_ASCII_VALUE;

				break;
			}

			case 0x4B:
			{
				// this is the left arrow
				LocalData = 0x5B;

				while (gDataRead != 0)
					Sleep(50);

				gDataRead = LocalData;

				while (gDataRead != 0)
					Sleep(50);

				LocalData = LEFT_ARROW_ASCII_VALUE;

				break;
			}

			case 0x4D:
			{
				// this is the right arrow
				LocalData = 0x5B;

				while (gDataRead != 0)
					Sleep(50);

				gDataRead = LocalData;

				while (gDataRead != 0)
					Sleep(50);

				LocalData = RIGHT_ARROW_ASCII_VALUE;

				break;
			}

			case 0x50:
			{
				// this is the down arrow
				LocalData = 0x5B;

				while (gDataRead != 0)
					Sleep(50);

				gDataRead = LocalData;

				while (gDataRead != 0)
					Sleep(50);

				LocalData = DOWN_ARROW_ASCII_VALUE;

				break;
			}
			}
		}

		while (gDataRead != 0)
			Sleep(50);

		gDataRead = LocalData;
	}

	return 0;
}

// this is the callback that the shell calls to send data out
UINT32 StreamWriteDataHALCallback(BYTE* DataBuffer, UINT32 DataBuffersSizeInBytes)
{
	printf("%s", DataBuffer);

	return DataBuffersSizeInBytes;
}

// this is the callback that the shell calls to read data in
UINT32 StreamReaderDataHALCallback(BYTE* DataBuffer, UINT32 DataBuffersSizeInBytes)
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

void StartThread(void)
{
	gThread = CreateThread(NULL, 0, ThreadFunc, NULL, 0, NULL);
}