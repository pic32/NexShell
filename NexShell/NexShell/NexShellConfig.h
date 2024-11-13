#ifndef NEXSHELL_CONFIG_H
	#define NEXSHELL_CONFIG_H

#include "ffconf.h"

// these are the defines for the prompt
// the prompt looks like this with the defines
// PROMPT_LEADING_SEQUENCE + USERNAME + USERNAME_DIRECTORY_SEPERATION_SEQUENCE + 
// current directory + PROMPT_ENDING_SEQUENCE + ATTENTION_CHARACTER
#define SHELL_USE_PROMPT											1
#define SHELL_PROMPT_LEADING_SEQUENCE								"["
#define SHELL_USERNAME												"brodie"
#define SHELL_USERNAME_DIRECTORY_SEPERATION_SEQUENCE				" | "
#define SHELL_PROMPT_ENDING_SEQUENCE								"]"
#define SHELL_ATTENTION_CHARACTER									"$"

// these are the defines for the system startup string
// the system startup string looks like this with the defines
// PROJECT_NAME + " " + MAJOR_VERSION + "." + MINOR_VERSION + "." + TEST_VERSION
#define SHELL_USE_PROJECT_NAME										1
#define SHELL_PROJECT_NAME											"NexShell"
#define SHELL_MAJOR_VERSION											"0"
#define SHELL_MINOR_VERSION											"00"
#define SHELL_TEST_VERSION											"01"

// this is the line end sequence, once this is received the
// system will take the current input and process it as a valid line
#define SHELL_DEFAULT_END_OF_LINE_SEQUENCE							"\r\n"

// this is the size in bytes of SHELL_DEFAULT_END_OF_LINE_SEQUENCE
#define SHELL_END_OF_LINE_SEQUENCE_SIZE_IN_BYTES					2

// this is used for tab, it could be \t or spaces
#define SHELL_TAB													"  "
#define SHELL_TAB_STRING_LENGTH_IN_BYTES							strlen(SHELL_TAB)

#include <stdio.h>

#define Shell_sprintf												sprintf

// these are all the sizes of various buffers used within the Nano Shell
#ifndef SIZE_OF_OUTPUT_STREAM_BUFFER_IN_BYTES
	#define SIZE_OF_OUTPUT_STREAM_BUFFER_IN_BYTES					512
#endif // end of #ifndef SIZE_OF_OUTPUT_STREAM_BUFFER_IN_BYTES

#ifndef SIZE_OF_INPUT_STREAM_BUFFER_IN_BYTES
	#define SIZE_OF_INPUT_STREAM_BUFFER_IN_BYTES					256
#endif // end of #ifndef SIZE_OF_INPUT_STREAM_BUFFER_IN_BYTES

#ifndef SIZE_OF_SHELL_OPERATOR_STREAM_BUFFER_IN_BYTES
	#define SIZE_OF_SHELL_OPERATOR_STREAM_BUFFER_IN_BYTES			256
#endif // end of #ifndef SIZE_OF_SHELL_OPERATOR_STREAM_BUFFER_IN_BYTES

#ifndef SIZE_OF_SHELL_STACK_BUFFER_IN_BYTES
	#define SIZE_OF_SHELL_STACK_BUFFER_IN_BYTES						256
#endif // end of #ifndef SIZE_OF_SHELL_STACK_BUFFER_IN_BYTES

#ifndef SHELL_MAX_DIRECTORY_SIZE_IN_BYTES
	#define SHELL_MAX_DIRECTORY_SIZE_IN_BYTES						(FF_MAX_LFN)
#endif // end of #ifndef SHELL_MAX_DIRECTORY_SIZE_IN_BYTES

#ifndef SHELL_HAL_MAX_TRANSFER_SIZE_IN_BYTES
	#define SHELL_HAL_MAX_TRANSFER_SIZE_IN_BYTES					8
#endif // end of SHELL_HAL_MAX_TRANSFER_SIZE_IN_BYTES

#ifndef SHELL_WORKING_ARGUMENTS_ARRAY_SIZE_IN_ELEMENTS
	#define SHELL_WORKING_ARGUMENTS_ARRAY_SIZE_IN_ELEMENTS			32
#endif // end of #ifndef SHELL_WORKING_ARGUMENTS_ARRAY_SIZE_IN_ELEMENTS

#if (SIZE_OF_OUTPUT_STREAM_BUFFER_IN_BYTES < 16)
	#error "SIZE_OF_OUTPUT_STREAM_BUFFER_IN_BYTES Too Small! Must be at least 16 bytes large!"
#endif // end of #if (SIZE_OF_OUTPUT_STREAM_BUFFER_IN_BYTES < 16)

#if (SIZE_OF_INPUT_STREAM_BUFFER_IN_BYTES < 16)
	#error "SIZE_OF_INPUT_STREAM_BUFFER_IN_BYTES Too Small! Must be at least 16 bytes large!"
#endif // end of #if (SIZE_OF_INPUT_STREAM_BUFFER_IN_BYTES < 16)

#if (SIZE_OF_SHELL_OPERATOR_STREAM_BUFFER_IN_BYTES < 16)
#error "SIZE_OF_SHELL_OPERATOR_STREAM_BUFFER_IN_BYTES Too Small! Must be at least 16 bytes large!"
#endif // end of #if (SIZE_OF_SHELL_OPERATOR_STREAM_BUFFER_IN_BYTES < 16)

#if (SIZE_OF_SHELL_STACK_BUFFER_IN_BYTES < 16)
	#error "SIZE_OF_SHELL_STACK_BUFFER_IN_BYTES Too Small! Must be at least 16 bytes large!"
#endif // end of #if (SIZE_OF_SHELL_STACK_BUFFER_IN_BYTES < 16)

