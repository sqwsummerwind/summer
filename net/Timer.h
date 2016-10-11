//2016.09.11
//qiangwei.su


#ifndef SUMMER_NET_TIMER_H
#define SUMMER_NET_TIMER_H

#include <stdint.h>

#include <boost/function.hpp>

#include "../base/Timestamp.h"
#include "../base/Atomic.h"


namespace summer
{
	namespace net
	{
		class Timer
		{
			public:
				typedef boost::function<void()> TimerCallback;
				Timer(const TimerCallback& callback, Timestamp& when, double interval)
				:callback_(callback),
				expiration_(when),
				repeat_(interval>0),
				interval_(interval),
				sequence_(numCreated_.incrementAndGet())
				{}

				void run() const
				{
					callback_();
				}

				bool repeat() const
				{
					return repeat_;
				}

				double interval() const
				{
					return interval_;
				}

				Timestamp expiration() const
				{
					return expiration_;
				}

				int64_t sequence() const
				{
					return sequence_;
				}

				static int64_t numCreated()
				{
					return numCreated_.get();
				}

				void restart(Timestamp now);

			private:
				const TimerCallback callback_;
				Timestamp expiration_;
				const bool repeat_;
				const double interval_;
				const int64_t sequence_;

				static AtomicInt64 numCreated_;
		};
	}
}

#endif


