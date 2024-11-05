#ifndef NEXSHELL_TIME_H
	#define NEXSHELL_TIME_H

#include "GenericTypeDefs.h"
#include "ff.h"

typedef union
{
	UINT32 Value;

	struct
	{
		unsigned int Seconds : 5; // LSB 0 - 30 (it's seconds divided by 2)
		unsigned int Minutes : 6; // 0 - 59
		unsigned int Hours : 5; // 0 - 23
		unsigned int Day : 5; // 1 - 31
		unsigned int Month : 4; // 1 - 12
		unsigned int Year : 6; // MSB (year since 1980, so 1981 is 1)
	}BITS;

	struct
	{
		UINT16 Time;
		UINT16 Date;
	}DATE_TIME;
}PACKED_DATE_TIME;

DWORD get_fattime(void);

BYTE GetNexShellFileInfoSeconds(UINT16 Time);

BYTE GetNexShellFileInfoMinutes(UINT16 Time);

BYTE GetNexShellFileInfoHours(UINT16 Time);

BYTE GetNexShellFileInfoDay(UINT16 Date);

BYTE GetNexShellFileInfoMonth(UINT16 Date);

UINT16 GetNexShellFileInfoYear(UINT16 Date);

#endif // end of NEXSHELL_TIME_H