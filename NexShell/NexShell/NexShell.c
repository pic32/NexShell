#include <string.h>
#include <ctype.h>

#include "Pipe.h"
#include "LinkedList.h"

#include "ff.h"

#include "NexShell.h"
#include "NexShellConfig.h"
#include "VirtualFile.h"
#include "DevFiles.h"
#include "NexShellCommands.h"

#if (USE_CD_COMMAND == 1)
	#if (EXTENDED_CD_SUPPORT == 1)
		#include "cd_Command.h"
	#endif // end of #if (EXTENDED_CD_SUPPORT == 1)
#endif // end of #if (USE_CD_COMMAND == 1)

#if (SHELL_USE_CONSOLE_ECHO == RUNTIME_CONFIGURABLE)
	BOOL gConsoleEcho;
#endif // end of #if (SHELL_USE_CONSOLE_ECHO == RUNTIME_CONFIGURABLE)

// these are the standard stream inputs and outputs
PIPE gStandardOutputStream, gStandardInputStream, gShellOperatorStream;

// these are the array buffers the gOutputStream and gInputStream use
BYTE gOutputStreamBuffer[SIZE_OF_OUTPUT_STREAM_BUFFER_IN_BYTES];
BYTE gInputStreamBuffer[SIZE_OF_INPUT_STREAM_BUFFER_IN_BYTES];
BYTE gShellOperatorBuffer[SIZE_OF_SHELL_OPERATOR_STREAM_BUFFER_IN_BYTES];

// this is used to keep track of the escape sequences
UINT32 gEscapeSequence;

// this is our main file system object
FATFS gFatFs;

// this is the current directory that the shell is working from, we add 1 for null, and 2 for the drive letter and :
BYTE gCurrentWorkingDirectory[SHELL_MAX_DIRECTORY_SIZE_IN_BYTES + 1 + 2];

// this is the drive letter of the root
BYTE gRootDirectoryDrive;

// these are all the errors to stringed from the SHELL_RESULT data type
const char* gNexShellError[] = {
	"success",
	"fs: disk error",
	"fs: int error",
	"fs: not ready",
	"fs: no file",
	"fs: no path",
	"fs: invalid name",
	"fs: denied",
	"fs: exist",
	"fs: invalid object",
	"fs: write protected",
	"fs: invalid drive",
	"fs: vol not enabled",
	"fs: no fs",
	"fs: mkfs abort",
	"fs: timeout",
	"fs: locked",
	"fs: not enough core",
	"fs: too many open files",
	"fs: invalid param",
	"shell: invalid input parameter",
	"shell: invalid input",
	"shell: argument overflow",
	"shell: invalid char found",
	"shell: insufficient args for command",
	"shell: invalid argument",
	"shell: file not found",
	"shell: file not executable",
	"shell: file not readable",
	"shell: file not writable",
	"shell: history buffer fail",
	"shell: invalid number of bytes transferred",
	"shell: generic buffer create fail",
	"shell: generic buffer write fail",
	"shell: generic buffer read fail",
	"shell: linked list create fail",
	"shell: linked list operation fail",
	"shell: malloc fail",
	"shell: invalid virtual directory name",
	"shell: virtual directory name already exists",
	"shell: virtual filename already exsits",
	"shell: command already exists",
	"shell: command not found",
	"shell: ioctl failed"
};

#if (USE_SHELL_COMMAND_HISTORY == 1)
	LINKED_LIST gHistoryList; // this holds data of type char *
	UINT32 gHistoryIndex; // this is the index inside the linked list to read for hte next history
#endif // end of #if (USE_SHELL_COMMAND_HISTORY == 1)

UINT32 StreamWriteDataHALCallback(BYTE* DataBuffer, UINT32 DataBuffersSizeInBytes);
UINT32 StreamReaderDataHALCallback(BYTE* DataBuffer, UINT32 DataBuffersSizeInBytes);

