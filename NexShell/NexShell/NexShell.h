#ifndef NEXSHELL_H
	#define NEXSHELL_H

/*! \mainpage NexShell
 *  \brief A Unix type shell interface with the <a href="../fatfs-documents/00index_e.html">FatFs</a> 
 *  \author brodie
 *  \version 0.00.01
 *  \date   November 18, 2024
 */

 /** \file
  * This is the main interface to the NexShell.
  * This file contains all the methods to initialize and run the NexShell.
  */

#include "NexShellConfig.h"
#include "GenericTypes.h"
#include "Pipe.h"
#include "ff.h"

  /*!
  * \enum NEXSHELL_FILE_RESULTS
  * This is a renamed enum from the FatFs FRESULT enum.
  */
typedef enum
{
	SHELL_FILE_SUCCESS = 0,

	/*! (1) A hard error occurred in the low level disk I/O layer */
	SHELL_FILE_DISK_ERR,

	/*! (2) Assertion failed */
	SHELL_FILE_INT_ERR,	

	/*! (3) The physical drive cannot work */
	SHELL_FILE_NOT_READY,

	/*! (4) Could not find the file */
	SHELL_FILE_NO_FILE,	

	/*! (5) Could not find the path */
	SHELL_FILE_NO_PATH,			

	/*! (6) The path name format is invalid */
	SHELL_FILE_INVALID_NAME,		

	/*! (7) Access denied due to prohibited access or directory full */
	SHELL_FILE_DENIED,			

	/*! (8) Access denied due to prohibited access */
	SHELL_FILE_EXIST,	

	/*! (9) The file/directory object is invalid */
	SHELL_FILE_INVALID_OBJECT,	

	/*! (10) The physical drive is write protected */
	SHELL_FILE_WRITE_PROTECTED,

	/*! (11) The logical drive number is invalid */
	SHELL_FILE_INVALID_DRIVE,	

	/*! (12) The volume has no work area */
	SHELL_FILE_NOT_ENABLED,		
	
	/*! (13) There is no valid FAT volume */
	SHELL_FILE_NO_FILESYSTEM,		

	/*! (14) The f_mkfs() aborted due to any problem */
	SHELL_FILE_MKFS_ABORTED,	

	/*! (15) Could not get a grant to access the volume within defined period */
	SHELL_FILE_TIMEOUT,	

	/*! (16) The operation is rejected according to the file sharing policy */
	SHELL_FILE_LOCKED,		

	/*! (17) LFN working buffer could not be allocated */
	SHELL_FILE_NOT_ENOUGH_CORE,		

	/*! (18) Number of open files > FF_FS_LOCK */
	SHELL_FILE_TOO_MANY_OPEN_FILES,

	/*! (19) Given parameter is invalid */
	SHELL_FILE_INVALID_PARAMETER,	

	NUMBER_OF_NEXSHELL_FILE_RESULTS
}NEXSHELL_FILE_RESULTS;

