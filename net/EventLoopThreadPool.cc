//2016.9.23
//qiangwei.su
//

#include <assert.h>

#include "EventLoopThreadPool.h"
#include "EventLoop.h"
#include "EventLoopThread.h"

using namespace summer;
using namespace summer::net;

EventLoopThreadPool::EventLoopThreadPool(EventLoop* loop):
baseLoop_(loop),
started_(false),
numThreads_(0),
next_(0)
{}

EventLoopThreadPool::~EventLoopThreadPool()
{
	for(int i =0;i<threads_.size();i++)
	{
		delete threads_[i];
	}
}

void EventLoopThreadPool::start()
{
	baseLoop_->assertInLoopThread();
	assert(!started_);
	started_ = true;
	for(int i=0; i<numThreads_; i++)
	{
		EventLoopThread* eventLoopThread = new EventLoopThread();
		threads_.push_back(eventLoopThread);
		loops_.push_back(eventLoopThread->startLoop());
	}	
}

bool EventLoopThreadPool::started()
{
	return started_;
}

EventLoop* EventLoopThreadPool::getNextLoop()
{
	baseLoop_->assertInLoopThread();
	EventLoop* loop = baseLoop_;
	assert(next_>=0 && next_<=numThreads_);
	
	if(!loops_.empty())
	{
		loop = loops_[next_];
		++next_;
		if(next_ >= loops_.size())
		{
			next_ = 0;
		}
	}

	return loop;
}