static SHELL_RESULT OutputPrompt(char *CurrentDirectory, PIPE *OutputStream)
{
	#ifdef SHELL_PROMPT_LEADING_SEQUENCE
		if (PipeWrite(OutputStream, SHELL_PROMPT_BRACKETS_TEXT_COLOR SHELL_PROMPT_LEADING_SEQUENCE, (UINT32)strlen(SHELL_PROMPT_BRACKETS_TEXT_COLOR SHELL_PROMPT_LEADING_SEQUENCE), NULL) != OS_SUCCESS)
			return SHELL_GENERIC_BUFFER_WRITE_FAILURE;
	#endif // end of SHELL_PROMPT_LEADING_SEQUENCE

	#ifdef SHELL_USERNAME
		if (PipeWrite(OutputStream, SHELL_USERNAME_TEXT_COLOR SHELL_USERNAME, (UINT32)strlen(SHELL_USERNAME_TEXT_COLOR SHELL_USERNAME), NULL) != OS_SUCCESS)
			return SHELL_GENERIC_BUFFER_WRITE_FAILURE;
	#endif // end of SHELL_USERNAME

	#ifdef SHELL_USERNAME_DIRECTORY_SEPERATION_SEQUENCE
		if (PipeWrite(OutputStream, SHELL_PROMPT_BRACKETS_TEXT_COLOR SHELL_USERNAME_DIRECTORY_SEPERATION_SEQUENCE, (UINT32)strlen(SHELL_PROMPT_BRACKETS_TEXT_COLOR SHELL_USERNAME_DIRECTORY_SEPERATION_SEQUENCE), NULL) != OS_SUCCESS)
			return SHELL_GENERIC_BUFFER_WRITE_FAILURE;
	#endif // end of SHELL_USERNAME_DIRECTORY_SEPERATION_SEQUENCE

	#if (USE_SHELL_COLOR == 1)
		if (PipeWrite(OutputStream, SHELL_DIRECTORY_TEXT_COLOR, (UINT32)strlen(SHELL_DIRECTORY_TEXT_COLOR), NULL) != OS_SUCCESS)
			return SHELL_GENERIC_BUFFER_WRITE_FAILURE;
	#endif // end of #if (USE_SHELL_COLOR == 1)

	if (PipeWrite(OutputStream, (const BYTE*)CurrentDirectory, (UINT32)strlen(CurrentDirectory), NULL) != OS_SUCCESS)
		return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

	#ifdef SHELL_PROMPT_ENDING_SEQUENCE
		if (PipeWrite(OutputStream, SHELL_PROMPT_BRACKETS_TEXT_COLOR SHELL_PROMPT_ENDING_SEQUENCE, (UINT32)strlen(SHELL_PROMPT_BRACKETS_TEXT_COLOR SHELL_PROMPT_ENDING_SEQUENCE), NULL) != OS_SUCCESS)
			return SHELL_GENERIC_BUFFER_WRITE_FAILURE;
	#endif // end of SHELL_PROMPT_ENDING_SEQUENCE

	#ifdef SHELL_ATTENTION_CHARACTER
		if (PipeWrite(OutputStream, SHELL_ATTENTION_CHARACTER_TEXT_COLOR SHELL_ATTENTION_CHARACTER, (UINT32)strlen(SHELL_ATTENTION_CHARACTER_TEXT_COLOR SHELL_ATTENTION_CHARACTER), NULL) != OS_SUCCESS)
			return SHELL_GENERIC_BUFFER_WRITE_FAILURE;
	#endif // end of SHELL_ATTENTION_CHARACTER

	#if (USE_SHELL_COLOR == 1)
		if (PipeWrite(OutputStream, SHELL_DEFAULT_TEXT_COLOR, (UINT32)strlen(SHELL_DEFAULT_TEXT_COLOR), NULL) != OS_SUCCESS)
			return SHELL_GENERIC_BUFFER_WRITE_FAILURE;
	#endif // end of #if (USE_SHELL_COLOR == 1)

	if (PipeWrite(OutputStream, (const BYTE*)" ", 1, NULL) != OS_SUCCESS)
		return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

	return SHELL_SUCCESS;
}

static char* GetLastDirectoryPresent(char* DirectoryPath)
{
	char* LastDirectory = &DirectoryPath[strlen(DirectoryPath)];

	while (*LastDirectory != '/')
		LastDirectory--;

	return LastDirectory;
}

char NexShellGetRootDriveVolume(void)
{
	return gRootDirectoryDrive;
}

