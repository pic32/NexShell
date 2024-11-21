#ifndef VIRTUAL_FILE_H
	#define VIRTUAL_FILE_H

/** \file
 * This is the interface to \ref VIRTUAL_FILES.
 * This file contains all the methods to interact with virtual files.
 * \ref VIRTUAL_FILES are made at runtime and do not exist on disk.  They
 * can have read, write and execute handlers.  These are expected to
 * be implemented by the developer.
 */

#include "NexShell.h"
#include "LinkedList.h"

/*!
* @struct VIRTUAL_FILE
* 
* @brief This struct holds the members for a \ref VIRTUAL_FILE.
* 
* @param ReadFileData - The read file method initialized when calling \ref CreateVirtualFile().
* This parameter is not mandatory.  This method is called when the user issues a cat command on the virtual file.
* 
* @param WriteFileData - The write file method initialized when calling \ref CreateVirtualFile().
* This parameter is not mandatory.  This method is called when the user issues an echo command on the virtual file.
* 
* @param ExecuteFile - The execute file method initialized when calling \ref CreateVirtualFile().
* This parameter is not mandatory.  This method is called when the user calls by name the virtual file.
* 
* @param FileName - The name of the \ref VIRTUAL_FILE.  This should just be the name and no path.  This parameter is mandatory.
* 
* @param FileDescription - A description of the \ref VIRTUAL_FILE.  this parameter is optional.  The description
* is shown when the user does an "ls" command on a directory or when the "help" command is done with -d option.
* 
* @param FileHelp - The help file associated with the \ref VIRTUAL_FILE.  This is to give the user help when typing in
* "help [virtual file name]".  This parameter is optional.  A file on disk can be referenced by this parameter to show
* the contents of.  The system will first try to interpret this string as a valid file (with full path).  If the file exists
* the contents of the file will be shown when the "help" command is used, otherwise the contents pointed to by FileHelp
* are displayed for help.
* 
*/
typedef struct
{
	SHELL_RESULT(*ReadFileData)(PIPE* OutputBuffer);
	SHELL_RESULT(*WriteFileData)(char* Args[], UINT32 NumberOfArgs, PIPE* OutputBuffer);
	SHELL_RESULT(*ExecuteFile)(char* Args[], UINT32 NumberOfArgs, PIPE* OutputBuffer);

	char* FileName;
	char* FileDescription;
	char* FileHelp;
}VIRTUAL_FILE;

typedef struct
{
	LINKED_LIST VirtualFileList;
	char* DirectoryName;
}VIRTUAL_FILE_DIRECTORY;

/*!
 * @brief Initializes the virtual file interface.
 *
 * This function is called at startup by NexShellInit() and should never be called by the user.
 *
 * @return \ref SHELL_RESULT - The outcome of the operation.
 *
 * @details
 * This function should not be called by the user.  The main use of this function is to initialize
 * the LINKED_LIST that is used to store virtual file handles in.  The system uses this LINKED_LIST
 * to find virtual files.
 */
SHELL_RESULT VirtualFileInit(void);

/*!
 * @brief Returns the LINKED_LIST which stores the virtual files.
 *
 * @param[in] char *Directory - The directory to get the virtual file list of.  This is the full path
 * of the directory of interest.  An example path might be "R:/dev".
 *
 * @return LINKED_LIST * - The LINKED_LIST which holds the virtual files in the directory specified.  If
 * the directory doesn't exist NULL is returned.
 *
 * @details
 * While this function could be used by the user, it really does not have much meaning and should
 * not be called.
 */
LINKED_LIST* GetVirtualFileList(char* Directory);

SHELL_RESULT VirtualFileAddToVirtualFileSystem(VIRTUAL_FILE* VirtualFile, char* DirectoryPath);

VIRTUAL_FILE* VirtualFileNameExists(LINKED_LIST* FileList, char* FileNameToFind);

VIRTUAL_FILE* GetVirtualFile(char* Directory, char* Filename);

/*!
 * @brief Initializes a \ref VIRTUAL_FILE struct.
 *
 * @param[in/out] VIRTUAL_FILE *NewFileToInitialize - A pointer to a valid \ref VIRTUAL_FILE.  This parameter is mandatory.
 * 
 * @param[in] const char *FileName - The file name of the virtual file.  This parameter is mandatory.
 *
 * @param[in] ReadFileData - The read file method.  This parameter is not mandatory.  This method is called when the user
 * issues a cat command on the \ref VIRTUAL_FILE.
 * 
 * @param[in] WriteFileData - The write file method.  This parameter is not mandatory.  This method is called when the user
 * issues an echo command on the \ref VIRTUAL_FILE.
 * 
 * @param[in] ExecuteFile - The execute file method initialized when calling \ref CreateVirtualFile().
 * This parameter is not mandatory.  This method is called when the user calls by name the virtual file.
 * 
 * @param FileName - The name of the \ref VIRTUAL_FILE.  This should just be the name and no path.  This parameter is mandatory.
 * 
 * @param FileDescription - A description of the \ref VIRTUAL_FILE.  this parameter is optional.  The description
 * is shown when the user does an "ls" command on a directory or when the "help" command is done with -d option.
 * 
 * @param FileHelp - The help file associated with the \ref VIRTUAL_FILE.  This is to give the user help when typing in
 * "help [virtual file name]".  This parameter is optional.  A file on disk can be referenced by this parameter to show
 * the contents of.  The system will first try to interpret this string as a valid file (with full path).  If the file exists
 * the contents of the file will be shown when the "help" command is used, otherwise the contents pointed to by FileHelp
 * are displayed for help.
 *
 * @return \ref SHELL_RESULT - The outcome of the operation.
 *
 * @details
 * This method just initializes the specified \ref VIRTUAL_FILE struct.  It does not add it to
 * any directory.  To add a \ref VIRTUAL_FILE to a directory call \ref VirtualFileAddToVirtualFileSystem()
 * 
 * @sa VirtualFileAddToVirtualFileSystem()
 */
SHELL_RESULT CreateVirtualFile(VIRTUAL_FILE* NewFileToInitialize, const char *FileName, SHELL_RESULT(*ReadFileData)(PIPE *), SHELL_RESULT(*WriteFileData)(char*[], UINT32, PIPE *), SHELL_RESULT(*ExecuteFile)(char*[], UINT32, PIPE *), const char* FileDescription, const char* FileHelp);

#endif // end of #ifndef VIRTUAL_FILE_H
