#include <assert.h>

#include "Atomic.h"

int main()
{
	{
		summer::AtomicInt32 i32;
		assert(i32.get() == 0);
		assert(i32.getAndAdd(3) == 0);
		assert(i32.get() == 3);
		assert(i32.addAndGet(4) == 7);
		assert(i32.get() == 7);
		i32.add(4);
		assert(i32.get() == 11);
		i32.increment();
		assert(i32.get() == 12);
		assert(i32.getAndIncrement() == 12);
		assert(i32.get() == 13);
		assert(i32.incrementAndGet() == 14);
		i32.decrement();
		assert(i32.get() == 13);
	}
}


