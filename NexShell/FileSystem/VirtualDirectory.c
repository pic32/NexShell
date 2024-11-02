#include <string.h>
#include <ctype.h>

#include "VirtualDirectory.h"
#include "VirtualFile.h"

extern char gCurrentWorkingDirectory[];

VIRTUAL_DIRECTORY *gCurrentWorkingVirtualDirectory;

// this is our root directory
VIRTUAL_DIRECTORY gRootVirtualDirectory;
char gRootVirtualDirectoryName;

static BOOL VirtualDirectoryCharacterIsValid(char Character)
{
	// we can't have spaces
	if (isgraph((int)Character) == 0)
		return FALSE;

	if (Character == '\\' || Character == '/' || Character == ' ')
		return FALSE;

	return TRUE;
}

static BOOL VirtualDirectoryNameIsValid(char* Buffer)
{
	if (*Buffer == 0)
		return FALSE;

	while (1)
	{
		if (*Buffer == 0)
			return TRUE;

		if (VirtualDirectoryCharacterIsValid(*Buffer++) == FALSE)
			return FALSE;
	}
}

BOOL DirectoryExists(char* DirectoryPath)
{
	DIR Directory;
	FRESULT FResult;

	if (DirectoryPath == NULL)
		return FALSE;

	// just open the file
	FResult = f_opendir(&Directory, (const TCHAR*)DirectoryPath);

	// now close it
	if (FResult == FR_OK)
		f_closedir(&Directory);

	// return the result
	return (BOOL)(FResult == FR_OK);
}

DIRECTORY_TYPE GetDirectoryType(char *Path)
{
	if (Path == NULL)
		return NUMBER_OF_DIRECTORY_TYPES;

	if (IsDirectoryVirtual(Path) == TRUE)
		return VIRTUAL_MEDIA;

	if (IsDirectoryAtVirtualTransition(Path) == TRUE)
		return TRANSITION_POINT_MEDIA;

	if (DirectoryExists(Path) == TRUE)
		return PHSYCIAL_MEDIA;

	return NUMBER_OF_DIRECTORY_TYPES;
}

static BOOL IsBeginningPathVolume(char* Path, char Volume)
{
	if (*Path++ == Volume)
	{
		if (*Path == ':' || *Path == '\\' || *Path == '/')
			return TRUE;
	}

	return FALSE;
}

UINT32 VirtualDirectoryGetNumberOfDirectories(VIRTUAL_DIRECTORY* Directory)
{
	if (Directory == NULL)
		return 0;

	return LinkedListGetSize(&Directory->Directories);
}

UINT32 VirtualDirectoryGetNumberOfFiles(VIRTUAL_DIRECTORY* Directory)
{
	if (Directory == NULL)
		return 0;

	return LinkedListGetSize(&Directory->Files);
}

VIRTUAL_DIRECTORY* VirtualDirectoryGetDirectory(VIRTUAL_DIRECTORY* Directory, UINT32 DirectoryIndex)
{
	if (Directory == NULL)
		return NULL;

	return (VIRTUAL_DIRECTORY*)LinkedListGetData(&Directory->Directories, DirectoryIndex);
}

VIRTUAL_FILE *VirtualDirectoryGetFile(VIRTUAL_DIRECTORY*Directory, UINT32 FileIndex)
{
	if (Directory == NULL)
		return NULL;

	return (VIRTUAL_FILE*)LinkedListGetData(&Directory->Files, FileIndex);
}

VIRTUAL_DIRECTORY* VirtualDirectoryNameExists(VIRTUAL_DIRECTORY* Directory, char* DirectoryNameToFind)
{
	UINT32 i, Size;

	if (Directory == NULL || DirectoryNameToFind == NULL)
		return NULL;

	Size = LinkedListGetSize(&Directory->Directories);

	for (i = 1; i <= Size; i++)
	{
		VIRTUAL_DIRECTORY* DirectoryNodeToCompare = LinkedListGetData(&Directory->Directories, i);

		// compare the names
		if (strcmp(DirectoryNodeToCompare->DirectoryName, DirectoryNameToFind) == 0)
			return DirectoryNodeToCompare;
	}

	return NULL;
}

char *GetFullDirectoryPath(char* CurrentDirectory, char* NewPath, char RootVolume)
{
	// did they start with an absolute path?
	if (*NewPath == '/')
	{
		CurrentDirectory[0] = RootVolume;
		CurrentDirectory[1] = ':';

		// they did, just copy and return it
		strcpy(&CurrentDirectory[2], NewPath);
	}
	else
	{
		// they did not start with an absolute path

		if (CurrentDirectory[strlen(CurrentDirectory) - 1] != '/')
			strcat(CurrentDirectory, "/");

		// now append whatever it is that they have
		strcat(CurrentDirectory, NewPath);
	}

	if (CurrentDirectory[strlen(CurrentDirectory) - 1] != '/')
		strcat(CurrentDirectory, "/");

	return CurrentDirectory;
}

