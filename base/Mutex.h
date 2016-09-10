//2016.9.8
//used for lock 
#ifndef SUMMER_BASE_MUTEX_H
#define SUMMER_BASE_MUTEX_H

#include <pthread.h>
#include <boost/noncopyable.hpp>

namespace summer
{

	class MutexLock: boost::noncopyable
	{
		public:
				
			MutexLock()
			{
				pthread_mutex_init(&mutex, NULL);
			}
			

			~MutexLock()
			{
				pthread_mutex_destroy(&mutex);
			}

			void lock()
			{
				pthread_mutex_lock(&mutex);
			}

			void unlock()
			{
				pthread_mutex_unlock(&mutex);
			}

			pthread_mutex_t* getPthreadMutex()
			{
				return &mutex;
			}

		private:
			pthread_mutex_t mutex;
	};

	class MutexLockGuard: boost::noncopyable
	{
		public:
			explicit MutexLockGuard(MutexLock& mutexlock):mutexlock_(mutexlock)
			{
				mutexlock_.lock();
			}

			~MutexLockGuard()
			{
				mutexlock_.unlock();
			}

		private:

			MutexLock& mutexlock_;
	};

}
#endif
