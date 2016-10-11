//2016.9.13
//qiangwei.su
//

#ifndef SUMMER_NET_EVENTLOOPTHREAD_H
#define SUMMER_NET_EVENTLOOPTHREAD_H

#include <string.h>

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

#include "../base/Mutex.h"
#include "../base/Thread.h"
#include "../base/Condition.h"

namespace summer
{
	namespace net
	{
		class EventLoop;

		class EventLoopThread : boost::noncopyable
		{
			public:
				typedef boost::function<void(EventLoop*)> ThreadInitCallback;

				EventLoopThread(const ThreadInitCallback& cb = ThreadInitCallback(),
								const std::string& name = std::string());
				~EventLoopThread();
				EventLoop* startLoop();

			private:

				void threadFunc();
				
				EventLoop* loop_;
				MutexLock mutex_;
				Condition cond_;
				bool exiting_;
				const ThreadInitCallback callback_;
				const std::string name_;
				Thread thread_;
		};
	}
}



#endif


