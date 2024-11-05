#include <string.h>
#include <ctype.h>

#include "date_Command.h"
#include "VirtualFile.h"
#include "DevFiles.h"
#include "NexShellTime.h"

SHELL_RESULT dateCommandExecuteMethod(char* Args[], UINT32 NumberOfArgs, GENERIC_BUFFER* OutputStream)
{
	SHELL_RESULT Result;
	GENERIC_BUFFER Stream;
	VIRTUAL_FILE* rtc0VirtualFile;
	BYTE Buffer[16];
	BYTE Filename[SIZE_OF_SHELL_STACK_BUFFER_IN_BYTES];
	PACKED_DATE_TIME CurrentDateTime;

	if (CreateGenericBuffer(&Stream, sizeof(Buffer), Buffer) == NULL)
		return SHELL_GENERIC_BUFFER_CREATE_FAILURE;

	Shell_sprintf(Filename, "%c:/" DEV_FOLDER_NAME, NexShellGetRootDriveVolume());

	// now read the date time
	rtc0VirtualFile = GetVirtualFile(Filename, RTC_0_FILENAME);

	// did we get it?
	if (rtc0VirtualFile == NULL)
		return SHELL_FILE_NOT_FOUND;

	// now get the current time
	Result = rtc0VirtualFile->ReadFileData(&Stream);

	if (Result != SHELL_SUCCESS)
		return Result;

	if (GenericBufferRead(&Stream, sizeof(PACKED_DATE_TIME), (BYTE*)&CurrentDateTime, sizeof(CurrentDateTime), FALSE) != sizeof(PACKED_DATE_TIME))
		return SHELL_GENERIC_BUFFER_READ_FAILURE;

	// now to string it into the buffer
	Shell_sprintf(Filename, "Year: %i" SHELL_DEFAULT_END_OF_LINE_SEQUENCE "Month: %i" SHELL_DEFAULT_END_OF_LINE_SEQUENCE "Day: %i" SHELL_DEFAULT_END_OF_LINE_SEQUENCE "Hour : %i" SHELL_DEFAULT_END_OF_LINE_SEQUENCE "Minute : %i" SHELL_DEFAULT_END_OF_LINE_SEQUENCE "Second : %i" SHELL_DEFAULT_END_OF_LINE_SEQUENCE, GetNexShellFileInfoYear(CurrentDateTime.DATE_TIME.Date), GetNexShellFileInfoMonth(CurrentDateTime.DATE_TIME.Date), GetNexShellFileInfoDay(CurrentDateTime.DATE_TIME.Date), GetNexShellFileInfoHours(CurrentDateTime.DATE_TIME.Time), GetNexShellFileInfoMinutes(CurrentDateTime.DATE_TIME.Time), GetNexShellFileInfoSeconds(CurrentDateTime.DATE_TIME.Time));

	if(GenericBufferWrite(OutputStream, strlen(Filename), Filename) != strlen(Filename))
		return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

	return SHELL_SUCCESS;
}