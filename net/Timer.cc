//2016.9.11
//qiangwei.su

#include "Timer.h"


using namespace summer::net;
using namespace summer;

AtomicInt64 Timer::numCreated_;

void Timer::restart(Timestamp now)
{
	if(repeat_)
	{
		expiration_ = addTime(now, interval_);
	}
	else
	{
		expiration_ = Timestamp::invalid();
	}
}


