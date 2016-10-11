//2016.9.11
//qiangwei.su
//

#ifndef SUMMER_NET_TIMERQUEUE_H
#define SUMMER_NET_TIMERQUEUE_H

#include <utility>
#include <vector>
#include <set>
#include <stdint.h>

#include <boost/function.hpp>

#include "Channel.h"
#include "../base/Timestamp.h"

namespace summer
{
	namespace net
	{	

		class EventLoop;
		class Timer;
		class TimerId;

		class TimerQueue
		{
			public:
				typedef boost::function<void()> TimerCallback;
				
				TimerQueue(EventLoop* loop);
				
				TimerId addTimer(const TimerCallback& cb, Timestamp when, double interval);
				void cancel(TimerId timerId);

			private:

				typedef std::pair<Timestamp, Timer*> Entry;
				typedef std::set<Entry> TimerList;
				typedef std::pair<Timer*, int64_t> Active;
				typedef std::set<Active> ActiveTimerSet;
				
				void addTimerInLoop(Timer* timer);
				void cancelInLoop(TimerId timerId);

				//call when timerfd is ready to read
				void handleRead();

				//insert into timer list
				bool insert(Timer* timer);

				//get all expired time
				std::vector<Entry> getExpired(Timestamp now);
				//the timer maybe repeated, call insert again
				void reset(std::vector<Entry>& expired, Timestamp now);

				EventLoop* loop_;
				const int timerfd_;
				Channel timerfdChannel_;
				
				//timer list sorted by expiration
				TimerList timers_;

				//for cancel
				ActiveTimerSet activeTimers_;
				bool doingExpiredTimers_;
				ActiveTimerSet cancelTimers_;
		};
	}
}

#endif