SHELL_RESULT NexShellInit(char CurrentDrive)
{
	SHELL_RESULT Result;

	gEscapeSequence = 0;

	// initialize the virtual file list section
	Result = VirtualFileInit();

	// did it work?
	if (Result != SHELL_SUCCESS)
		return Result;

	// now create the default dev files
	Result = CreateDevFiles(CurrentDrive);

	if (Result != SHELL_SUCCESS)
		return Result;

	#if (USE_CD_COMMAND == 1)
		#if (EXTENDED_CD_SUPPORT == 1)
			Result = cdInit();

			// did it work?
			if (Result != SHELL_SUCCESS)
				return Result;
		#endif // end of #if (EXTENDED_CD_SUPPORT == 1)
	#endif // end of #if (USE_CD_COMMAND == 1)

	#if (USE_USER_COMMANDS == 1)
		Result = InitNexShellCommands();

		// did it work?
		if (Result != SHELL_SUCCESS)
			return Result;
	#endif // end of #if (USE_USER_COMMANDS == 1)

	// initialize our input and output streams
	if (CreatePipe(&gStandardOutputStream, gOutputStreamBuffer, SIZE_OF_OUTPUT_STREAM_BUFFER_IN_BYTES) == NULL)
		return SHELL_GENERIC_BUFFER_CREATE_FAILURE;

	if (CreatePipe(&gStandardInputStream, gInputStreamBuffer, SIZE_OF_INPUT_STREAM_BUFFER_IN_BYTES) == NULL)
		return SHELL_GENERIC_BUFFER_CREATE_FAILURE;

	if (CreatePipe(&gShellOperatorStream, gShellOperatorBuffer, SIZE_OF_SHELL_OPERATOR_STREAM_BUFFER_IN_BYTES) == NULL)
		return SHELL_GENERIC_BUFFER_CREATE_FAILURE;

	if (isalpha((int)CurrentDrive) == 0)
		return SHELL_INVALID_INPUT_PARAMETER;

	// get the working drive
	gCurrentWorkingDirectory[0] = CurrentDrive;
	gCurrentWorkingDirectory[1] = ':';
	gCurrentWorkingDirectory[2] = '\\';
	gCurrentWorkingDirectory[3] = 0;

	// copy the root directory drive letter
	gRootDirectoryDrive = CurrentDrive;

	#if (USE_SHELL_COMMAND_HISTORY == 1)
		if (CreateLinkedList(&gHistoryList, NULL, NexShellFreeMethod) == NULL)
			return SHELL_LINKED_LIST_CREATE_FAILURE;

		gHistoryIndex = 0;
	#endif // end of #if (USE_SHELL_COMMAND_HISTORY == 1)

	// now initialize our console echo
	#if (SHELL_USE_CONSOLE_ECHO == RUNTIME_CONFIGURABLE)
		gConsoleEcho = DEFAULT_CONSOLE_ECHO_VALUE;
	#endif // end of #if (SHELL_USE_CONSOLE_ECHO == RUNTIME_CONFIGURABLE)

	// now output the starting string, then the prompt
	#if (SHELL_USE_PROJECT_NAME == 1)
		#ifdef SHELL_PROJECT_NAME
			if (PipeWrite(&gStandardOutputStream, SHELL_PROJECT_NAME_TEXT_COLOR SHELL_PROJECT_NAME, (UINT32)strlen(SHELL_PROJECT_NAME_TEXT_COLOR SHELL_PROJECT_NAME), NULL) != OS_SUCCESS)
				return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

			if (PipeWrite(&gStandardOutputStream, " ", 1, NULL) != OS_SUCCESS)
				return SHELL_GENERIC_BUFFER_WRITE_FAILURE;
		#endif // end of SHELL_PROJECT_NAME

		#if (USE_SHELL_COLOR == 1)
			if (PipeWrite(&gStandardOutputStream, SHELL_DEFAULT_TEXT_COLOR, (UINT32)strlen(SHELL_DEFAULT_TEXT_COLOR), NULL) != OS_SUCCESS)
				return SHELL_GENERIC_BUFFER_WRITE_FAILURE;
		#endif // end of #if (USE_SHELL_COLOR == 1)

		#ifdef SHELL_MAJOR_VERSION
			if (PipeWrite(&gStandardOutputStream, SHELL_MAJOR_VERSION, (UINT32)strlen(SHELL_MAJOR_VERSION), NULL) != OS_SUCCESS)
				return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

			if (PipeWrite(&gStandardOutputStream, (const BYTE*)".", 1, NULL) != OS_SUCCESS)
				return SHELL_GENERIC_BUFFER_WRITE_FAILURE;
		#endif // end of SHELL_MAJOR_VERSION

		#ifdef SHELL_MINOR_VERSION
			if (PipeWrite(&gStandardOutputStream, SHELL_MINOR_VERSION, (UINT32)strlen(SHELL_MINOR_VERSION), NULL) != OS_SUCCESS)
				return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

			if (PipeWrite(&gStandardOutputStream, (const BYTE*)".", 1, NULL) != OS_SUCCESS)
				return SHELL_GENERIC_BUFFER_WRITE_FAILURE;
		#endif // end of SHELL_MINOR_VERSION

		#ifdef SHELL_TEST_VERSION
			if (PipeWrite(&gStandardOutputStream, SHELL_TEST_VERSION, (UINT32)strlen(SHELL_TEST_VERSION), NULL) != OS_SUCCESS)
				return SHELL_GENERIC_BUFFER_WRITE_FAILURE;
		#endif // end of SHELL_TEST_VERSION

		if (PipeWrite(&gStandardOutputStream, SHELL_DEFAULT_END_OF_LINE_SEQUENCE, SHELL_END_OF_LINE_SEQUENCE_SIZE_IN_BYTES, NULL) != OS_SUCCESS)
			return SHELL_GENERIC_BUFFER_WRITE_FAILURE;
	#endif // end of #if (SHELL_USE_PROJECT_NAME == 1)

	// now initialize our file system according to the drive they gave us
	if ((SHELL_RESULT)f_mount(&gFatFs, gCurrentWorkingDirectory, 1) != SHELL_SUCCESS)
	{
		// we're not dead in the water, attempt to initialize the disk then
		char TempDrivePath[3];
		BYTE TempWorkingBuffer[512];

		// construct the drive path
		TempDrivePath[0] = CurrentDrive;
		TempDrivePath[1] = ':';
		TempDrivePath[2] = 0;

		Result = f_mkfs(TempDrivePath, NULL, TempWorkingBuffer, sizeof(TempWorkingBuffer));

		// did it work?
		if (Result != SHELL_SUCCESS)
			return Result;

		// now try and mount the filesystem
		Result = (SHELL_RESULT)f_mount(&gFatFs, gCurrentWorkingDirectory, 1);

		// did it work?
		if (Result != SHELL_SUCCESS)
			return Result;

		// we have a valid file system now
		Shell_sprintf(gCurrentWorkingDirectory, "%c:/" DEV_FOLDER_NAME, CurrentDrive);

		// now create the first folder which is dev, this is virtual
		Result = f_mkdir(gCurrentWorkingDirectory);

		// did it work?
		if (Result != SHELL_SUCCESS)
			return Result;
	}

	// get this back for use with the file system
	gCurrentWorkingDirectory[0] = CurrentDrive;
	gCurrentWorkingDirectory[1] = ':';
	gCurrentWorkingDirectory[2] = '/';
	gCurrentWorkingDirectory[3] = 0;

	#if (EXTENDED_CD_SUPPORT == 1)
		Result = cdInit();

		// did it work?
		if (Result != SHELL_SUCCESS)
			return Result;
	#endif // end of #if (EXTENDED_CD_SUPPORT == 1)

	// now output the prompt
	return OutputPrompt(GetLastDirectoryPresent(&gCurrentWorkingDirectory[2]), &gStandardOutputStream);
}

