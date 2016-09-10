//2016.9.10
//qiangwei.su
//

#ifndef SUMMER_BASE_THREAD_H
#define SUMMER_BASE_THREAD_H

#include <syscall.h>
#include <string.h>
#include <pthread.h>

#include <boost/noncopyable.hpp>
#include <boost/function.hpp>

#include "Atomic.h"

namespace summer
{
	class Thread: public boost::noncopyable
	{
		public:
			typedef boost::function<void ()> ThreadFunc;
			
			Thread(const ThreadFunc& func, const std::string& name = std::string());
			~Thread();

			void start();
			void join();

			pid_t tid() const
			{
				return tid_;
			}

			bool started() const
			{
				return started_;
			}

			const std::string& name() const
			{
				return name_;
			}

			static int numCreated()
			{
				return numCreated_.get();
			}

		private:
			
			static void* startThread(void* obj);
			void runInThread();

			bool started_;
			pthread_t pthreadId_;
			pid_t tid_;
			ThreadFunc func_;
			std::string name_;

			static AtomicInt32 numCreated_;
	};

	namespace CurrentThread
	{
		pid_t tid();
		const char* threadName();
		bool isMainThread();

	}
}

#endif

