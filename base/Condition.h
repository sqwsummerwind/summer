//条件变量
//2016.9.9
//
#ifndef SUMMER_BASE_CONDITION_H
#define SUMMER_BASE_CONDITION_H
#include <pthread.h>

#include <boost/noncopyable.hpp>

#include "Mutex.h"


namespace summer
{
	class Condition : boost::noncopyable
	{
		public:
			Condition(MutexLock& mutex) : mutex_(mutex)
			{
				pthread_cond_init(&cond_, NULL);
			}

			void wait()
			{
				pthread_cond_wait(&cond_, mutex_.getPthreadMutex());
			}

			void notify()
			{
				pthread_cond_signal(&cond_);
			}

			void notifyAll()
			{
				pthread_cond_broadcast(&cond_);
			}

			~Condition()
			{
				pthread_cond_destroy(&cond_);
			}
		private:
			pthread_cond_t cond_;
			MutexLock& mutex_;

	};
}
#endif