BOOL IsDirectoryVirtual(const char* FullFilePath)
{
	char TempDirectory[8];

	// make the virtual directory root path
	Shell_sprintf(TempDirectory, "%c:/" DEV_FOLDER_NAME, gCurrentWorkingDirectory[0]);

	if (memcmp(FullFilePath, TempDirectory, strlen(TempDirectory)) == 0)
	{
		// if it is bigger than R:/dev then we are referencing something in the virtual path
		if (strlen(FullFilePath) > strlen(TempDirectory))
			return TRUE;
	}

	return FALSE;
}

BOOL IsDirectoryAtVirtualTransition(const char* FullFilePath)
{
	char TempDirectory[8];

	// make the virtual directory root path
	Shell_sprintf(TempDirectory, "%c:/" DEV_FOLDER_NAME, gCurrentWorkingDirectory[0]);

	return (BOOL)(memcmp(FullFilePath, TempDirectory, strlen(TempDirectory)) == 0);
}

VIRTUAL_DIRECTORY *GenerateRootVirtualDirectory(void)
{
	gRootVirtualDirectoryName = 0;

	gRootVirtualDirectory.ParentDirectory = NULL;
	gRootVirtualDirectory.DirectoryName = &gRootVirtualDirectoryName;

	CreateLinkedList(&gRootVirtualDirectory.Directories, NULL, NULL);
	CreateLinkedList(&gRootVirtualDirectory.Files, NULL, NULL);

	gCurrentWorkingVirtualDirectory = &gRootVirtualDirectory;

	return &gRootVirtualDirectory;
}

SHELL_RESULT CreateVirtualDirectory(char *ParentDirectory, VIRTUAL_DIRECTORY* NewDirectoryToInitialize, char* DirectoryName)
{
	VIRTUAL_DIRECTORY* ParentDirectoryNode;
	BOOL Outcome;

	// check all incoming parameters
	if (ParentDirectory == NULL)
		return SHELL_INVALID_INPUT_PARAMETER;

	if (NewDirectoryToInitialize == NULL)
		return SHELL_INVALID_INPUT_PARAMETER;

	if (DirectoryName == NULL)
		return SHELL_INVALID_INPUT_PARAMETER;

	// now, is the name valid?
	if (VirtualDirectoryNameIsValid(DirectoryName) == FALSE)
		return SHELL_INVALID_DIRECTORY_NAME;

	// so we know the characters in the name are valid
	// does the name already exist?
	ParentDirectoryNode = FollowVirtualDirectory(ParentDirectory, &gRootVirtualDirectory, NULL, &Outcome);

	// was it valid?
	if (Outcome == FALSE)
		return SHELL_INVALID_DIRECTORY_NAME;

	// does it already exist in the parent directory as a directory or file?
	if (VirtualDirectoryNameExists(ParentDirectoryNode, DirectoryName) != NULL)
		return SHELL_VIRTUAL_DIRECTORY_NAME_ALREADY_EXISTS;

	// does the file already exist?
	if (VirtualFileNameExists(ParentDirectoryNode, DirectoryName) != NULL)
		return SHELL_VIRTUAL_FILENAME_ALREADY_EXISTS;

	// set the name
	NewDirectoryToInitialize->DirectoryName = DirectoryName;

	// it is a valid directory, add it
	if (LinkedListAddLast(&ParentDirectoryNode->Directories, NewDirectoryToInitialize) == FALSE)
		return SHELL_LINKED_LIST_OPERATION_FAILURE;

	// it was valid, initialize it
	NewDirectoryToInitialize->ParentDirectory = (void*)ParentDirectoryNode;

	// initialize the linked lists
	CreateLinkedList(&NewDirectoryToInitialize->Directories, NULL, NULL);
	CreateLinkedList(&NewDirectoryToInitialize->Files, NULL, NULL);

	return SHELL_SUCCESS;
}

char *GetNextVirtualDirectorySection(char *Buffer, char *DirectoryHolder, UINT32 DirectoryHolderSizeInBytes)
{
	UINT32 i;

	// clear out the stack buffer
	memset(DirectoryHolder, 0, DirectoryHolderSizeInBytes);

	i = 0;

	// are they specifying a relative directory?
	if (*Buffer == '.')
	{
		// they are, copy in the .
		while (*Buffer == '.')
		{
			// copy it over
			DirectoryHolder[i++] = *Buffer;

			// iterate to the next
			Buffer++;
		}

		// how did we end?
		if (*Buffer == '/' || *Buffer == '\\')
		{
			Buffer++;

			return Buffer;
		}

		// null is ok
		if (*Buffer == 0)
			return Buffer;
	}
	else
	{
		// now start copying
		// copy over up to the first slash, or null
		while (VirtualDirectoryCharacterIsValid(*Buffer) == TRUE)
		{
			DirectoryHolder[i++] = *Buffer;

			Buffer++;

			// are we getting overflow?
			if (i == DirectoryHolderSizeInBytes - 1)
				return NULL;
		}
	}

	switch (*Buffer)
	{
		case '/':
		case '\\':
		{
			// iterate to the next directory
			Buffer++;

			return Buffer;
		}

		case '.':
		{
			return Buffer;
		}

		case 0:
		{
			// we are at the end
			return Buffer;
		}

		default:
		{
			// invalid character
			return NULL;
		}
	}
}

