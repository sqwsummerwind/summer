//2016.9.11
//qiangwei.su
//

#ifndef SUMMER_NET_TIMERID_H
#define SUMMER_NET_TIMERID_H
#include <stdint.h>

namespace summer
{
	namespace net
	{	
		class Timer;

		class TimerId
		{
			public:
				TimerId()
				:timer_(NULL),
				sequence_(0)
				{}
				
				TimerId(Timer* timer, int64_t sequence)
				:timer_(timer),
				sequence_(sequence)
				{}

				friend class TimerQueue;
			private:
				Timer* timer_;
				const int64_t sequence_;
		};
	}
}

#endif