SHELL_RESULT NexShellWriteTasks(PIPE *OutputStream)
{
	if (PipeGetSize(OutputStream) != 0)
	{
		BYTE CharacterBuffer[SHELL_HAL_MAX_TRANSFER_SIZE_IN_BYTES + 1];
		UINT32 BytesToTransfer, OriginalBytesToTransfer;

		if(PipePeek(OutputStream, CharacterBuffer, SHELL_HAL_MAX_TRANSFER_SIZE_IN_BYTES, SHELL_HAL_MAX_TRANSFER_SIZE_IN_BYTES, &OriginalBytesToTransfer) != OS_SUCCESS)
			return SHELL_GENERIC_BUFFER_READ_FAILURE;

		// now null it out for the method that is about to send it
		CharacterBuffer[OriginalBytesToTransfer] = 0;

		// now get the data from the user if ther is any
		if ((BytesToTransfer = StreamWriteDataHALCallback(CharacterBuffer, OriginalBytesToTransfer)) != 0)
		{
			// did they write a valid amount?
			if (BytesToTransfer > SHELL_HAL_MAX_TRANSFER_SIZE_IN_BYTES || BytesToTransfer > OriginalBytesToTransfer)
			{
				return SHELL_INVALID_NUMBER_OF_BYTES_TRANSFERRED;
			}

			// some or all was written, now read what thye wrote out of the buffer since it was a peek prior
			if (PipeRead(OutputStream, CharacterBuffer, SHELL_HAL_MAX_TRANSFER_SIZE_IN_BYTES, BytesToTransfer, NULL) != OS_SUCCESS)
			{
				return SHELL_GENERIC_BUFFER_READ_FAILURE;
			}
		}
	}

	return SHELL_SUCCESS;
}

static char* ParseArgument(char** Buffer)
{
	char* StartOfString;
	BOOL SkipSpaces = FALSE;

	// go through and skip white space at the beginning
	while (isspace(**Buffer) != FALSE)
		(*Buffer)++;

	// now we're pointing to something
	// is it null?
	if (**Buffer == 0)
		return NULL;

	// point to the beginning
	StartOfString = *Buffer;

	// so we have a valid string
	// start moving across it, and don't ignore ""
	if (**Buffer == '"')
	{
		// iterate beyond the "
		StartOfString++;
		(*Buffer)++;

		// keep going until we find " or nothing
		while (**Buffer != '"')
		{
			// if we found null in quotes, thats a problem
			if (**Buffer == 0)
				return NULL;

			(*Buffer)++;
		}

		// so now we're pointing to "
		**Buffer = 0;

		// iterate beyond the " to the next portion of the string
		(*Buffer)++;
	}
	else
	{
		if (**Buffer == '\'')
		{
			// iterate beyond the '
			StartOfString++;
			(*Buffer)++;

			// keep going until we find ' or nothing
			while (**Buffer != '\'')
			{
				// if we found null in quotes, thats a problem
				if (**Buffer == 0)
					return NULL;

				// can't have quotes in this mode
				if (**Buffer == '\"')
					return NULL;

				// cant have backslash in this mode
				if (**Buffer == '\\')
					return NULL;

				(*Buffer)++;
			}

			// so now we're pointing to '
			**Buffer = 0;

			// iterate beyond the ' to the next portion of the string
			(*Buffer)++;
		}
		else
		{
			// go through and skip all non white space
			while (isspace(**Buffer) == FALSE && **Buffer != 0)
				(*Buffer)++;

			// if it wasn't the end of the string, null it out and advance
			if (**Buffer != 0)
			{
				// so now we're pointing to some space or NULL
				**Buffer = 0;

				(*Buffer)++;
			}
		}
	}

	return StartOfString;
}

