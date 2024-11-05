#ifndef NEXSHELL_COMMANDS_H
	#define NEXSHELL_COMMANDS_H

#include "GenericTypeDefs.h"
#include "NexShell.h"

SHELL_RESULT InitNexShellCommands(void);

UINT32 GetNumberOfNextShellNativeCommands(void);

COMMAND_INFO* GetNativeCommandInfo(UINT32 Index);

COMMAND_INFO* GetUserCommandByName(char* CommandName);

COMMAND_INFO* GetUserCommandByIndex(UINT32 Index);

SHELL_RESULT AddUserCommand(const char* CommandName, const char* Description, const char* HelpFile, SHELL_RESULT(*ExecuteFile)(char* [], UINT32, GENERIC_BUFFER*));

#endif // end of #ifndef NEXSHELL_COMMANDS_H