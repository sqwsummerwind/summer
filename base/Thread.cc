//2016.9.10
//qiangwei.su
//
//
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

#include "Thread.h"

namespace summer
{
	namespace CurrentThread
	{
		__thread const char* t_threadName_ = "unknow";
	}
}

namespace
{	
	__thread pid_t t_cachedTid_ = 0;

	pid_t gettid()
	{
		return static_cast<pid_t>(::syscall(SYS_gettid));
	}

}

using namespace summer;

pid_t CurrentThread::tid()
{
	if(t_cachedTid_ == 0)
	{
		t_cachedTid_ = gettid();
	}

	return t_cachedTid_;
}

const char* CurrentThread::threadName()
{
	return t_threadName_;
}

bool CurrentThread::isMainThread()
{
	return tid() == ::getpid();
}

AtomicInt32 Thread::numCreated_;

Thread::Thread(const ThreadFunc& func, const std::string& name)
:started_(false),
pthreadId_(0),
tid_(0),
func_(func),
name_(name)
{
	numCreated_.increment();
}

Thread::~Thread()
{
}

void Thread::start()
{
	assert(!started_);
	started_ = true;
	pthread_create(&pthreadId_, NULL, &startThread, this);
}

void Thread::join()
{
	assert(started_);
	pthread_join(pthreadId_, NULL);
	started_ = false;
}

void* Thread::startThread(void* obj)
{
	Thread *thread = static_cast<Thread*>(obj);
	thread->runInThread();
	return NULL;
}

void Thread::runInThread()
{	
	tid_ = CurrentThread::tid();
	CurrentThread::t_threadName_ = name_.c_str(); 
	func_();
	CurrentThread::t_threadName_ = "finished";
}


