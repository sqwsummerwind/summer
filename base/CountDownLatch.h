//2016.9,9 qiangwei.su
//

#ifndef SUMMER_BASE_COUNTDOWNLATCH_H
#define SUMMER_BASE_COUNTDOWNLATCH_H
#include <boost/noncopyable.hpp>

#include "Condition.h"
#include "Mutex.h"

namespace summer
{
	class CountDownLatch : boost::noncopyable
	{
		public:
			CountDownLatch(int count);
			
			void wait();
			int getCount() const;
			void countDown(); 
		private:
			mutable MutexLock mutex_;
			Condition condition_;
			int count_;
	};
}

#endif


