#include <string.h>

#include "NexShellCommands.h"
#include "LinkedList.h"

#if (USE_CAT_COMMAND == 1)
	#include "cat_Command.h"
#endif // end of #if (USE_CAT_COMMAND == 1)

#if (USE_CD_COMMAND == 1)
	#include "cd_Command.h"
#endif // end of #if (USE_CD_COMMAND == 1)

#if (USE_CLEAR_COMMAND == 1)
	#include "clear_Command.h"
#endif // end of #if (USE_CLEAR_COMMAND == 1)

#if (USE_HELP_COMMAND == 1)
	#include "help_Command.h"
#endif // end of #if (USE_HELP_COMMAND == 1)

#if (USE_LS_COMMAND == 1)
	#include "ls_Command.h"
#endif // end of #if (USE_LS_COMMAND == 1)

#if (USE_PWD_COMMAND == 1)
	#include "pwd_Command.h"
#endif // end of #if (USE_PWD_COMMAND == 1)

#if (USE_SHUTDOWN_COMMAND == 1)
	#include "shutdown_Command.h"
#endif // end of #if (USE_SHUTDOWN_COMMAND == 1)

static COMMAND_INFO gCommandList[] =
{
	#if (USE_CAT_COMMAND == 1)
		{CAT_COMMAND_NAME, catCommandExecuteMethod, "Reads the contents of a file", CAT_HELP_TEXT},
	#endif // end of #if (USE_CAT_COMMAND == 1)

	#if (USE_CD_COMMAND == 1)
		{CD_COMMAND_NAME, cdCommandExecuteMethod, "Changes the current directory", CD_HELP_TEXT},
	#endif // end of #if (USE_CD_COMMAND == 1)

	#if (USE_CLEAR_COMMAND == 1)
		{CLEAR_COMMAND_NAME, clearCommandExecuteMethod, "Clears the screen", CLEAR_HELP_TEXT},
	#endif // end of #if (USE_CLEAR_COMMAND == 1)

	#if (USE_ECHO_COMMAND == 1)
		{ECHO_COMMAND_NAME, echoCommandExecuteMethod, "Writes data to a file", ECHO_HELP_TEXT},
	#endif // end of #if (USE_ECHO_COMMAND == 1)

	#if (USE_HELP_COMMAND == 1)
		{HELP_COMMAND_NAME, helpCommandExecuteMethod, "Gets detailed help information on files", HELP_HELP_TEXT},
	#endif // end of #if (USE_HELP_COMMAND == 1)

	#if (USE_LS_COMMAND == 1)
		{LS_COMMAND_NAME, lsCommandExecuteMethod, "List the contents of a directory", LS_HELP_TEXT},
	#endif // end of #if (USE_LS_COMMAND == 1)

	#if (USE_PWD_COMMAND == 1)
		{PWD_COMMAND_NAME, pwdCommandExecuteMethod, "Outputs the current full directory", PWD_HELP_TEXT},
	#endif // end of #if (USE_PWD_COMMAND == 1)

	#if (USE_SHUTDOWN_COMMAND == 1)
		{SHUTDOWN_COMMAND_NAME, shutdownCommandExecuteMethod, "Bring the system down", SHUTDOWN_HELP_TEXT}
	#endif // end of #if (USE_SHUTDOWN_COMMAND == 1)
};

#if (USE_USER_COMMANDS == 1)
	LINKED_LIST gUserCommandList;

	static INT32 UserCommandListCompareTo(const void* Data1, const void* Data2)
	{
		// Data1 is a pointer to COMMAND_INFO
		// Data2 is a char pointer to a name
		COMMAND_INFO* CommandInfo = (COMMAND_INFO*)Data1;

		return (INT32)strcmp(CommandInfo->CommandName, (const char*)Data2);
	}

	SHELL_RESULT InitNexShellCommands(void)
	{
		if (CreateLinkedList(&gUserCommandList, UserCommandListCompareTo, NULL) == NULL)
			return SHELL_LINKED_LIST_CREATE_FAILURE;

		return SHELL_SUCCESS;
	}

	SHELL_RESULT AddUserCommand(const char* CommandName, const char* Description, const char* HelpFile, SHELL_RESULT(*ExecuteFile)(char* [], UINT32, GENERIC_BUFFER*))
	{
		UINT32 i;
		COMMAND_INFO* CommandInfo;

		// validate the minimum data required
		if (CommandName == NULL || ExecuteFile == NULL)
			return SHELL_INVALID_ARGUMENT;

		// does the command already exist?
		if (LinkedListContains(&gUserCommandList, CommandName, 0, 0) != 0)
		{
			// it already exists
			return SHELL_COMMAND_ALREADY_EXISTS;
		}

		// is it in the regular command list?
		for (i = 0; i < sizeof(gCommandList) / sizeof(COMMAND_INFO); i++)
		{
			if (strcmp(CommandName, gCommandList[i].CommandName) == 0)
				return SHELL_COMMAND_ALREADY_EXISTS;
		}

		// get the space for the structure
		CommandInfo = NexShellMalloc(sizeof(COMMAND_INFO));

		// did we get the space?
		if (CommandInfo == NULL)
			return SHELL_MALLOC_FAILURE;

		// add it to the list
		if (LinkedListAdd(&gUserCommandList, CommandInfo) == FALSE)
		{
			// we didn't get the space, free this space
			NexShellFreeMethod(CommandInfo);

			return SHELL_LINKED_LIST_OPERATION_FAILURE;
		}

		// initialize the data since we know we have all the space
		CommandInfo->CommandName = (char*)CommandName;
		CommandInfo->Description = (char*)Description;
		CommandInfo->Help = (char*)HelpFile;
		CommandInfo->ExecuteFile = ExecuteFile;

		return SHELL_SUCCESS;
	}

	COMMAND_INFO* GetUserCommandByName(char* CommandName)
	{
		UINT32 Index = LinkedListContains(&gUserCommandList, CommandName, 0, 0);

		if (Index == 0)
			return NULL;

		return (COMMAND_INFO*)LinkedListGetData(&gUserCommandList, Index);
	}

	COMMAND_INFO* GetUserCommandByIndex(UINT32 Index)
	{
		return (COMMAND_INFO*)LinkedListGetData(&gUserCommandList, Index);
	}
#endif // end of #if (USE_USER_COMMANDS == 1)

UINT32 GetNumberOfNextShellNativeCommands(void)
{
	return sizeof(gCommandList) / sizeof(COMMAND_INFO);
}

COMMAND_INFO* GetNativeCommandInfo(UINT32 Index)
{
	if (Index >= sizeof(gCommandList) / sizeof(COMMAND_INFO))
		return NULL;

	return &gCommandList[Index];
}