static BOOL UserCommandIsValid(char* Buffer)
{
	if ((BOOL)isprint((int)*Buffer) == FALSE)
		return FALSE;

	do
	{
		// if it is not printable, 0 is valid, otherwise theres an issue
		if ((BOOL)isprint((int)*Buffer) == FALSE)
		{
			if (*Buffer == 0)
				return TRUE;
			else
				return FALSE;
		}

		if (*Buffer == '"')
		{
			// skip ahead until the next \ or "
			while (1)
			{
				// we can't end with a null, we have to quote out then null
				if ((BOOL)isprint((int)*Buffer) == FALSE && *Buffer != '\n' && *Buffer != '\r')
					return FALSE;

				if (*Buffer == '\\')
				{
					// ignore the next character unless not printable

					Buffer++;

					if ((BOOL)isprint((int)*Buffer) == FALSE)
						return FALSE;
				}
				else
				{
					// are we quoting out?
					if (*Buffer == '\"')
					{
						break;
					}
				}

				// go to the next character
				Buffer++;
			}
		}

		if (*Buffer == '\'')
		{
			// skip ahead until the next \ or "
			while (1)
			{
				// we can't end with a null, we have to quote out then null
				if ((BOOL)isprint((int)*Buffer) == FALSE && *Buffer != '\n' && *Buffer != '\r')
					return FALSE;

				// are we quoting out?
				if (*Buffer == '\'')
				{
					break;
				}

				// go to the next character
				Buffer++;
			}
		}

		Buffer++;
	} 
	while (1);
}

static SHELL_LOGICAL_OPERATOR GetShellExpression(char** Buffer)
{
	// scan the buffer for a logical operator
	while (**Buffer)
	{
		switch (**Buffer)
		{
			case '|':
			{
				// we found a pipe operator
				**Buffer = 0;

				return SHELL_PIPE;
			}

			case '>':
			{
				**Buffer = 0;

				(*Buffer)++;

				if ((BOOL)isprint(**Buffer) == FALSE)
					return NUMBER_OF_SHELL_LOGICAL_OPERATORS;

				if (**Buffer == '>')
					return SHELL_APPEND;

				return SHELL_WRITE;
			}

			case '"':
			{
				// keep going unil "
				do
				{
					(*Buffer)++;

					// if we got a quote, leave
					if (**Buffer == '\"')
					{
						(*Buffer)++;

						break;
					}

					// if we got a backslash, get the next item
					if (**Buffer == '\\')
					{
						(*Buffer)++;

						if ((BOOL)isprint(**Buffer) == FALSE)
							return NUMBER_OF_SHELL_LOGICAL_OPERATORS;

					}
				} 
				while (1);
			
				break;
			}

			case '\'':
			{
				// keep going unil '"'
				do
				{
					(*Buffer)++;

					// if we got a quote, leave
					if (**Buffer == 0)
					{
						return NUMBER_OF_SHELL_LOGICAL_OPERATORS;
					}

					// if we got a quote, leave
					if (**Buffer == '\'')
					{
						(*Buffer)++;

						break;
					}

					// if we got a backslash, get the next item
					if (**Buffer == '\\')
					{
						(*Buffer)++;

						if ((BOOL)isprint(**Buffer) == FALSE)
							return NUMBER_OF_SHELL_LOGICAL_OPERATORS;

					}
				}
				while (1);
			}

			default:
			{
				(*Buffer)++;

				break;
			}
		}
	}

	return SHELL_NO_LOGICAL_OPERATOR;
}

