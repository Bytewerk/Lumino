#include <errno.h>
#include <time.h>
#include <stdint.h>

#include "timeutils.h"

double get_hires_time(void)
{
	struct timespec clk;
	clock_gettime(CLOCK_REALTIME, &clk);
	return clk.tv_sec + 1e-9 * clk.tv_nsec;
}

void sleep_until(double hires_time)
{
	struct timespec tv;
	int ret;

	tv.tv_sec = hires_time;
	tv.tv_nsec = (uint64_t)(1e9 * hires_time) % 1000000000;
	do {
		ret = clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &tv, NULL);
	} while(ret == EINTR);
}

