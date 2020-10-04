#ifndef _TIMER_H_
#define _TIMER_H_

#include <stddef.h>

namespace mytime {

class Timer {
public:
	Timer()
	{
		if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &m_time_start) != 0)
			throw TimerError();
	}
	
	class TimerError { };

	uint64_t elapsed_us() const
	{
		struct timespec time_end;
		if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time_end) != 0)
			throw TimerError();	

		return (time_end.tv_sec - m_time_start.tv_sec) * 1000000
			+ (time_end.tv_nsec - m_time_start.tv_nsec) / 1000;
	}
private:
	struct timespec m_time_start;
};

} // mytime namespace end

#endif // _TIMER_H_