static SHELL_RESULT NexShellProcessCommand(char* Buffer, PIPE *OutputStream)
{
	UINT32 argc;
	UINT32 WorkingIndex;
	PIPE* StreamPtr;

	// we add 2 for the potential location and command name
	// this way the command/file can get a full SHELL_WORKING_ARGUMENTS_ARRAY_SIZE_IN_ELEMENTS
	char *argv[SHELL_WORKING_ARGUMENTS_FULL_ARRAY_SIZE_IN_ELEMENTS];

	// is the input string even valid?
	if (UserCommandIsValid(Buffer) == FALSE)
		return SHELL_INVALID_INPUT;

	// now truncate the expression
	do
	{
		char* TempBuffer = Buffer;

		SHELL_LOGICAL_OPERATOR Operator = GetShellExpression(&Buffer);

		// was it valid
		if (Operator == NUMBER_OF_SHELL_LOGICAL_OPERATORS)
			return SHELL_INVALID_INPUT;

		// is it going to the standard stream?
		if (Operator != SHELL_NO_LOGICAL_OPERATOR)
			StreamPtr = &gShellOperatorStream;
		else
			StreamPtr = OutputStream;

		// now we can perform the operation after getting the args

		// get the first arguments
		for (argc = 0; argc < SHELL_WORKING_ARGUMENTS_FULL_ARRAY_SIZE_IN_ELEMENTS; argc++)
		{
			// get the next argument from the user
			argv[argc] = ParseArgument(&TempBuffer);

			// we're done if it is NULL
			if (argv[argc] == NULL)
				break;
		}

		// check for overflow
		if (argc == SHELL_WORKING_ARGUMENTS_FULL_ARRAY_SIZE_IN_ELEMENTS)
			if (ParseArgument(&TempBuffer) != NULL)
				return SHELL_ARGUMENT_OVERFLOW;

		// did we get any arguemnts?
		if (argc == 0)
			return SHELL_SUCCESS;

		// now get the command
		{
			UINT32 i;

			WorkingIndex = 0;

			for (i = 0; i < GetNumberOfNextShellNativeCommands(); i++)
			{
				// compare it to a file
				if (strcmp(GetNativeCommandInfo(i)->CommandName, argv[0]) == 0)
				{
					// we found a winner
					return GetNativeCommandInfo(i)->ExecuteFile(&argv[1], argc - 1, StreamPtr);
				}
			}

			return SHELL_FILE_NOT_FOUND;
		}
	} 
	while (*Buffer);

	return SHELL_SUCCESS;
}

#if (USE_SHELL_COMMAND_HISTORY == 1)
	static BOOL AddLineToHistory(char* Line)
	{
		char* HistoryLine;

		// if it is an empty string, just leave
		// don't add it to the history
		if (strlen(Line) == 0)
			return TRUE;

		// clear out our history
		gHistoryIndex = 0;

		// if we're at max size, delete the oldest node
		if (LinkedListGetSize(&gHistoryList) == NUMBER_OF_ENTRIES_IN_HISTORY)
			if (LinkedListDeleteNode(&gHistoryList, 1) != TRUE)
				return FALSE;

		// get the space
		HistoryLine = NexShellMalloc(strlen(Line) + 1);

		// did we get the space?
		if (HistoryLine == NULL)
			return FALSE;

		// now add the string to the linked list

		// copy the data in
		strcpy(HistoryLine, Line);

		return LinkedListAdd(&gHistoryList, HistoryLine);
	}

	SHELL_RESULT UpdateHistory(BOOL Increment, PIPE *InputStream, PIPE *OutputStream)
	{
		UINT32 i;
		char* DataFromHistoryBuffer;

		// Increment our index
		if (Increment == TRUE)
		{
			if (gHistoryIndex == 0)
			{
				gHistoryIndex = LinkedListGetSize(&gHistoryList);
			}
			else
			{
				gHistoryIndex--;

				if (gHistoryIndex == 0)
					gHistoryIndex = 1;
			}
		}
		else
		{
			// if we're at the size, disable us
			if (gHistoryIndex == LinkedListGetSize(&gHistoryList))
			{
				gHistoryIndex = 0;
			}
			else
			{
				if (gHistoryIndex != 0)
				{
					gHistoryIndex++;
				}
				else
				{
					return SHELL_SUCCESS;
				}
			}
		}

		// shift over to the beginning of the prompt
		for (i = 0; i < PipeGetSize(InputStream); i++)
		{
			if (PipeWrite(OutputStream, SHELL_MOVE_CURSOR_LEFT_COMMAND, (UINT32)strlen(SHELL_MOVE_CURSOR_LEFT_COMMAND), NULL) != OS_SUCCESS)
				return SHELL_GENERIC_BUFFER_WRITE_FAILURE;
		}

		// now clear out the line
		if (PipeWrite(OutputStream, SHELL_CLEAR_REMAINING_LINE_COMMAND, (UINT32)strlen(SHELL_CLEAR_REMAINING_LINE_COMMAND), NULL) != OS_SUCCESS)
			return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

		// always clear this out
		if (PipeFlush(InputStream) != OS_SUCCESS)
			return SHELL_LINKED_LIST_OPERATION_FAILURE;

		// now read the data from the list
		DataFromHistoryBuffer = LinkedListGetData(&gHistoryList, gHistoryIndex);

		if (DataFromHistoryBuffer != NULL)
		{
			if (PipeWrite(OutputStream, DataFromHistoryBuffer, (UINT32)strlen(DataFromHistoryBuffer), NULL) != OS_SUCCESS)
				return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

			if(PipeWrite(InputStream, DataFromHistoryBuffer, (UINT32)strlen(DataFromHistoryBuffer), NULL) != OS_SUCCESS)
				return SHELL_GENERIC_BUFFER_WRITE_FAILURE;
		}

		return SHELL_SUCCESS;
	}
