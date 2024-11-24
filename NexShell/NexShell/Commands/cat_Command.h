#ifndef CAT_COMMAND_H
	#define CAT_COMMAND_H

#include "NexShell.h"

#define CAT_HELP_TEXT \
\
"Usage: " CAT_COMMAND_NAME " [OPTION]... [FILE]..." SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB "Read contents of a file." SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB "Display the content of file on the standard output." SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB "Options:" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB"-E        display $ at end of each line" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB"-n        number all output lines" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB"-T        display TAB characters as ^I" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB"-v        use ^ and M- notation, except for LFD and TAB" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB "Examples:" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB CAT_COMMAND_NAME " /a-folder/file-to-get-the-text-of" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_TAB SHELL_TAB CAT_COMMAND_NAME " -EnT ../a-folder/file-to-get-the-text-of another-file-to-read" SHELL_DEFAULT_END_OF_LINE_SEQUENCE \
SHELL_DEFAULT_END_OF_LINE_SEQUENCE

typedef union
{
	BYTE Value;

	struct
	{
		unsigned int ShowLineEnds : 1;					// -E
		unsigned int NumberAllLines : 1;				// -n
		unsigned int SupressRepeativeEmptyLines : 1;	// -s (unsupported for now)
		unsigned int ShowTabs : 1;						// -T
		unsigned int ShowControlCharacters : 1;			// -v
		unsigned int CarriageReturnPresent : 1;			// Internal Use
		unsigned int Reserved : 2;
	}BITS;
}READ_OPTIONS;

typedef struct
{
	READ_OPTIONS ReadOptions;
	UINT32 LineNumber;
	UINT32 NumberOfEmptyLines;
}READ_INFO;

SHELL_RESULT catCommandExecuteMethod(char* Args[], UINT32 NumberOfArgs, PIPE* OutputStream);

#endif // end of CAT_COMMAND_H