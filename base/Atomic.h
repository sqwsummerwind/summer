//2016.9.10
//qiangwei.su
//

#ifndef SUMMER_BASE_ATOMIC_H
#define SUMMER_BASE_ATOMIC_H

#include <stdint.h>

#include <boost/noncopyable.hpp>

namespace summer
{
	namespace wind
	{	
		template<typename T>
		class AtomicIntegerT : public boost::noncopyable
		{
			public:
				AtomicIntegerT():value_(0){}

				T get()
				{
					return __sync_val_compare_and_swap(&value_, 0, 0);
				}

				T getAndAdd(T x)
				{
					return __sync_fetch_and_add(&value_, x);
				}

				T addAndGet(T x)
				{
					return getAndAdd(x)+x;
				}

				T getAndIncrement()
				{
					return getAndAdd(1);
				}

				T incrementAndGet()
				{
					return addAndGet(1);
				}

				void increment()
				{
					incrementAndGet();
				}

				void decrement()
				{
					getAndAdd(-1);
				}

				void add(T x)
				{
					getAndAdd(x);
				}

				T getAndSet(T newVal)
				{
					return __sync_lock_test_and_set(&value_, newVal);
				}
			private:
				volatile T value_;
		};


	}
		typedef wind::AtomicIntegerT<int64_t> AtomicInt64;
		typedef wind::AtomicIntegerT<int32_t> AtomicInt32;
}


#endif