SHELL_RESULT VirtualDirectoryGetPath(VIRTUAL_DIRECTORY* DirectoryToGetPath, char *PathBuffer)
{
	UINT32 DirectoryLevels, CurrentLevel;
	VIRTUAL_DIRECTORY* Directory;

	if (DirectoryToGetPath == NULL || PathBuffer == NULL)
		return SHELL_INVALID_INPUT_PARAMETER;

	Directory = DirectoryToGetPath;

	DirectoryLevels = 0;

	// see how deep we are
	while (Directory != &gRootVirtualDirectory)
	{
		Directory = (VIRTUAL_DIRECTORY*)Directory->ParentDirectory;

		DirectoryLevels++;
	}

	// get the root path setup
	PathBuffer[0] = NexShellGetRootDriveVolume();
	PathBuffer[1] = ':';
	PathBuffer[2] = '\\';
	PathBuffer[3] = 0;

	// are we in root?
	if (DirectoryLevels != 0)
	{
		// now iterate through and print out the directories from the parents
		while (DirectoryLevels != 0)
		{
			DirectoryLevels--;
			Directory = DirectoryToGetPath;

			strcat(PathBuffer, "\\");

			// iterate to the next directory
			for (CurrentLevel = 0; CurrentLevel < DirectoryLevels; CurrentLevel++)
			{
				Directory = (VIRTUAL_DIRECTORY*)Directory->ParentDirectory;
			}

			// copy over the directory path
			strcat(PathBuffer, Directory->DirectoryName);
		}
	}

	return SHELL_SUCCESS;
}

// this function allows you to bottom out over and over on root.
// meaning if you were at root and you did ../../../../ that is fine, it just stays at root
VIRTUAL_DIRECTORY* FollowVirtualDirectory(char* Buffer, VIRTUAL_DIRECTORY* RootDir, VIRTUAL_DIRECTORY* CurrentDir, BOOL *Outcome)
{
	VIRTUAL_DIRECTORY* WorkingDirectory;
	char TempBuffer[SHELL_MAX_DIRECTORY_SIZE_IN_BYTES + 1];

	if (Buffer == NULL)
		return NULL;

	if (Outcome == NULL)
		return NULL;

	*Outcome = FALSE;

	if (strlen(Buffer) == 0)
	{
		// this is the root
		*Outcome = TRUE;

		return RootDir;
	}

	#if (USE_SHELL_HOME_DIRECTORY == 1)
		// are they supplying the home directory?
		if (*Buffer == SHELL_HOME_DIRECTORY_CHARACTER && Buffer[1] == 0)
		{
			Buffer = SHELL_HOME_DIRECTORY;
		}
	#endif // end of #if (USE_SHELL_HOME_DIRECTORY == 1)

	// now are we relative or absolute for the path?
	if (*Buffer == '/' || *Buffer == '\\')
	{
		// they must have passed in a valid directory
		if (RootDir == NULL)
			return NULL;

		// we are absolute
		WorkingDirectory = RootDir;

		// iterate beyond the slashes
		Buffer++;
	}
	else
	{
		// did they pass in a valid directory?
		if (CurrentDir == NULL)
			return NULL;

		// we are relative
		WorkingDirectory = CurrentDir;
	}

	// now follow the directory
	while (1)
	{
		// get the next portion of the directory
		// it'll look like folder1/folder2/textfile
		// and we will return folder1\0 in TempBuffer
		// and Buffer will be pointing to folder2/textfile 
		if ((Buffer = GetNextVirtualDirectorySection(Buffer, TempBuffer, sizeof(TempBuffer))) == NULL)
			return NULL;

		// did we get a zero sized string?
		if (TempBuffer[0] == 0)
		{
			// we had a valid outcome
			*Outcome = TRUE;

			// return our current directory that we found
			return WorkingDirectory;
		}

		// find the specified directory
		if (TempBuffer[0] == '.')
		{
			// this is a relative directory
			if (TempBuffer[1] == '.')
			{
				// this is going up
				WorkingDirectory = (VIRTUAL_DIRECTORY*)WorkingDirectory->ParentDirectory;

				// is it root?
				if (WorkingDirectory == NULL)
				{
					WorkingDirectory =  RootDir;
				}
			}
			else
			{
				// this is the current directory
				// do nothing
			}
		}
		else
		{
			// does the directory exist?
			VIRTUAL_DIRECTORY* Directory = VirtualDirectoryNameExists(WorkingDirectory, TempBuffer);

			if (Directory == NULL)
			{
				// so the directory doesn't exist, but this could
				// still be a file.  So we will set outcome to false (already)
				// and we will return the directory to signal this
				return WorkingDirectory;
			}

			// the directory does exist, iterate to it
			WorkingDirectory = Directory;
		}
	}
}
