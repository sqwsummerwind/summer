//2016.9.11
//qiangwei.su
//
#include <assert.h>
#include <poll.h>
#include <sys/eventfd.h>
#include <unistd.h>

#include <boost/bind.hpp>

#include "EventLoop.h"
#include "Poller.h"
#include "Channel.h"
#include "TimerQueue.h"
#include "TimerId.h"

#include "../base/Timestamp.h"
#include "../base/Thread.h"
#include "../base/Logging.h"

using namespace summer;
using namespace summer::net;

namespace
{
	//forbid having two eventloop obj
	__thread EventLoop* t_loopInThisThread_ = 0;

	int create_eventfd()
	{
		int eventfd_ = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
		if(eventfd_<0)
		{
			LOG_SYSFATAL <<" eventfd create ";
		}
		return eventfd_;
	}
}

EventLoop::EventLoop():
looping_(false),
threadId_(CurrentThread::tid()),
poller_(Poller::newDefaultPoller(this)),
wakeupFd_(create_eventfd()),
wakeupChannel_(new Channel(this, wakeupFd_)),
callingPendingFunctor_(false),
quit_(false),
timerQueue_(new TimerQueue(this))
{
	LOG_TRACE << "EventLoop created " << this << "in thread " << threadId_;
	
	if(t_loopInThisThread_)
	{
		//this thread has event loop obj already
		LOG_FATAL << "Another EventLoop "<< t_loopInThisThread_
				<< "exits in thread " << threadId_;
	}
	else
	{
		t_loopInThisThread_ = this;
	}

	wakeupChannel_ ->setReadCallback(boost::bind(&EventLoop::handleRead, this));
	wakeupChannel_ ->enableReading();

}



EventLoop::~EventLoop()
{
	assert(!looping_);
	close(wakeupFd_);
	t_loopInThisThread_ = NULL;
}

void EventLoop::loop()
{
	assert(!looping_);
	assertInLoopThread();
	looping_=true;

	quit_=false;

	while(!quit_)
	{
		
		activeChannels_.clear();
		poller_->poll(5*1000, &activeChannels_);
		LOG_DEBUG<<"wakeup from poll, activeChannels size:"<<
				activeChannels_.size()<<" pendingFunctor size :"
				<<pendingFunctors_.size();
		for(ChannelList::const_iterator it = activeChannels_.begin();
						it<activeChannels_.end(); it++)
		{
			(*it)->handleEvent(Timestamp::now());
		}

		doPendingFunctor();
	}

	LOG_TRACE << "EventLoop "<<this << " stop looping";

	looping_ = false;

}

void EventLoop::runInLoop(const Functor& functor)
{
	if(isInLoopThread())
	{
		functor();
	}
	else
	{
		queueInLoop(functor);
		wakeup();
	}
}

void EventLoop::queueInLoop(const Functor& functor)
{	
	{
		MutexLockGuard lock(mutex_);
		pendingFunctors_.push_back(functor);
	}

	if (isInLoopThread()&&callingPendingFunctor_)
	{
		LOG_TRACE<<"wakeup";
		wakeup();
	}
}

void EventLoop::doPendingFunctor()
{
	std::vector<Functor> functors;
	
	callingPendingFunctor_ = true;

	{
		MutexLockGuard lock(mutex_);
		functors.swap(pendingFunctors_);
	}

	for(int i=0;i<functors.size();i++)
	{
		functors[i]();
	}

	callingPendingFunctor_ = false;
}

void EventLoop::handleRead()
{
	uint64_t one;
	ssize_t n = ::read(wakeupFd_, &one, sizeof(one));
	LOG_DEBUG<<"wake up";
	if(n != sizeof(one))
	{
		LOG_SYSERR<<"handleRead read "<< n <<"bytes instead of 8";
	}
}

void EventLoop::wakeup()
{
	uint64_t one = 1;	//8 bytes
	ssize_t n = ::write(wakeupFd_, &one, sizeof(one));
	LOG_DEBUG<<"wakeupfd: "<<wakeupFd_;
	if(n < 8)
	{
		LOG_SYSERR << "wakeup write "<<n<<" bytes instead of 8";
	}
}

void EventLoop::quit()
{
	quit_ = true;
}

void EventLoop::updateChannel(Channel* channel)
{
	assert(channel->ownerLoop()==this);
	assertInLoopThread();
	poller_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel* channel)
{
	assert(channel->ownerLoop() == this);
	assertInLoopThread();
	poller_->removeChannel(channel);
}

void EventLoop::assertNotInLoopThread()
{
		LOG_FATAL << "need to loop in thread = "<< threadId_;
}

EventLoop* EventLoop::getEventOfCurrentThread()

{
	return t_loopInThisThread_;
}

TimerId EventLoop::runAt(const Timestamp when, const TimerCallback& cb)
{
	return timerQueue_->addTimer(cb, when, 0);
} 

TimerId EventLoop::runAfter(double delay, const TimerCallback& cb)
{
	Timestamp time(addTime(Timestamp::now(), delay));
	return runAt(time, cb);
}

TimerId EventLoop::runEvery(double interval, const TimerCallback& cb)
{
	Timestamp time(addTime(Timestamp::now(), interval));
	return timerQueue_->addTimer(cb, time, interval);
}

void EventLoop::cancelTimer(TimerId timerId)
{
	timerQueue_->cancel(timerId);
}
