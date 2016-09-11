//2016.9.11
//qiangwei.su
//
#include <assert.h>
#include <poll.h>

#include "EventLoop.h"
#include "Poller.h"
#include "Channel.h"
#include "../base/Thread.h"
#include "../base/Logging.h"

using namespace summer;
using namespace summer::net;

namespace
{
	//forbid having two eventloop obj
	__thread EventLoop* t_loopInThisThread_ = 0;
}

EventLoop::EventLoop()
: looping_(false),
  threadId_(CurrentThread::tid()),
poller_(new Poller(this)),
quit_(false)
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
}

EventLoop::~EventLoop()
{
	assert(!looping_);
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
		for(ChannelList::const_iterator it = activeChannels_.begin();it<activeChannels_.end();it++)
		{
			(*it)->handleEvent();
		}
	}

	LOG_TRACE << "EventLoop "<<this << " stop looping";

	looping_ = false;

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

void EventLoop::assertNotInLoopThread()
{
		LOG_FATAL << "need to loop in thread = "<< threadId_;
}

EventLoop* EventLoop::getEventOfCurrentThread()

{
	return t_loopInThisThread_;
}






