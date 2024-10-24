#ifndef NEXSHELL_H
	#define NEXSHELL_H

#include "NexShellConfig.h"
#include "GenericTypeDefs.h"
#include "GenericBuffer.h"
#include "ff.h"

typedef enum
{
	SHELL_FILE_SUCCESS = 0,

	SHELL_FILE_DISK_ERR,			/* (1) A hard error occurred in the low level disk I/O layer */
	SHELL_FILE_INT_ERR,				/* (2) Assertion failed */
	SHELL_FILE_NOT_READY,			/* (3) The physical drive cannot work */
	SHELL_FILE_NO_FILE,				/* (4) Could not find the file */
	SHELL_FILE_NO_PATH,				/* (5) Could not find the path */
	SHELL_FILE_INVALID_NAME,		/* (6) The path name format is invalid */
	SHELL_FILE_DENIED,				/* (7) Access denied due to prohibited access or directory full */
	SHELL_FILE_EXIST,				/* (8) Access denied due to prohibited access */
	SHELL_FILE_INVALID_OBJECT,		/* (9) The file/directory object is invalid */
	SHELL_FILE_WRITE_PROTECTED,		/* (10) The physical drive is write protected */
	SHELL_FILE_INVALID_DRIVE,		/* (11) The logical drive number is invalid */
	SHELL_FILE_NOT_ENABLED,			/* (12) The volume has no work area */
	SHELL_FILE_NO_FILESYSTEM,		/* (13) There is no valid FAT volume */
	SHELL_FILE_MKFS_ABORTED,		/* (14) The f_mkfs() aborted due to any problem */
	SHELL_FILE_TIMEOUT,				/* (15) Could not get a grant to access the volume within defined period */
	SHELL_FILE_LOCKED,				/* (16) The operation is rejected according to the file sharing policy */
	SHELL_FILE_NOT_ENOUGH_CORE,		/* (17) LFN working buffer could not be allocated */
	SHELL_FILE_TOO_MANY_OPEN_FILES,	/* (18) Number of open files > FF_FS_LOCK */
	SHELL_FILE_INVALID_PARAMETER,	/* (19) Given parameter is invalid */

	NUMBER_OF_NEXSHELL_FILE_RESULTS
}NEXSHELL_FILE_RESULTS;

typedef enum
{
	SHELL_SUCCESS = 0,
	SHELL_INVALID_INPUT_PARAMETER = NUMBER_OF_NEXSHELL_FILE_RESULTS,
	SHELL_INVALID_INPUT,
	SHELL_ARGUMENT_OVERFLOW,
	SHELL_INVALID_CHARACTER_FOUND,
	SHELL_FILE_NOT_FOUND,
	SHELL_FILE_NOT_EXECUTABLE,
	SHELL_FILE_NOT_READABLE,
	SHELL_FILE_NOT_WRITABLE,
	SHELL_HISTORY_BUFFER_FAILURE,
	SHELL_INVALID_NUMBER_OF_BYTES_TRANSFERRED,
	SHELL_GENERIC_BUFFER_CREATE_FAILURE,
	SHELL_GENERIC_BUFFER_WRITE_FAILURE,
	SHELL_GENERIC_BUFFER_READ_FAILURE,
	SHELL_LINKED_LIST_CREATE_FAILURE,
	SHELL_LINKED_LIST_OPERATION_FAILURE,

	// always put this one at the lowest of the valid list
	SHELL_OPERATION_FAILED_OUTPUT_USER_BUFFER,

	NUMBER_OF_NEXSHELL_RESULTS
}SHELL_RESULT;

typedef enum
{
	SHELL_NO_LOGICAL_OPERATOR = 0,
	SHELL_PIPE,
	SHELL_WRITE,
	SHELL_APPEND,
	NUMBER_OF_SHELL_LOGICAL_OPERATORS
}SHELL_LOGICAL_OPERATOR;

typedef enum
{
	SHELL_FILE_DISK = 0,
	SHELL_FILE_VIRTUAL,
	NUMBER_OF_SHELL_FILES
}SHELL_FILE_TYPE;

typedef struct
{
	SHELL_RESULT(*ReadFileData)(GENERIC_BUFFER* OutputStream);
	SHELL_RESULT(*WriteFileData)(char* Args[], UINT32 NumberOfArgs, GENERIC_BUFFER* OutputStream);
	SHELL_RESULT(*ExecuteFile)(char* Args[], UINT32 NumberOfArgs, GENERIC_BUFFER* OutputStream);

	char* FileName;

	#if (USE_FILE_DESCRIPTION == 1)
		char* FileDescription;
	#endif // end of #if (USE_FILE_DESCRIPTION == 1)

	#if (USE_FILE_HELP == 1)
		char* FileHelp;
	#endif // end of #if (USE_FILE_HELP == 1)
}VIRTUAL_FILE;

