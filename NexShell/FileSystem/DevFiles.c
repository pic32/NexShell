#include <stdlib.h>

#include "DevFiles.h"
#include "VirtualFile.h"
#include "ioctl.h"
#include "Pipe.h"

#if (USE_DEV_ZERO_VIRTUAL_FILE == 1)
	VIRTUAL_FILE gZeroFile;
	const BYTE gZeroFileDescription[] = { "Contains an infinite sequence of zeros" };

	static SHELL_RESULT ZeroReadFileData(PIPE* OutputStream)
	{
		BYTE Data = 0;
		UINT32 BytesWritten;

		if (PipeWrite(OutputStream, 1, &Data) != OS_SUCCESS)
			return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

		return SHELL_SUCCESS;
	}
#endif // end of #if (USE_DEV_ZERO_VIRTUAL_FILE == 1)

#if (USE_DEV_NULL_VIRTUAL_FILE == 1)
	VIRTUAL_FILE gNullFile;
	const BYTE gNullFileDescription[] = { "Is a black hole for data written to it" };

	static SHELL_RESULT NullWriteFileData(char* Args[], UINT32 NumberOfArgs, PIPE* OutputStream)
	{
		// do nothing but return success
		return SHELL_SUCCESS;
	}
#endif // end of #if (USE_DEV_NULL_VIRTUAL_FILE == 1)

#if(USE_DEV_RANDOM_VIRTUAL_FILE == 1)
	VIRTUAL_FILE gRandomFile;
	const BYTE gRandomFileDescription[] = { "Outputs a random number on each read" };

	static SHELL_RESULT RandomReadFileData(PIPE* OutputStream)
	{
		UINT16 RandomNumber = rand();

		if (GenericBufferWrite(OutputStream, 2, &RandomNumber) != 2)
			return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

		return SHELL_SUCCESS;
	}
#endif // end of #if(USE_DEV_RANDOM_VIRTUAL_FILE == 1)

#if(USE_DEV_RTC_VIRTUAL_FILE == 1)
	#include "NexShellTime.h"
	#include <time.h>

	VIRTUAL_FILE grtc0File;
	const BYTE grtc0FileDescription[] = { "Provides access to the RTCC" };

	static SHELL_RESULT rtc0ReadFileData(PIPE* OutputStream)
	{
		time_t RawTime;
		struct tm* TimeInfo;
		rtc_time CurrentTime;

		time(&RawTime);

		TimeInfo = localtime(&RawTime);
		
		CurrentTime.tm_sec = TimeInfo->tm_sec;
		CurrentTime.tm_min = TimeInfo->tm_min;
		CurrentTime.tm_hour = TimeInfo->tm_hour;
		CurrentTime.tm_wday = TimeInfo->tm_wday;
		CurrentTime.tm_mday = TimeInfo->tm_mday;
		CurrentTime.tm_mon = TimeInfo->tm_mon;
		CurrentTime.tm_year = TimeInfo->tm_year;
		CurrentTime.tm_isdst = TimeInfo->tm_isdst;
		CurrentTime.tm_yday = TimeInfo->tm_yday;

		if (GenericBufferWrite(OutputStream, sizeof(rtc_time), &CurrentTime) != sizeof(rtc_time))
			return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

		return SHELL_SUCCESS;
	}
#endif // end of #if(USE_DEV_RTC_VIRTUAL_FILE == 1)

SHELL_RESULT CreateDevFiles(char RootVolume)
{
	SHELL_RESULT Result;
	char TempBuffer[16];

	Shell_sprintf(TempBuffer, "%c:/" DEV_FOLDER_NAME, RootVolume);

	#if (USE_DEV_ZERO_VIRTUAL_FILE == 1)
		// now create the default dev files
		Result = CreateVirtualFile(&gZeroFile, "zero", ZeroReadFileData, NULL, NULL, gZeroFileDescription, NULL);

		if (Result != SHELL_SUCCESS)
			return Result;

		Result = VirtualFileAddToVirtualFileSystem(&gZeroFile, TempBuffer);

		if (Result != SHELL_SUCCESS)
			return Result;
	#endif // end of #if (USE_DEV_ZERO_VIRTUAL_FILE == 1)

	#if (USE_DEV_NULL_VIRTUAL_FILE == 1)
		Result = CreateVirtualFile(&gNullFile, "null", NULL, NullWriteFileData, NULL, gNullFileDescription, NULL);

		if (Result != SHELL_SUCCESS)
			return Result;

		Result = VirtualFileAddToVirtualFileSystem(&gNullFile, TempBuffer);

		if (Result != SHELL_SUCCESS)
			return Result;
	#endif // end of #if (USE_DEV_NULL_VIRTUAL_FILE == 1)

	#if(USE_DEV_RANDOM_VIRTUAL_FILE == 1)
		Result = CreateVirtualFile(&gRandomFile, "random", RandomReadFileData, NULL, NULL, gRandomFileDescription, NULL);

		if (Result != SHELL_SUCCESS)
			return Result;

		Result = VirtualFileAddToVirtualFileSystem(&gRandomFile, TempBuffer);

		if (Result != SHELL_SUCCESS)
			return Result;
	#endif // end of #if(USE_DEV_RANDOM_VIRTUAL_FILE == 1)

	#if(USE_DEV_RTC_VIRTUAL_FILE == 1)
		Result = CreateVirtualFile(&grtc0File, RTC_0_FILENAME, rtc0ReadFileData, NULL, NULL, grtc0FileDescription, NULL);

		if (Result != SHELL_SUCCESS)
			return Result;

		Result = VirtualFileAddToVirtualFileSystem(&grtc0File, TempBuffer);

		if (Result != SHELL_SUCCESS)
			return Result;
	#endif // end of #if(USE_DEV_RTC_VIRTUAL_FILE == 1)

	return SHELL_SUCCESS;
}