#if (SHELL_MAX_DIRECTORY_SIZE_IN_BYTES < 16)
	#error "SHELL_MAX_DIRECTORY_SIZE_IN_BYTES Too Small! Must be at least 16 bytes large!"
#endif // end of #if (SHELL_MAX_DIRECTORY_SIZE_IN_BYTES < 16)

#if (SHELL_HAL_MAX_TRANSFER_SIZE_IN_BYTES == 0)
	#error "SHELL_HAL_MAX_TRANSFER_SIZE_IN_BYTES Too Small! Must be at least 1 byte large!"
#endif // end of #if (SHELL_HAL_MAX_TRANSFER_SIZE_IN_BYTES == 0)

#if (SHELL_WORKING_ARGUMENTS_ARRAY_SIZE_IN_ELEMENTS < 4)
	#error "SHELL_WORKING_ARGUMENTS_ARRAY_SIZE_IN_ELEMENTS Too Small! Must be at least 4 entries large!"
#endif // end of #if (SHELL_WORKING_ARGUMENTS_ARRAY_SIZE_IN_ELEMENTS < 16)

// these are used when configuring different options within the system
#define OPTION_NOT_USED												0
#define RUNTIME_CONFIGURABLE										1
#define ALWAYS_ON													2

// this determines if the console echo can be used or not
// it should be set to either OPTION_NOT_USED, RUNTIME_CONFIGURABLE, or ALWAYS_ON
#define SHELL_USE_CONSOLE_ECHO										ALWAYS_ON

// this is the console echo setting at startup if SHELL_USE_ECHO is RUNTIME_CONFIGURABLE
#define DEFAULT_CONSOLE_ECHO_VALUE									TRUE

// if colors aren't used then the USE_SHELL_COLOR should be set to zero
#define USE_SHELL_COLOR												1

// These are all the colors of the prompt and other text
#if (USE_SHELL_COLOR == 1)
	#define SHELL_PROJECT_NAME_TEXT_COLOR							SHELL_FONT_COLOR_BRIGHT_CYAN
	#define SHELL_PROMPT_BRACKETS_TEXT_COLOR						SHELL_FONT_COLOR_BRIGHT_BLUE
	#define SHELL_USERNAME_TEXT_COLOR								SHELL_FONT_COLOR_BRIGHT_BLUE
	#define SHELL_DEFAULT_TEXT_COLOR								SHELL_FONT_COLOR_WHITE
	#define SHELL_DIRECTORY_TEXT_COLOR								SHELL_FONT_COLOR_BRIGHT_BLUE
	#define SHELL_ATTENTION_CHARACTER_TEXT_COLOR					SHELL_FONT_COLOR_BRIGHT_BLUE
	#define SHELL_ERROR_COLOR										SHELL_FONT_COLOR_BRIGHT_RED
#else
	#define SHELL_PROJECT_NAME_TEXT_COLOR						
	#define SHELL_PROMPT_BRACKETS_TEXT_COLOR						
	#define SHELL_USERNAME_TEXT_COLOR								
	#define SHELL_DEFAULT_TEXT_COLOR								
	#define SHELL_DIRECTORY_TEXT_COLOR								
	#define SHELL_ATTENTION_CHARACTER_TEXT_COLOR					
	#define SHELL_ERROR_COLOR										
#endif // end of #if (USE_SHELL_COLOR == 1)

#define SHELL_NUMBER_OF_FILE_CHARACTERS_TO_DISPLAY					16
#define USE_USER_COMMANDS											1

// these are virtual files that can be turned on and off in dev folder
#define USE_DEV_ZERO_VIRTUAL_FILE									1
#define USE_DEV_NULL_VIRTUAL_FILE									1
#define USE_DEV_RANDOM_VIRTUAL_FILE									1
#define USE_DEV_RTC_VIRTUAL_FILE									1

// use these to enable each command
#define USE_CAT_COMMAND												1
#define USE_CD_COMMAND												1
#define USE_CLEAR_COMMAND											1
#define USE_DATE_COMMAND											1
#define USE_ECHO_COMMAND											1
#define USE_HELP_COMMAND											1
#define USE_LS_COMMAND												1
#define USE_PWD_COMMAND												1
#define USE_SHUTDOWN_COMMAND										1

#define EXTENDED_CD_SUPPORT											1

// these are called when the shutdown command is received
#define ShellPowerOff()												exit(0)
#define ShellPowerDownSleep()										exit(0)
#define ShellPowerReset()											exit(0)

// this is the home directory when ~ is used for a path
#define USE_SHELL_HOME_DIRECTORY									1
#define SHELL_HOME_DIRECTORY_CHARACTER								'~'
#define SHELL_HOME_DIRECTORY_CHARACTER_STRING						"~"
#define SHELL_HOME_DIRECTORY										"R:/"


// this is the config for the history feature
#include <stdlib.h>

#define USE_SHELL_COMMAND_HISTORY									1
#define NUMBER_OF_ENTRIES_IN_HISTORY								3
#define NexShellMalloc												malloc
#define NexShellFreeMethod											free

#if (USE_SHELL_COMMAND_HISTORY == 1)
	#if (NUMBER_OF_ENTRIES_IN_HISTORY == 0)
		#error "NUMBER_OF_ENTRIES_IN_HISTORY Too Small! Must be at least 1 entries large!"
	#endif // end of #if (NUMBER_OF_ENTRIES_IN_HISTORY == 0)
#endif // end of #if (USE_SHELL_COMMAND_HISTORY == 1)

// this is how many characters the help file allocates for a commnad
// before it displays the description
#define HELP_COMMAND_NAME_SIZE_IN_BYTES								16


#endif // end of #ifndef NEXSHELL_CONFIG_H
