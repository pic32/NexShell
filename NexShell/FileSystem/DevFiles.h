#ifndef DEV_FILES_H
	#define DEV_FILES_H

/** \file
 * This file has the init method for the default dev files in the system.
 */

#include "NexShell.h"

#define RTC_0_FILENAME									"rtc0"

/*!
 * @brief Creates the default dev files of the system.
 *
 * @param[in] char RootVolume - The default root volume of the file system to place the dev files in.
 *
 * @return SHELL_RESULT - The result of the operation.
 *
 * @details
 * This method is called from NexShellInit().  This method should not be called by the user.
 */
SHELL_RESULT CreateDevFiles(char RootVolume);

#endif // end of #ifndef DEV_FILES_H
