//2016.9.12
//qiangwei.su

#include <sys/timerfd.h>

#include <boost/bind.hpp>

#include "EventLoop.h"
#include "TimerQueue.h"
#include "Timer.h"
#include "TimerId.h"

#include "../base/Logging.h"
#include "../base/Timestamp.h"


namespace
{
	using namespace summer;

	int create_timerfd()
	{
		int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
		if(timerfd < 0)
		{
			LOG_FATAL<<"timerfd create failed";
		}
		return timerfd;
	}

	struct timespec howLongToExpiration(Timestamp when)
	{
		int64_t microSeconds = when.microSecondSinceEpoch() - Timestamp::now().microSecondSinceEpoch();
		
		time_t t_sec = static_cast<time_t>(microSeconds / Timestamp::kMicroSecondPerSecond);
		long t_nsec = static_cast<long>((microSeconds % Timestamp::kMicroSecondPerSecond)*1000);

		struct timespec ts;
		ts.tv_sec = t_sec;
		ts.tv_nsec = t_nsec;

		return ts;
	}

	void readTimerfd(int timerfd)
	{
		uint64_t howlong = 1;
		ssize_t n = ::read(timerfd, &howlong, sizeof(howlong));
		
		LOG_TRACE <<"TimerQueue::handleRead "<< howlong << " at ";
		if(n != 8)
		{
			LOG_ERROR << "Timerfd read "<<n<<" bytes instead of 8";
		}
	}

	void resetTimerfd(int timerfd, Timestamp when)
	{
		struct timespec it_val = howLongToExpiration(when);

		struct itimerspec new_value;
		struct itimerspec old_value;

		bzero(&new_value, sizeof(new_value));
		bzero(&old_value, sizeof(old_value));

		new_value.it_value = it_val;
		
		int n = ::timerfd_settime(timerfd, 0, &new_value, &old_value);

		if(n<0)
		{
			LOG_SYSERR<<"timerfd_settime()";
		}

	}
}

using namespace summer;
using namespace summer::net;

TimerQueue::TimerQueue(EventLoop* loop)
:loop_(loop),
timerfd_(create_timerfd()),
timerfdChannel_(loop_, timerfd_),
timers_(),
doingExpiredTimers_(false)
{	
	LOG_INFO<<"timerfd: "<<timerfdChannel_.fd();
	timerfdChannel_.setReadCallback(boost::bind(&TimerQueue::handleRead, this));
	timerfdChannel_.enableReading();
}

void TimerQueue::handleRead()
{
	
	loop_->assertInLoopThread();

	Timestamp now(Timestamp::now());

	readTimerfd(timerfd_);

	std::vector<Entry> expired = getExpired(now);

	std::vector<Entry>::iterator it;

	doingExpiredTimers_ = true;
	cancelTimers_.clear();
	for(it = expired.begin();it != expired.end();it++)
	{
		it->second->run();
	}
	doingExpiredTimers_ = false;

	reset(expired, now);
}

std::vector<TimerQueue::Entry> TimerQueue::getExpired(Timestamp now)
{
	assert(timers_.size() == activeTimers_.size());

	TimerList::iterator lower_it =
			timers_.lower_bound(Entry(now, reinterpret_cast<Timer*>(UINTPTR_MAX)));
	
	Timestamp first = lower_it->first;
	assert(lower_it != timers_.end() || now < first);

	std::vector<Entry> expired;
	std::copy(timers_.begin(), lower_it, std::back_inserter(expired));
	/*for(TimerList::iterator it = timers_.begin(); it!=lower_it; it++)
	{
		expired.push_back((*it));
	}*/

	//delete expiration in timers_
	timers_.erase(timers_.begin(), lower_it);

	for(std::vector<Entry>::iterator it = expired.begin(); it != expired.end(); it++)
	{
		//ActiveTimerSet::iterator active_it =
				//activeTimers_.find(Active(it->second, it->second->sequence()));
		
		Active active(it->second, it->second->sequence());
		ssize_t n = activeTimers_.erase(active);
		assert(n == 1);
		(void)active;

	}

	assert(timers_.size() == activeTimers_.size());
	return expired;

}

void TimerQueue::reset(std::vector<Entry>& expired, Timestamp now)
{
	
	
	for(std::vector<Entry>::iterator it = expired.begin();it != expired.end(); it++)
	{
		Active active(it->second, it->second->sequence());
		if(it->second->repeat() && cancelTimers_.find(active) == cancelTimers_.end())
		{
			//Timestamp when = addTime(Timestamp::now, it->second->interval());
			it->second->restart(now);
			insert(it->second);
		}
		else
		{
			delete it->second;
		}
	}

	Timestamp nextExpired;
	if(!timers_.empty())
	{
		nextExpired = timers_.begin()->first;
	}

	if(nextExpired.valid())
	{
		resetTimerfd(timerfd_, nextExpired);
	}

}

void TimerQueue::cancel(TimerId timerId)
{
	loop_->runInLoop(boost::bind(&TimerQueue::cancelInLoop, this, timerId));
}

void TimerQueue::cancelInLoop(TimerId timerId)
{
	loop_->assertInLoopThread();
	assert(timers_.size() == activeTimers_.size());
	
	Active active(timerId.timer_, timerId.sequence_);
	ActiveTimerSet::iterator it = activeTimers_.find(active);
	
	Entry entry(it->first->expiration(), it->first);
	if(it != activeTimers_.end())
	{	
		ssize_t n = timers_.erase(entry);
		assert(n == 1);
		(void)entry;

		delete it->first;

		activeTimers_.erase(it);
	}
	else if(doingExpiredTimers_)
	{
		cancelTimers_.insert((*it));
	}

	assert(timers_.size() == activeTimers_.size());
}

TimerId TimerQueue::addTimer(const TimerCallback& cb, Timestamp when, double interval)
{
	Timer* timer = new Timer(cb, when, interval);
	TimerId timerId(timer, timer->sequence());

	loop_->runInLoop(boost::bind(&TimerQueue::addTimerInLoop, this, timer));

	return timerId;
}

void TimerQueue::addTimerInLoop(Timer* timer)
{
	loop_->assertInLoopThread();

	bool earliestTimerChanged = insert(timer);

	if(earliestTimerChanged)
	{
		resetTimerfd(timerfd_, timer->expiration());
	}
}

bool TimerQueue::insert(Timer* timer)
{
	loop_->assertInLoopThread();

	assert(timers_.size() == activeTimers_.size());

	bool earliestTimerChanged = false;
	Timestamp when = (*timer).expiration();
	TimerList::iterator it = timers_.begin();
	Timestamp first = it->first;
	if(it == timers_.end() || when < first )
	{
		earliestTimerChanged = true;
	}

	{
		std::pair<TimerList::iterator, bool> ret 
				= timers_.insert(Entry(when, timer));
		assert(ret.second);
		(void)ret;
	}

	{
		std::pair<ActiveTimerSet::iterator, bool> ret
				= activeTimers_.insert(Active(timer, timer->sequence()));
		assert(ret.second);
		(void)ret;
	}

	assert(activeTimers_.size() == timers_.size());
	return earliestTimerChanged;
}