#endif // end of #if (USE_SHELL_COMMAND_HISTORY == 1)

static SHELL_RESULT NexShellProcessIncomingBuffer(char *IncomingData, UINT32 NumberOfBytes, PIPE *InputStream, PIPE *OutputStream)
{
	while (NumberOfBytes != 0)
	{
		switch(gEscapeSequence)
		{
			case 0:
			{
				switch (*IncomingData)
				{
					case BACKSPACE_ASCII_VALUE:
					{
						if (PipeRemoveLastByteWritten(InputStream, NULL) == OS_SUCCESS)
						{
							// if echo is on, output the data
							#if (SHELL_USE_CONSOLE_ECHO == RUNTIME_CONFIGURABLE)
								if (gConsoleEcho == TRUE)
								{
									if (PipeWrite(&gOutputStream, SHELL_MOVE_CURSOR_LEFT_COMMAND, (UINT32)strlen(SHELL_MOVE_CURSOR_LEFT_COMMAND), NULL) != OS_SUCCESS)
										return SHELL_OUTPUT_GENERIC_BUFFER_OVERFLOW;

									if (PipeWrite(&gOutputStream, SHELL_CLEAR_REMAINING_LINE_COMMAND, (UINT32)strlen(SHELL_CLEAR_REMAINING_LINE_COMMAND), NULL) != OS_SUCCESS)
										return SHELL_OUTPUT_GENERIC_BUFFER_OVERFLOW;
								}
							#else
								#if (SHELL_USE_CONSOLE_ECHO == ALWAYS_ON)
									// always output the data in this configuration
									if (PipeWrite(OutputStream, SHELL_DELETE_COMMAND, (UINT32)strlen(SHELL_DELETE_COMMAND), NULL) != OS_SUCCESS)
										return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

									if (PipeWrite(OutputStream, SHELL_CLEAR_REMAINING_LINE_COMMAND, (UINT32)strlen(SHELL_CLEAR_REMAINING_LINE_COMMAND), NULL) != OS_SUCCESS)
										return SHELL_GENERIC_BUFFER_WRITE_FAILURE;
								#endif // end of #if (SHELL_USE_CONSOLE_ECHO == ALWAYS_ON)
							#endif // end of #if (SHELL_USE_CONSOLE_ECHO == RUNTIME_CONFIGURABLE)
						}

						break;
					}

					case ESCAPE_ASCII_VALUE:
					{
						gEscapeSequence = 1;

						break;
					}

					default:
					{
						// we have data we can write to the input stream

						// write the data to the input stream
						if (PipeWrite(InputStream, IncomingData, 1, NULL) != OS_SUCCESS)
							return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

						// if echo is on, output the data
						#if (SHELL_USE_CONSOLE_ECHO == RUNTIME_CONFIGURABLE)
							if (gConsoleEcho == TRUE)
							{
								if (PipeWrite(&gOutputStream, IncomingData, 1, NULL) != OS_SUCCESS)
									return SHELL_OUTPUT_GENERIC_BUFFER_OVERFLOW;
							}
						#else
							#if (SHELL_USE_CONSOLE_ECHO == ALWAYS_ON)
								// always output the data in this configuration
								if (PipeWrite(OutputStream, IncomingData, 1, NULL) != OS_SUCCESS)
									return SHELL_GENERIC_BUFFER_WRITE_FAILURE;
							#endif // end of #if (SHELL_USE_CONSOLE_ECHO == ALWAYS_ON)
						#endif // end of #if (SHELL_USE_CONSOLE_ECHO == RUNTIME_CONFIGURABLE)

						break;
					}
				}

				break;
			}

			case 1:
			{
				// next sequence in the escape potentially
				if (*IncomingData == 0x5B || *IncomingData == 0x4F)
					gEscapeSequence++;
				else
					gEscapeSequence = 0;

				break;
			}

			case 2:
			{
				gEscapeSequence = 0;

				switch (*IncomingData)
				{
					#if (USE_SHELL_COMMAND_HISTORY == 1)
						case UP_ARROW_ASCII_VALUE:
						{
							return UpdateHistory(TRUE, InputStream, OutputStream);
						}

						case DOWN_ARROW_ASCII_VALUE:
						{
							return UpdateHistory(FALSE, InputStream, OutputStream);
						}

						case RIGHT_ARROW_ASCII_VALUE:
						{
							break;
						}

						case LEFT_ARROW_ASCII_VALUE:
						{
							break;
						}
					#endif // end of #if (USE_SHELL_COMMAND_HISTORY == 1)
				}

				break;
			}
		}

		// increment our pointer
		IncomingData++;

		// decrease our size
		NumberOfBytes--;
	}

	return SHELL_SUCCESS;
}

