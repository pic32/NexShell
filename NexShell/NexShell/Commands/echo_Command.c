#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "NexShell.h"
#include "echo_Command.h"

static BOOL EchoOptionsValid(char *String)
{
	if (*String++ != '-')
		return FALSE;

	do
	{
		switch (*String++)
		{
			case 'e':
			case 'E':
			case 'n':
			{
				// valid letter
				break;
			}

			case 0:
			{
				// we reached the end
				return TRUE;
			}

			default:
			{
				// invalid letter
				return FALSE;
			}
		}
	} 
	while (*String);

	return TRUE;
}

void UpdateBackslashCharacters(char *String)
{
	while (*String)
	{
		if (*String == '\\')
		{
			// now whats the second character?
			switch (String[1])
			{
				case 'n':
				{
					*String = '\n';

					break;
				}

				case 'r':
				{
					*String = '\r';

					break;
				}

				case 't':
				{
					*String = '\t';

					break;
				}

				case 'v':
				{
					*String = '\v';

					break;
				}

				case '\\':
				{
					*String = '\\';

					break;
				}

				case 'x':
				{
					char HexBuffer[3];
					char *EndPtr;
					char *HexPtr = &String[2];

					// clear it out
					memset(HexBuffer, 0, sizeof(HexBuffer));

					// do we even have a hex digit?
					if (isxdigit(*HexPtr) != FALSE)
					{
						// yes, get it
						HexBuffer[0] = *HexPtr++;

						// is the next digit hex?
						if (isxdigit(*HexPtr) != FALSE)
						{
							// it is, get it
							HexBuffer[1] = *HexPtr++;
						}

						// now convert it
						*String = (BYTE)strtol(HexBuffer, &EndPtr, 16);
					}
					else
					{
						// invalid hex, just continue
						String += 2;

						continue;
					}

					// this is a special case to continue since the width is bigger
					String++;

					// null it out in case we're a strlen of 0
					*String = 0;

					// now copy over any data
					strcpy(String, HexPtr);
					
					continue;
				}

				case 0:
				{
					return;
				}

				default:
				{
					// skip this, very bad to use continue here, but it'll cut down on code
					String += 2;

					continue;
				}
			}

			// null this out in case we have a strlen of zero
			String[1] = 0;

			// we could only get here if we found a valid option and need to shift things
			// now shift everything
			strcpy(&String[1], &String[2]);

			// iterate to the new character
			String++;
		}
		else
		{
			// go to the next character, we didn't find a '\'
			String++;
		}
	}
}

SHELL_RESULT echoCommandExecuteMethod(char* Args[], UINT32 NumberOfArgs, PIPE* OutputStream)
{
	char *SpecialCharacters;
	UINT32 StringArguments;
	BOOL EnableBackslash = FALSE;
	BOOL AppendNewLine = TRUE;

	if (NumberOfArgs == 0)
		return SHELL_INVALID_INPUT_PARAMETER;

	// get any options
	if (EchoOptionsValid(Args[0]) == TRUE)
	{
		// update our assumed user string index
		StringArguments = 1;

		// decrease our size
		NumberOfArgs--;

		// we have options
		SpecialCharacters = Args[0];
		
		// iterate beyond the '-'
		SpecialCharacters++;

		while (*SpecialCharacters)
		{
			switch (*SpecialCharacters++)
			{
				case 'e':
				{
					EnableBackslash = TRUE;

					break;
				}

				case 'E':
				{
					EnableBackslash = FALSE;

					break;
				}

				case 'n':
				{
					AppendNewLine = FALSE;

					break;
				}
			}
		}
	}
	else
	{
		StringArguments = 0;
	}

	// we're done?
	if (NumberOfArgs == 0)
	{
		if (PipeWrite(OutputStream, SHELL_DEFAULT_END_OF_LINE_SEQUENCE, SHELL_END_OF_LINE_SEQUENCE_SIZE_IN_BYTES, NULL) != OS_SUCCESS)
			return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

		// yes, we're done
		if(AppendNewLine == TRUE)
			if (PipeWrite(OutputStream, SHELL_DEFAULT_END_OF_LINE_SEQUENCE, SHELL_END_OF_LINE_SEQUENCE_SIZE_IN_BYTES, NULL) != OS_SUCCESS)
				return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

		return SHELL_SUCCESS;
	}

	// is that all they specified
	do
	{
		// now where's the data going?
		NumberOfArgs--;

		// so now we have the options, process the backslash if it was specified
		if (EnableBackslash == TRUE)
		{
			// we have to go through each area and interpret any values
			// that have a backslash before them
			UpdateBackslashCharacters(Args[StringArguments]);
		}

		if (PipeWrite(OutputStream, Args[StringArguments], (UINT32)strlen(Args[StringArguments]), NULL) != OS_SUCCESS)
			return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

		StringArguments++;
	}
	while (NumberOfArgs != 0);

	if (PipeWrite(OutputStream, SHELL_DEFAULT_END_OF_LINE_SEQUENCE, SHELL_END_OF_LINE_SEQUENCE_SIZE_IN_BYTES, NULL) != OS_SUCCESS)
		return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

	if (AppendNewLine == TRUE)
		if (PipeWrite(OutputStream, SHELL_DEFAULT_END_OF_LINE_SEQUENCE, SHELL_END_OF_LINE_SEQUENCE_SIZE_IN_BYTES, NULL) != OS_SUCCESS)
			return SHELL_GENERIC_BUFFER_WRITE_FAILURE;

	return SHELL_SUCCESS;
}