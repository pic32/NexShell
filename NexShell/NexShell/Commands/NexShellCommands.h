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

 /*!
  * @brief Initializes the user shell commands feature.
  *
  * @return SHELL_RESULT - The outcome of the operation.
  *
  * @details
  * This function is called by NexShellInit().  This function should not be called by the user.
  */
SHELL_RESULT InitNexShellCommands(void);

/*!
 * @brief Gets the number of native commands supported by the shell.
 *
 * @return UINT32 - The number of native commands supported by the shell.  These commands are the ls, cd, etc. commands.
 */
UINT32 GetNumberOfNextShellNativeCommands(void);

/*!
 * @brief Gets the \ref COMMAND_INFO of the native command.
 *
 * @param[in] UINT32 Index - This is the 1 based index of the native command list.
 *
 * @return COMMAND_INFO - The \ref COMMAND_INFO found at the index.  If the index does not exist, NULL is returned.
 *
 * @sa GetNumberOfNextShellNativeCommands(), GetUserCommandByIndex()
 */
COMMAND_INFO* GetNativeCommandInfo(UINT32 Index);

/*!
 * @brief Gets the \ref COMMAND_INFO of the user command.
 *
 * @param[in] char* CommandName - The command name to find in the user command list.
 *
 * @return COMMAND_INFO - The user \ref COMMAND_INFO found with the name of CommandName.  If the command does not exist, NULL is returned.
 *
 * @sa GetUserCommandByIndex()
 */
COMMAND_INFO* GetUserCommandByName(char* CommandName);

/*!
 * @brief Gets the \ref COMMAND_INFO of the user command.
 *
 * @param[in] UINT32 Index - This is the 1 based index of the user command list.
 *
 * @return COMMAND_INFO - The \ref COMMAND_INFO found at the index.  If the index does not exist, NULL is returned.
 *
 * @sa GetUserCommandByName()
 */
COMMAND_INFO* GetUserCommandByIndex(UINT32 Index);

/*!
 * @brief Adds a user command to the user command list.
 *
 * @param[in] const char* CommandName - The command name to associate with the command.  This must be unique when compared to other user commands and native commands.
 * @param[in] const char* Description - The description to associate with the command.  This is displayed with the ls and help commands.  This parameter is optional.
 * @param[in] const char* HelpFile - The help message to display when help [command_name] is entered into the shell.  This can be a string or the path of a file to display.  This parameter is optional.
 * @param[in] SHELL_RESULT(*)(char* [], UINT32, PIPE*) ExecuteFile - The method to execute when the user enters the command into the shell.  The char*[] argument is an array
 * of arguments the user can send the command.  The UINT32 is the number of arguments in the arg array.  The PIPE* is the stream to write data output to.
 *
 * @return SHELL_RESULT - The outcome of the operation.
 *
 * @details
 * This function is useful to add custom commands to the shell interface.  Each command name must be unique.  Description and HelpFile are not mandatory 
 * but strongly encouraged.  It is also strongly encouraged to support the --help option too.  The PIPE* which is passed to the ExecuteFile method can
 * be the standard output stream or a file.  The ExecuteFile method doesn't know or care as the interface for writing is always the same.
 */
SHELL_RESULT AddUserCommand(const char* CommandName, const char* Description, const char* HelpFile, SHELL_RESULT(*ExecuteFile)(char* [], UINT32, PIPE*));

#endif // end of #ifndef NEXSHELL_COMMANDS_H