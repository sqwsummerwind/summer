//2016.9.11
//qiangwei.su
//

#ifndef SUMMER_NET_EVENTLOOP_H
#define SUMMER_NET_EVENTLOOP_H

#include <vector>
#include <pthread.h>

#include <boost/function.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/noncopyable.hpp>

#include "TimerId.h"

#include "../base/Thread.h"
#include "../base/Mutex.h"
#include "../base/Timestamp.h"

namespace summer
{
	namespace net
	{	
		class Poller;
		class Channel;
		class TimerQueue;
		//class TimerId;

		class EventLoop : boost::noncopyable
		{
			public:
				typedef boost::function<void()> Functor;
				typedef boost::function<void()> TimerCallback;

				EventLoop();
				~EventLoop();
				void loop();
				void assertInLoopThread()
				{
					if(!isInLoopThread())
					{
						assertNotInLoopThread();
					}
				}

				bool isInLoopThread()
				{
					return threadId_ == CurrentThread::tid();
				}
				
				static EventLoop* getEventOfCurrentThread();
				void updateChannel(Channel* channel);
				void removeChannel(Channel* channel);
				void quit();

				void runInLoop(const Functor& functor);
				void queueInLoop(const Functor& functor);
				void wakeup();

				//for timerqueue
				TimerId runAt(const Timestamp when, const TimerCallback& cb);
				TimerId runAfter(double delay, const TimerCallback& cb);
				TimerId runEvery(double interval, const TimerCallback& cb);
				void cancelTimer(TimerId timerId);

			private:
				void assertNotInLoopThread();
				void handleRead();	//handle wake up
				void doPendingFunctor();

				typedef std::vector<Channel*> ChannelList;

				ChannelList activeChannels_;
				boost::scoped_ptr<Poller> poller_;
				int wakeupFd_;	//eventfd
				//for wake up
				boost::scoped_ptr<Channel> wakeupChannel_;

				bool looping_;
				bool quit_;
				const pid_t threadId_;
				bool callingPendingFunctor_;
				MutexLock mutex_;
				std::vector<Functor> pendingFunctors_;	//	

				boost::scoped_ptr<TimerQueue> timerQueue_;
		};
	}
}

#endif