/*!
* \enum SHELL_RESULT
* This is the main return type of the NexShell.  It contains all error values of an opertion.
*/
typedef enum
{	/*! This is returned upon success of the requested operation */
	SHELL_SUCCESS = 0,								

	/*! This is an offset based off of SHELL_FILE_DISK_ERR in the NEXSHELL_FILE_RESULTS enum */
	SHELL_INVALID_INPUT_PARAMETER = NUMBER_OF_NEXSHELL_FILE_RESULTS, 

	/*! The shell command got an invalid input parameter */
	SHELL_INVALID_INPUT,							

	/*! The shell command got too many arguments for the operation */
	SHELL_ARGUMENT_OVERFLOW,						

	/*! The shell found an invalid character in the stream */
	SHELL_INVALID_CHARACTER_FOUND,					

	/*! The shell command did not get enough arguments for the operation */
	SHELL_INSUFFICIENT_ARGUMENTS_FOR_FILE,			

	/*! The shell command got an invalid argument */
	SHELL_INVALID_ARGUMENT,							

	/*! The shell could not find the requested file */
	SHELL_FILE_NOT_FOUND,							

	/*! Attempted execution of a non executable file */
	SHELL_FILE_NOT_EXECUTABLE,						

	/*! Attempted read of a non readable file */
	SHELL_FILE_NOT_READABLE,						

	/*! Attempted write of a non writable file */
	SHELL_FILE_NOT_WRITABLE,						

	/*! Failed interaction with the shell history feature */
	SHELL_HISTORY_BUFFER_FAILURE,					

	/*! The HAL callback method for the shell returned an invalid number of bytes */
	SHELL_INVALID_NUMBER_OF_BYTES_TRANSFERRED,		

	/*! A \ref GENERIC_BUFFER create operation failed */
	SHELL_GENERIC_BUFFER_CREATE_FAILURE,			

	/*! A \ref GENERIC_BUFFER write operation failed */
	SHELL_GENERIC_BUFFER_WRITE_FAILURE,				

	/*! A \ref GENERIC_BUFFER read operation failed */
	SHELL_GENERIC_BUFFER_READ_FAILURE,				

	/*! A \ref LINKED_LIST create operation failed */
	SHELL_LINKED_LIST_CREATE_FAILURE,				

	/*! A \ref LINKED_LIST read, write, etc. operation failed */
	SHELL_LINKED_LIST_OPERATION_FAILURE,			

	/*! Dynamic memory allocation failed */
	SHELL_MALLOC_FAILURE,							

	/*! An invalid directory name was specified */
	SHELL_INVALID_DIRECTORY_NAME,					

	/*! The requested virtual directory alredy exists */
	SHELL_VIRTUAL_DIRECTORY_NAME_ALREADY_EXISTS,	

	/*! The requested virtual file already exists */
	SHELL_VIRTUAL_FILENAME_ALREADY_EXISTS,			

	/*! The requested shell command already exists */
	SHELL_COMMAND_ALREADY_EXISTS,					

	/*! The requested shell command does not exist */
	SHELL_COMMAND_NOT_FOUND,						

	/*! The ioctl() operation failed */
	SHELL_IO_CTL_FAILED,							

	/*! An invalid option was specified in a shell command */
	SHELL_INVALID_OPTION,							

	/*! A shell command was expecting arguments but didn't get them */
	SHELL_MISSING_OPERANDS,							

	/*! The requested directory could not be created */
	SHELL_CANNOT_CREATE_DIRECTORY,					

	// always put this one at the lowest of the valid list
	/*! An error happened that was non standard and the user buffer contains the error message */
	SHELL_OPERATION_FAILED_OUTPUT_USER_BUFFER,		

	/*! Used internally by the shell */
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
#define DATE_COMMAND_NAME										"date"
#define ECHO_COMMAND_NAME										"echo"
#define HELP_COMMAND_NAME										"help"
#define HEXDUMP_COMMAND_NAME									"hexdump"
#define LS_COMMAND_NAME											"ls"
#define MKDIR_COMMAND_NAME										"mkdir"
#define PEEK_COMMAND_NAME										"peek"
#define POKE_COMMAND_NAME										"poke"
#define PWD_COMMAND_NAME										"pwd"
#define SHUTDOWN_COMMAND_NAME									"shutdown"

// this is the ascii value for the backspace character
#define BACKSPACE_ASCII_VALUE									0x08
#define ESCAPE_ASCII_VALUE										0x1B
#define CTRL_PLUS_L_VALUE										12
#define UP_ARROW_ASCII_VALUE									41
#define DOWN_ARROW_ASCII_VALUE									42
#define RIGHT_ARROW_ASCII_VALUE									43
#define LEFT_ARROW_ASCII_VALUE									44

// despite it being 1 char, it needs to have double quotes
#define END_OF_LINE_CAT_PRINT_CHARACTER							"$"

#define NexShellAssert(condition)								{if(!(condition)){while(1);}}

/*!
* @struct COMMAND_INFO
*
* @brief This struct holds all the information for global commands in the shell.
*
* @param char *CommandName - The name of the command.  This is something like "ls".
*
* @param ExecuteFile - The method to execute when the command is called.
* 
* @param char *Description - A short optional description of what the command does.  This 
* is printed out with the help -d option or the ls command.
* 
* @param char *Help - The help file associated with the \ref COMMAND_INFO.  This is to give the user help when typing in
* "help [command name]".  This parameter is optional.  A file on disk can be referenced by this parameter to show
* the contents of.  The system will first try to interpret this string as a valid file (with full path).  If the file exists
* the contents of the file will be shown when the "help" command is used, otherwise the contents pointed to by Help
* are displayed for help.
* 
* @details
* This data structure is used to store the needed information on a global command.  There is a single
* \ref LINKED_LIST that holds all the user global commands, and an array that holds the system
* native global commands.
*/
typedef struct
{
	char* CommandName;
	SHELL_RESULT(*ExecuteFile)(char* Args[], UINT32 NumberOfArgs, PIPE* Buffer);
	char* Description;
	char* Help;
}COMMAND_INFO;

/*!
 * @brief Initializes the NexShell.
 *
 * This function will initialize all data structures needed to run the NexShell.
 * This method should be called before any other method in the NexShell library.
 *
 * @param[in] BYTE CurrentDrive - The main drive to initialize.  This is the disk drive that the NexShell will work out of.
 *
 * @return SHELL_RESULT - The outcome of the operation.
 *
 * @details
 * This function initializes everything in the NexShell.  It should be called before any other method in the NexShell.
 * The drive will be initialized through the FatFs.  If no valid file system is found this method will attempt to create one
 * with a call to \ref f_mkfs() on the root.  This function initializes all standard streams too.  Afterwards the methods 
 * \ref NexShellReadTasks() and NexShellWriteTasks() should be called to run the shell.  These can be called from the same thread,
 * but the size of data writing out cannot exceed \ref SIZE_OF_OUTPUT_STREAM_BUFFER_IN_BYTES found in NexShellConfig.h.
 * If \ref NexShellReadTasks() and \ref NexShellWriteTasks() are called from different threads then the data written to the standard
 * stream can exceed \ref SIZE_OF_OUTPUT_STREAM_BUFFER_IN_BYTES.
 * 
 * @sa NexShellUserReadTasks(), NexShellUserWriteTasks()
 */
SHELL_RESULT NexShellInit(char CurrentDrive);

/*!
 * @brief Updates the input stream of the NexShell.
 *
 * This function must be called periodically to update the standard input stream of the NexShell.  This method is thread safe.
 *
 * @return SHELL_RESULT - The outcome of the operation.
 *
 * @details
 * This function updates the input stream of the NexShell and will execute any commands sent to the NexShell.
 * The function \ref StreamReaderDataHALCallback() is called from this method and will update the standard input
 * stream with the data returned.
 *
 * @sa NexShellInit(), NexShellUserWriteTasks(), StreamReaderDataHALCallback()
 */
SHELL_RESULT NexShellUserReadTasks(void);

/*!
 * @brief Updates the input stream of the NexShell.
 *
 * This function must be called periodically to update the standard output stream of the NexShell.  This method is thread safe.
 *
 * @return SHELL_RESULT - The outcome of the operation.
 *
 * @details
 * This function updates the input stream of the NexShell and will execute any commands sent to the NexShell.
 * The function \ref StreamWriteDataHALCallback() is called from this method and will read data from the standard
 * output stream and pass it to \ref StreamWriteDataHALCallback() for transmission.
 *
 * @sa NexShellInit(), NexShellUserReadTasks(), StreamReaderDataHALCallback()
 */
SHELL_RESULT NexShellUserWriteTasks(void);

/*!
 * @brief Gets the root drive of the NexShell.
 *
 * @return char - The NexShell root drive.
 *
 * @details
 * This function can be called to get the root drive of the NexShell.  The NexShell root drive is determined by what
 * was passed in to \ref NexShellInit() for the parameter CurrentDrive.
 *
 * @sa NexShellInit()
 */
char NexShellGetRootDriveVolume(void);

/*!
 * @brief Returns the current working directory of the NexShell.
 *
 * @return char* - The NexShell working directory.
 *
 * @details
 * This function can be called to get the working directory of the NexShell.
 * The working directory is what is referenced with "./" in the shell.
 */
char* NexShellGetCurrentWorkingDirectory(void);

#endif // end of NEXSHELL_H