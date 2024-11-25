#ifndef NEXSHELL_CALLBACKS_H
	#define NEXSHELL_CALLBACKS_H

/** \file
 * This contains the callbacks that need to be implemented by the user for the NexShell to run.
 */

#include "GenericTypes.h"

/*!
 * @brief Writes data out from the standard output stream.
 *
 * @param[in] BYTE *DataBuffer - The data to write.
 * @param[in] UINT32 DataBuffersSizeInBytes - The requested number of bytes in DataBuffer to write.
 *
 * @return UINT32 - The number of bytes successfully written by the callback function.
 *
 * @details
 * This function must be implemented by the user.  It is designed to take data from the shell and
 * output it to a hardware device like a UART.
 *
 * @sa StreamReaderDataHALCallback()
 */
UINT32 StreamWriteDataHALCallback(BYTE* DataBuffer, UINT32 DataBufferSizeInBytes);

/*!
 * @brief Read data in from another source.
 *
 * @param[in,out] BYTE *DataBuffer - The buffer to store the data in.
 * @param[in] UINT32 DataBuffersSizeInBytes - The requested number of bytes in DataBuffer to write.
 *
 * @return UINT32 - The number of bytes successfully written by the callback function.
 *
 * @details
 * This function must be implemented by the user.  It is designed to take data from another source
 * (usually a hardware peripheral like a UART) and write it to DataBuffer.  The max number that
 * DataBufferSizeInBytes would contain is \ref SHELL_HAL_MAX_TRANSFER_SIZE_IN_BYTES.
 *
 * @sa StreamWriteDataHALCallback()
 */
UINT32 StreamReaderDataHALCallback(BYTE* DataBuffer, UINT32 DataBufferSizeInBytes);

/*!
 * @brief Get the packed date time for the FatFs.
 *
 * @return DWORD - The packed date time for use by the FatFs.
 *
 * @details
 * This function must be implemented by the user.  It is designed to return the packed date time
 * for the FatFs.  The DWORD that is returned must have the following format:
 *
 *	bit 31:25
 *	Year origin from the 1980 (0..127, e.g. 37 for 2017)
 *
 *	bit 24:21
 *	Month (1..12)
 *
 *	bit 20:16
 *	Day of the month (1..31)
 *
 *	bit 15:11
 *	Hour (0..23)
 *
 *	bit 10:5
 *	Minute (0..59)
 *
 *	bit 4:0
 *	Second / 2 (0..29, e.g. 25 for 50) 
 */
DWORD get_fattime(void);

/*!
 * @brief Shuts the system down and powers off.
 *
 * @details
 * This function must be implemented by the user if \ref USE_SHUTDOWN_COMMAND is set to a 1.
 * This callback is called when the user initiates a "shutdown -P" command, and is called
 * before the system calls \ref ioctl() with the \ref SYSTEM_SHUTDOWN_POWER_OFF argument.
 * This gives the developer a chance to clean anything up before the system actually turns off.
 *
 * @sa ShellPowerDownSleepCallback(), ShellPowerResetCallback()
 */
void ShellPowerOffCallback(void);

/*!
 * @brief Puts the system into sleep mode.
 *
 * @details
 * This function must be implemented by the user if \ref USE_SHUTDOWN_COMMAND is set to a 1.
 * This callback is called when the user initiates a "shutdown -H" command, and is called
 * before the system calls \ref ioctl() with the \ref SYSTEM_SHUTDOWN_SLEEP argument.
 * This gives the developer a chance to clean anything up before the system actually sleeps.
 * Once the system is asleep power consumption will be reduced.  Any interrupt will wake the
 * system up and execution will resume from this command.
 *
 * @sa ShellPowerOffCallback(), ShellPowerResetCallback()
 */
void ShellPowerDownSleepCallback(void);

/*!
 * @brief Resets the system.
 *
 * @details
 * This function must be implemented by the user if \ref USE_SHUTDOWN_COMMAND is set to a 1.
 * This callback is called when the user initiates a "shutdown -r" command, and is called
 * before the system calls \ref ioctl() with the \ref SYSTEM_SHUTDOWN_RESET argument.
 * This gives the developer a chance to clean anything up before the system actually resets.
 * Once the system is reset execution will begin as if the system powered on.
 *
 * @sa ShellPowerOffCallback(), ShellPowerDownSleepCallback()
 */
void ShellPowerResetCallback(void);

#endif // end of #ifndef NEXSHELL_CALLBACKS_H