typedef struct
{
	SHELL_FILE_TYPE FileType;
	void *File; // this points to a FIL or a VIRTUAL_FILE
}SHELL_FILE;

#define SHELL_WORKING_ARGUMENTS_FULL_ARRAY_SIZE_IN_ELEMENTS		(SHELL_WORKING_ARGUMENTS_ARRAY_SIZE_IN_ELEMENTS + 2)

#define SHELL_FONT_STYLE_RESET									"\x1B[0m"
#define SHELL_FONT_STYLE_BOLD									"\x1B[1m"
#define SHELL_FONT_STYLE_DISABLED								"\x1B[2m"
#define SHELL_FONT_STYLE_ITALIC									"\x1B[3m"
#define SHELL_FONT_STYLE_UNDERSCORE								"\x1B[4m"

#define SHELL_FONT_COLOR_RED									"\x1B[31m"
#define SHELL_FONT_COLOR_GREEN									"\x1B[32m"
#define SHELL_FONT_COLOR_YELLOW									"\x1B[33m"
#define SHELL_FONT_COLOR_BLUE									"\x1B[34m"
#define SHELL_FONT_COLOR_MAGENTA								"\x1B[35m"
#define SHELL_FONT_COLOR_CYAN									"\x1B[36m"
#define SHELL_FONT_COLOR_WHITE									"\x1B[37m"

#define SHELL_FONT_COLOR_BRIGHT_RED								"\x1B[91m"
#define SHELL_FONT_COLOR_BRIGHT_GREEN							"\x1B[92m"
#define SHELL_FONT_COLOR_BRIGHT_YELLOW							"\x1B[93m"
#define SHELL_FONT_COLOR_BRIGHT_BLUE							"\x1B[94m"
#define SHELL_FONT_COLOR_BRIGHT_MAGENTA							"\x1B[95m"
#define SHELL_FONT_COLOR_BRIGHT_CYAN							"\x1B[96m"
#define SHELL_FONT_COLOR_BRIGHT_WHITE							"\x1B[97m"

#define SHELL_DELETE_COMMAND									"\x1B[1D"
#define SHELL_MOVE_CURSOR_LEFT_COMMAND							"\x1B[1D"
#define SHELL_CLEAR_SCREEN_COMMAND								"\x1B[2J"
#define SHELL_HOME_CURSOR_COMMAND								"\x1B[0;0f"
#define SHELL_CLEAR_REMAINING_LINE_COMMAND						"\x1B[0K"

#define SHELL_RELATIVE_DIRECTORY_SEQUENCE						("." SHELL_DEFAULT_END_OF_LINE_SEQUENCE ".." SHELL_DEFAULT_END_OF_LINE_SEQUENCE)

#define DEV_FOLDER_NAME											"dev"

#define CAT_COMMAND_NAME										"cat"
#define CD_COMMAND_NAME											"cd"
#define CLEAR_COMMAND_NAME										"clear"
#define ECHO_COMMAND_NAME										"echo"
#define HELP_COMMAND_NAME										"help"
#define HEXDUMP_COMMAND_NAME									"hexdump"
#define LS_COMMAND_NAME											"ls"
#define PEEK_COMMAND_NAME										"peek"
#define POKE_COMMAND_NAME										"poke"
#define PWD_COMMAND_NAME										"pwd"
#define SHUTDOWN_COMMAND_NAME									"shutdown"

// this is the ascii value for the backspace character
#define BACKSPACE_ASCII_VALUE									0x08
#define ESCAPE_ASCII_VALUE										0x1B
#define UP_ARROW_ASCII_VALUE									41
#define DOWN_ARROW_ASCII_VALUE									42
#define RIGHT_ARROW_ASCII_VALUE									43
#define LEFT_ARROW_ASCII_VALUE									44

#define NexShellAssert(condition)								{if(!(condition)){while(1);}}

typedef struct
{
	char* CommandName;
	SHELL_RESULT(*ExecuteFile)(char* Args[], UINT32 NumberOfArgs, GENERIC_BUFFER* Buffer);


	char* Description;

	#if (USE_COMMAND_HELP == 1)
		char* Help;
	#endif // #if (USE_COMMAND_HELP == 1)
}COMMAND_INFO;

SHELL_RESULT NexShellInit(char CurrentDrive);

// call this continuously
SHELL_RESULT NexShellTasks(void);

#endif // end of NEXSHELL_H