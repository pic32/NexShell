#include <time.h>

#include "ioctl.h"
#include "NexShellTime.h"

long ioctl(unsigned int cmd, void *arg)
{
	switch (cmd)
	{
		case GET_DATE_TIME_CMD:
		{
			time_t RawTime;
			struct tm* TimeInfo;
			rtc_time* CurrentTime = (rtc_time *)arg;

			time(&RawTime);

			TimeInfo = localtime(&RawTime);

			CurrentTime->tm_sec = TimeInfo->tm_sec;
			CurrentTime->tm_min = TimeInfo->tm_min;
			CurrentTime->tm_hour = TimeInfo->tm_hour;
			CurrentTime->tm_wday = TimeInfo->tm_wday;
			CurrentTime->tm_mday = TimeInfo->tm_mday;
			CurrentTime->tm_mon = TimeInfo->tm_mon;
			CurrentTime->tm_year = TimeInfo->tm_year;
			CurrentTime->tm_isdst = TimeInfo->tm_isdst;
			CurrentTime->tm_yday = TimeInfo->tm_yday;

			return 0;
		}

		default:
		{
			return ESRCH;
		}
	}
}