#ifndef DATE_COMMAND_H
	#define DATE_COMMAND_H

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
SHELL_TAB SHELL_TAB "%Y        last two digits of year(00..99)" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB "Examples:" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB DATE_COMMAND_NAME " \'+The current time is %c%n\'" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB DATE_COMMAND_NAME " \"-r help.txt\" \'+The last modified time of help.txt was %c%n\'" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB DATE_COMMAND_NAME SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_DEFAULT_END_OF_LINE_SEQUENCE

BOOL ConvertFileSystemDateTimeToSystemDateTime(UINT16 Date, UINT16 Time, rtc_time* SystemDateTime);

SHELL_RESULT dateCommandExecuteMethod(char* Args[], UINT32 NumberOfArgs, PIPE* OutputStream);

#endif // end of DATE_COMMAND_H
