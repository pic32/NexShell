#ifndef NEXSHELL_COMMANDS_H
	#define NEXSHELL_COMMANDS_H

/** \file
 * This is the interface to adding user commands.
 * User commands are global commands that will appear in the help command list.
 * There is a global \ref LINKED_LIST that is used to hold the data for the commands.
 * When the shell gets a valid command it will go through this list to see if a 
 * match occurs.  If there is a match the execute method will be called.
 * 
 * \image html UserCommandList.png "User Command List"
 */

#include "GenericTypes.h"
#include "NexShell.h"

SHELL_RESULT InitNexShellCommands(void);

UINT32 GetNumberOfNextShellNativeCommands(void);

COMMAND_INFO* GetNativeCommandInfo(UINT32 Index);

COMMAND_INFO* GetUserCommandByName(char* CommandName);

COMMAND_INFO* GetUserCommandByIndex(UINT32 Index);

SHELL_RESULT AddUserCommand(const char* CommandName, const char* Description, const char* HelpFile, SHELL_RESULT(*ExecuteFile)(char* [], UINT32, PIPE*));

#endif // end of #ifndef NEXSHELL_COMMANDS_H