static SHELL_RESULT NexShellReadTasks(PIPE *InputStream, PIPE *OutputStream, char *CurrentWorkingDirectory)
{
	SHELL_RESULT Result;
	BYTE CharacterBuffer[SHELL_HAL_MAX_TRANSFER_SIZE_IN_BYTES + 1];
	UINT32 BytesToTransfer;

	// read whatever is available
	BytesToTransfer = StreamReaderDataHALCallback(CharacterBuffer, SHELL_HAL_MAX_TRANSFER_SIZE_IN_BYTES);

	if (BytesToTransfer != 0)
	{
		UINT32 NumberOfBytesToRead;

		if (BytesToTransfer > SHELL_HAL_MAX_TRANSFER_SIZE_IN_BYTES)
			return SHELL_INVALID_NUMBER_OF_BYTES_TRANSFERRED;

		// now write all the incoming data to our buffer
		Result = NexShellProcessIncomingBuffer(CharacterBuffer, BytesToTransfer, InputStream, OutputStream);

		if (Result != SHELL_SUCCESS)
			return Result;
	
		// did we get an end of line sequence?
		if (PipeContainsSequence(InputStream, SHELL_DEFAULT_END_OF_LINE_SEQUENCE, SHELL_END_OF_LINE_SEQUENCE_SIZE_IN_BYTES, &NumberOfBytesToRead) == OS_SUCCESS)
		{
			SHELL_RESULT Result;

			char TempBuffer[SIZE_OF_SHELL_STACK_BUFFER_IN_BYTES + 1];

			// add on the size of the sequence we are reading
			NumberOfBytesToRead += SHELL_END_OF_LINE_SEQUENCE_SIZE_IN_BYTES;

			// read the data out and process the command
			if (PipeRead(InputStream, TempBuffer, sizeof(TempBuffer), NumberOfBytesToRead, NULL) == OS_SUCCESS)
			{
				TempBuffer[NumberOfBytesToRead - SHELL_END_OF_LINE_SEQUENCE_SIZE_IN_BYTES] = 0;

				// is it just an empty line?
				if(strlen(TempBuffer) != 0)
				{
					SHELL_RESULT TempResult;

					#if (USE_SHELL_COMMAND_HISTORY == 1)
						if (AddLineToHistory(TempBuffer) == FALSE)
							return SHELL_HISTORY_BUFFER_FAILURE;
					#endif // end of #if (USE_SHELL_COMMAND_HISTORY == 1)

					// now go ahead and process the command
					Result = NexShellProcessCommand(TempBuffer, OutputStream);

					TempResult = f_chdir(gCurrentWorkingDirectory);

					// if we didn't get a main error, output it, otherwise output the result of f_chdir()
					if (Result == SHELL_SUCCESS)
						Result = TempResult;
				}
				else
				{
					Result = SHELL_SUCCESS;
				}
			}
			else
			{
				Result = SHELL_GENERIC_BUFFER_READ_FAILURE;
			}

			if (Result != SHELL_SUCCESS)
			{
				// to string the error and output it

				if (Result != SHELL_OPERATION_FAILED_OUTPUT_USER_BUFFER)
				{
					#if (USE_SHELL_COLOR == 1)
						if (PipeWrite(OutputStream, SHELL_ERROR_COLOR, (UINT32)strlen(SHELL_ERROR_COLOR), NULL) != OS_SUCCESS)
							return SHELL_GENERIC_BUFFER_WRITE_FAILURE;
					#endif // end of #if (USE_SHELL_COLOR == 1)

					if(PipeWrite(OutputStream, gNexShellError[Result], (UINT32)strlen(gNexShellError[Result]), NULL) != OS_SUCCESS)
						return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

					if (PipeWrite(OutputStream, SHELL_DEFAULT_END_OF_LINE_SEQUENCE, SHELL_END_OF_LINE_SEQUENCE_SIZE_IN_BYTES, NULL) != OS_SUCCESS)
						return SHELL_GENERIC_BUFFER_WRITE_FAILURE;
				}
			}

			// now output the prompt last
			return OutputPrompt(GetLastDirectoryPresent(CurrentWorkingDirectory), OutputStream);
		}
	}

	return SHELL_SUCCESS;
}

unsigned long UserNexShellWriteTasks(void)
{
	return NexShellWriteTasks(&gStandardOutputStream);
}

SHELL_RESULT NexShellTasks(void)
{
	//SHELL_RESULT WriteResult = NexShellWriteTasks(&gStandardOutputStream);
	SHELL_RESULT ReadResult = NexShellReadTasks(&gStandardInputStream, &gStandardOutputStream, gCurrentWorkingDirectory);

	return SHELL_SUCCESS;
}
