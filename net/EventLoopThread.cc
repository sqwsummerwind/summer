//2016.9.13
//qiangwei.su
//

#include <boost/bind.hpp>

#include "EventLoopThread.h"
#include "EventLoop.h"

using namespace summer;
using namespace summer::net;

EventLoopThread::EventLoopThread(const ThreadInitCallback& cb, const std::string& name)
:loop_(NULL),
mutex_(),
cond_(mutex_),
exiting_(false),
callback_(cb),
name_(name),
thread_(boost::bind(&EventLoopThread::threadFunc, this), name_)
{
	
}

EventLoopThread::~EventLoopThread()
{
	exiting_ = true;
	if(loop_)
	{
		loop_->quit();
		thread_.join();
	}
}

EventLoop* EventLoopThread::startLoop()
{
	assert(!thread_.started());

	thread_.start();

	{
		MutexLockGuard lock(mutex_);
		while(loop_ == NULL)
		{
			cond_.wait();
		}
	}

	return loop_;
}


void EventLoopThread::threadFunc()
{
	EventLoop loop;
	
	if(callback_)
	{
		callback_(&loop);
	}

	{
		MutexLockGuard lock(mutex_);
		loop_ = &loop;
		cond_.notify();
	}
	
	loop.loop();

	loop_ = NULL;
}


