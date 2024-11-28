#ifndef ECHO_COMMAND_H
	#define ECHO_COMMAND_H

/** \file
 * This file is the interface to the echo command.
 */

#include "NexShell.h"

#define ECHO_HELP_TEXT \
\
"Usage: " ECHO_COMMAND_NAME " [-neE] [arg ...]"SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB "Write arguments to the standard output." SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB "Display the ARGs, separated by a single space character and followed by a newline, on the standard output." SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB "Options:" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB"-n        do not append a newline" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB"-e        enable interpretation of the following backslash escapes" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB"-E        explicitly suppress interpretation of backslash escapes" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB "`" ECHO_COMMAND_NAME "' interprets the following backslash-escaped characters:" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB "\\n        new line" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB "\\r        carriage return" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB "\\t        horizontal tab" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB "\\v        vertical tab" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB "\\\\        backslash" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB "\\xHH      the eight - bit character whose value is HH(hexadecimal).  HH can be one or two hex digits" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB "Examples:" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB ECHO_COMMAND_NAME " 2345 > ../a-directory/numbers_file" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB ECHO_COMMAND_NAME " \"Text to a file\" > text_file" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB ECHO_COMMAND_NAME " -e \"\\r\\n Wrote a new line carriage return\" > text_file" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB ECHO_COMMAND_NAME " -e \"\\x55 The hex value 0x55 (the letter U) was written to the file too\" > text_file" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_DEFAULT_END_OF_LINE_SEQUENCE

/*!
 * @brief Main entry point for the echo command in the shell.
 *
 * @param[in] char* Args[] - An array of string arguments to process.
 *
 * @param[in] UINT32 NumberOfArgs - The number of valid arguments present in char *Args[].
 *
 * @param[in,out] PIPE* OutputStream - The stream that will get written to with any output.
 *
 * @return SHELL_RESULT - The outcome of the operation.
 *
 * @details
 * This function is what is eventually called when the user issues the echo command through
 * the shell.  Any output that is generated by the echo command is written to the OutputStream.
 */
SHELL_RESULT echoCommandExecuteMethod(char* Args[], UINT32 NumberOfArgs, PIPE* OutputStream);

#endif // end of ECHO_COMMAND_H
