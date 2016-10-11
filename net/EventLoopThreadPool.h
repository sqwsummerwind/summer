//2016.9.23
//qiangwei.su
//
#ifndef SUMMER_NET_EVENTLOOPTHREADPOOL_H
#define SUMMER_NET_EVENTLOOPTHREADPOOL_H

#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

namespace summer
{
namespace net
{
	class EventLoop;
	class EventLoopThread;

	class EventLoopThreadPool : public boost::noncopyable
	{
		public:
			EventLoopThreadPool(EventLoop* loop);
			~EventLoopThreadPool();
			void setThreadNum(int numThreads)
			{
				numThreads_ = numThreads;
			}
			void start();
			bool started();
			EventLoop* getNextLoop();
		private:
			EventLoop* baseLoop_;
			bool started_;
			int numThreads_;
			int next_;
			std::vector<EventLoopThread*> threads_;
			//boost::ptr_vector<EventLoopThread> threads_;
			//for one loop per thread, we can ignore the free of loop_
			std::vector<EventLoop*> loops_;
	};
}
}

#endif


