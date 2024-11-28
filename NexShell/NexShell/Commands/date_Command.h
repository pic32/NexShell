#ifndef DATE_COMMAND_H
	#define DATE_COMMAND_H

/** \file
 * This file is the interface to the date command.
 */

#include "NexShell.h"
#include "NexShellTime.h"

#define DATE_HELP_TEXT \
\
"Usage: " DATE_COMMAND_NAME " [OPTION]... [+FORMAT]" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB "Read contents of a file." SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB "Display the content of file on the standard output." SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB "Options:" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB"-r        display the last modification time of FILE" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB"-s        set the current date and time" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB "Format:" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB "%%        a literal %" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB "%a        locale's abbreviated weekday name (e.g., Sun)" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB "%A        locale's full weekday name (e.g., Sunday)" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB "%b        locale's abbreviated month name (e.g., Jan)" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB "%B        locale's full month name (e.g., January)" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB "%c        locale's date and time (e.g., Thu Mar 3 23:05:25 2005)" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB "%C        century; like %Y, except omit last two digits (e.g., 20)" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB "%d        day of month (e.g, 01)" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB "%D        date; same as %m/%d/%y" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB "%e        day of month, space padded; same as %_d" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB "%F        full date; same as %Y-%m-%d" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB "%h        same as %b" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB "%H        hour (00..23)" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB "%I        hour (01..12)" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB "%j        day of year (001..366)" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB "%k        hour ( 0..23)" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB "%l        hour ( 1..12)" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB "%m        month (01..12)" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB "%M        minute (00..59)" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB "%n        a newline" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB "%p        locale's equivalent of either AM or PM; blank if not known" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB "%P        like %p, but lower case" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB "%r        locale's 12-hour clock time (e.g., 11:11:04 PM)" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB "%R        24 - hour hour and minute; same as %H:%M" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB "%S        second(00..60)" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB "%t        a tab" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB "%T        time; same as time; same as %H:%M:%S" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB "%u        day of week(1..7); 1 is Monday" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB "%w        day of week(0..6); 0 is Sunday" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB "%x        locale's date representation (e.g., 12/31/99)" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB "%X        locale's time representation (e.g., 23:13:48)" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB "%y        last two digits of year(00..99)" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB "%Y        year" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB "Examples:" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB DATE_COMMAND_NAME " \'+The current time is %c%n\'" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB DATE_COMMAND_NAME " \"-r help.txt\" \'+The last modified time of help.txt was %c%n\'" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB DATE_COMMAND_NAME SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_DEFAULT_END_OF_LINE_SEQUENCE

 /*!
  * @brief Converts a packed date time to a rtc_time.
  *
  * @param[in] UINT16 Date - The packed date.  This is usually used with the FatFs.
  *
  * @param[in] UINT16 Time - The packed time.  This is usually used with the FatFs.
  *
  * @param[in,out] rtc_time* SystemDateTime - The rtc_time which will get its date 
  * and time from Date and Time parameters passed to this method.
  *
  * @return BOOL - TRUE if the conversion succeeded, FALSE otherwise.
  *
  * @details
  * This method will take the specified packed date and time and convert it into the rtc_time.
  * The packed date and time are usually only used with the FatFs.
  */
BOOL ConvertFileSystemDateTimeToSystemDateTime(UINT16 Date, UINT16 Time, rtc_time* SystemDateTime);

/*!
 * @brief Main entry point for the date command in the shell.
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
 * This function is what is eventually called when the user issues the date command through
 * the shell.  Any output that is generated by the date command is written to the OutputStream.
 */
SHELL_RESULT dateCommandExecuteMethod(char* Args[], UINT32 NumberOfArgs, PIPE* OutputStream);

#endif // end of DATE_COMMAND_H
